#include "rclcpp/rclcpp.hpp"
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace std::chrono_literals;

class InterferenceNode : public rclcpp::Node {
public:
  InterferenceNode(size_t buffer_mb, size_t stride_bytes_param) : Node("interference_node")
  {
    _buffer_size_bytes = buffer_mb * 1024 * 1024;
    _stride_bytes = stride_bytes_param; 
    if (_stride_bytes == 0) {
        _stride_bytes = 4096; 
    }
    RCLCPP_INFO(this->get_logger(),
                "InterferenceNode created: buffer=%zu MB, stride=%zu", buffer_mb, _stride_bytes);

    _buffer.resize(_buffer_size_bytes, 0);

    _timer = this->create_wall_timer(std::chrono::microseconds(1),
                                     std::bind(&InterferenceNode::do_memory_intensive_work, this));
  }

private:
  void do_memory_intensive_work() {
    size_t sz = _buffer.size();
    size_t step = _stride_bytes; // 使用配置的步长

    // 使用volatile防止编译器优化内存访问
    volatile uint8_t *p = _buffer.data();

    for (size_t start = 0; start < step; ++start) {
        for (size_t i = start; i < sz; i += step) {
            uint8_t v = p[i];
            p[i] = v + 1; 
        }
    }
  }

  size_t _buffer_size_bytes;
  size_t _stride_bytes;
  std::vector<uint8_t> _buffer;
  rclcpp::TimerBase::SharedPtr _timer;
};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);

  size_t buffer_mb = 512;        
  size_t stride = 4096;      

  for (int i = 1; i < argc; ++i) {
    std::string a = argv[i];
    if (a == "--buffer-mb" && i + 1 < argc) { buffer_mb = std::stoul(argv[++i]); }
    else if (a == "--stride" && i + 1 < argc) { stride = std::stoul(argv[++i]); }
    else {
      std::cout << "Unknown arg\n";
      return 0;
    }
  }

  auto node = std::make_shared<InterferenceNode>(buffer_mb, stride);
  rclcpp::spin(node);

  rclcpp::shutdown();
  return 0;
}