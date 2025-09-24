#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

class ControlDecision : public rclcpp::Node {
public:
    ControlDecision(const std::string &name) : Node(name) {
        sub_ = this->create_subscription<std_msgs::msg::String>(
            "control/signals", 10,
            std::bind(&ControlDecision::callback, this, std::placeholders::_1));

        log_pub_ = this->create_publisher<std_msgs::msg::String>("system/log", 10);

        // 日志
        std_msgs::msg::String msg;
        msg.data = "[ControlDecision] [Node " + name + "] 创建成功";
        log_pub_->publish(msg);

        state = "停止";
    }

private:
    void callback(const std_msgs::msg::String::SharedPtr msg) {
        std::string signal = msg->data;

        
        if (signal == "FORWARD") {
            state = "前进";
        } else if (signal == "LEFT") {
            state = "左转";
        } else if (signal == "RIGHT") {
            state = "右转";
        } else if (signal == "STOP") {
            state = "停止";
        } else {
            state = "未知";
        }

        // 日志输出
        std_msgs::msg::String log_msg;
        log_msg.data = "[ControlDecision] 收到控制信号: " + signal + " , 当前状态 : " + state;
        log_pub_->publish(log_msg);
    }

    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr log_pub_;
    std::string state;
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<ControlDecision>("control_decision");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
