#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

class SystemMonitor : public rclcpp::Node {
public:
    SystemMonitor(std::string name) : Node(name) {
        sub_ = this->create_subscription<std_msgs::msg::String>(
            "can/bus", 10,
            std::bind(&SystemMonitor::callback, this, std::placeholders::_1));

        log_pub_ = this->create_publisher<std_msgs::msg::String>("system/log", 10);

        std_msgs::msg::String msg;
        msg.data = "[SystemMonitor] [Node " + name + "] 创建";
        log_pub_->publish(msg);
    }

private:
    void callback(const std_msgs::msg::String::SharedPtr msg) {
        std_msgs::msg::String log_msg;
        log_msg.data = "[SystemMonitor] 收到 can 帧 -> " + msg->data;
        log_pub_->publish(log_msg);

        if (msg->data.find("0xff") != std::string::npos) {
            std_msgs::msg::String log_msg;
            log_msg.data = "[SystemMonitor] 警告: 收到异常数据 -> " + msg->data;
            log_pub_->publish(log_msg);
        }
    }

    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr log_pub_;
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<SystemMonitor>("system_monitor");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
