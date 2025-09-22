#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"

#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace std::chrono_literals;

class VictimNode : public rclcpp::Node {
public:
  VictimNode(size_t buffer_mb, size_t stride, int period_ms) : Node("victim_node"),
    _buffer_size_bytes(buffer_mb * 1024 * 1024),
    _stride_bytes(stride),
    _period_ms(period_ms),
    _iter(0)
  {
    _publisher = this->create_publisher<std_msgs::msg::Float64>("latency", 10);

    RCLCPP_INFO(this->get_logger(),
                "VictimNode create : buffer=%ld MB, stride=%ld, period=%dms",
                buffer_mb, _stride_bytes, _period_ms);

    _buffer.resize(_buffer_size_bytes, 0);

    _timer = this->create_wall_timer(std::chrono::milliseconds(_period_ms),
                                     std::bind(&VictimNode::on_timer, this));
  }

private:
  void test_memory_workload() {
    size_t sz = _buffer.size();
    size_t step = _stride_bytes ? _stride_bytes : 256;

    // 使用volatile防止编译器优化内存访问
    volatile uint8_t *p = _buffer.data();

    for (size_t start = 0; start < step; start++) {
        for (size_t i = start; i < sz; i += step) {
            uint8_t v = p[i];
            p[i] = v + 1;
        }
    }
  }

  void on_timer() {
    _iter++;

    // 计算时间
    auto t_start = std::chrono::high_resolution_clock::now();
    test_memory_workload();
    auto t_end = std::chrono::high_resolution_clock::now();

    // 计算延迟
    long delay = std::chrono::duration_cast<std::chrono::microseconds>(t_end - t_start).count();

    auto msg = std_msgs::msg::Float64();
    msg.data = static_cast<double>(delay);
    _publisher->publish(msg);

    RCLCPP_INFO(this->get_logger(), "iter=%ld latency=%ld", _iter, delay);
  }

  size_t _buffer_size_bytes;
  size_t _stride_bytes;
  int _period_ms;
  std::string _result_path;
  std::vector<uint8_t> _buffer;
  uint64_t _iter;

  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr _publisher;
  rclcpp::TimerBase::SharedPtr _timer;
};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);

  size_t buffer_mb = 64;       
  size_t stride = 4096;        
  int period_ms = 50;        

  for (int i = 1; i < argc; ++i) {
    std::string a = argv[i];
    if (a == "--buffer-mb" && i + 1 < argc) { buffer_mb = std::stoul(argv[++i]); }
    else if (a == "--stride" && i + 1 < argc) { stride = std::stoul(argv[++i]); }
    else if (a == "--period-ms" && i + 1 < argc) { period_ms = std::stoi(argv[++i]); }
    else {
      std::cout << "Unknow arg\n";
      return 0;
    }
  }

  auto node = std::make_shared<VictimNode>(buffer_mb, stride, period_ms);
  rclcpp::spin(node);

  rclcpp::shutdown();
  return 0;
}
