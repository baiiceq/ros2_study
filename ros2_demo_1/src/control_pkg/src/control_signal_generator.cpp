#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

#include <string>
#include <random>

class ControlSignalGenerator : public rclcpp::Node {
public:
    ControlSignalGenerator(const std::string &name) : Node(name) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));

        pub_ = this->create_publisher<std_msgs::msg::String>("control/signals", 10);
        log_pub_ = this->create_publisher<std_msgs::msg::String>("system/log", 10);

        std_msgs::msg::String msg;
        msg.data = "[ControlSignalGenerator] [Node " + name + "] 创建成功";
        log_pub_->publish(msg);

        using namespace std::chrono_literals;
        timer_ = this->create_wall_timer(1000ms, std::bind(&ControlSignalGenerator::publish_signal, this));
    }

private:
    void publish_signal() {
        // 模拟几个控制信号
        static std::string signals[4] = {"FORWARD", "LEFT", "RIGHT", "STOP"};

        int idx = std::rand() % 4;
        
        std_msgs::msg::String msg;
        msg.data = signals[idx];
        pub_->publish(msg);

        // 日志
        std_msgs::msg::String log_msg;
        log_msg.data = "[ControlSignalGenerator] 发送控制信号: " + signals[idx];
        log_pub_->publish(log_msg);
    }

    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr log_pub_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<ControlSignalGenerator>("control_signal_generator");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
