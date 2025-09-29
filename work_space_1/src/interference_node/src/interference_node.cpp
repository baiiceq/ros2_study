#include "rclcpp/rclcpp.hpp"
#include <chrono>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <thread>
#include <vector>

class InterferenceNode : public rclcpp::Node {
public:
  InterferenceNode(size_t buffer_mb, size_t stride_bytes_param, size_t threads)
      : Node("interference_node") {
    // 初始化数据
    _buffer_size_bytes = buffer_mb * 1024 * 1024;
    _stride_bytes = stride_bytes_param ? stride_bytes_param : 4096;
    _threads = threads;
      
    RCLCPP_INFO(this->get_logger(),
                "InterferenceNode created: buffer=%zu MB, stride=%zu, threads=%zu",
                buffer_mb, _stride_bytes, _threads);

    _buffer.resize(_buffer_size_bytes, 0);

    // 启动多个线程执行干扰任务
    for (size_t t = 0; t < _threads; t++) {
      _interferences.emplace_back([this]() { this->memory_stress_work(); });
    }
  }

  ~InterferenceNode() {
    _running = false;
    for (auto &w : _interferences) {
      if (w.joinable()) w.join();
    }
  }

private:
  void memory_stress_work() {
    volatile uint8_t *p = _buffer.data();
    size_t sz = _buffer.size();
    size_t step = _stride_bytes;

    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<size_t> dist(0, sz - 1);

    while (_running) {
      // 随机访问
      for (size_t i = 0; i < sz / step; i++) {
        size_t idx = (dist(rng) / step) * step; 
        uint8_t v = p[idx];
        p[idx] = (uint8_t)(v + 1);
      }
    }
  }

  bool _running = true;                     // 干扰函数运行
  size_t _buffer_size_bytes;                // buffer总占内存数
  size_t _stride_bytes;                     // 跨步大小
  size_t _threads;                          // 线程数
  std::vector<uint8_t> _buffer;             // 大数组
  std::vector<std::thread> _interferences;  // 干扰线程组
};


int main(int argc, char **argv) {
  rclcpp::init(argc, argv);

  size_t buffer_mb = 1024;    
  size_t stride = 4096;      
  size_t threads = 8;

  for (int i = 1; i < argc; ++i) {
    std::string a = argv[i];
    if (a == "--buffer-mb" && i + 1 < argc) buffer_mb = std::stoul(argv[++i]);
    else if (a == "--stride" && i + 1 < argc) stride = std::stoul(argv[++i]);
    else if (a == "--threads" && i + 1 < argc) threads = std::stoul(argv[++i]);
  }

  auto node = std::make_shared<InterferenceNode>(buffer_mb, stride, threads);
  rclcpp::spin(node);

  rclcpp::shutdown();
  return 0;
}
