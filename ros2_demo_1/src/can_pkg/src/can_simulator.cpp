#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include <cstdlib>
#include <ctime>
#include <sstream>

class CanSimulator : public rclcpp::Node {
public:
    CanSimulator(std::string name) : Node(name) {
        pub_ = this->create_publisher<std_msgs::msg::String>("can/bus", 10);
        log_pub_ = this->create_publisher<std_msgs::msg::String>("system/log", 10);

        std_msgs::msg::String msg;
        msg.data = "[CanSimulator] [Node " + name + "] 创建";
        log_pub_->publish(msg);

        std::srand(std::time(nullptr));
        using namespace std::chrono_literals;
        timer_ = this->create_wall_timer(1000ms, std::bind(&CanSimulator::publish_can_frame, this));
    }

private:
    void publish_can_frame() {
        // 模拟 CAN 帧
        int can_id = std::rand() % 2048; 
        int data = std::rand() % 256;

        std_msgs::msg::String msg;
        std::stringstream ss;
        ss << "发送can帧: " << "id=0x" << std::hex << can_id 
           << " data=0x" << std::hex << data;
        msg.data = ss.str();
        pub_->publish(msg);

        // 日志
        std_msgs::msg::String log_msg;
        log_msg.data = "[CanSimulator] 发送: " + msg.data;
        log_pub_->publish(log_msg);
    }

    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr log_pub_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<CanSimulator>("can_simulator");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
