#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "std_msgs/msg/string.hpp"

#include <random>

class VirtualCamera : public rclcpp::Node {
public:
    VirtualCamera(std::string name) : Node(name) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        image_pub_ = this->create_publisher<sensor_msgs::msg::Image>("camera/image", 10);
        log_pub_ = this->create_publisher<std_msgs::msg::String>("system/log", 10);

        std_msgs::msg::String msg;
        msg.data = "[VirtualCamera] [Node " + name + "] 创建";
        log_pub_->publish(msg);

        using namespace std::chrono_literals;
        timer_ = this->create_wall_timer(100ms, std::bind(&VirtualCamera::publish_image, this));
    }

private:
    void publish_image() {
        auto msg = sensor_msgs::msg::Image();
        msg.header.stamp = this->now();
        msg.height = 480;
        msg.width = 640;
        msg.encoding = "mono8";
        msg.step = 640;

        // 随机纯色照片颜色
        msg.data.resize(480 * 640, static_cast<uint8_t>(std::rand() % 256));
        image_pub_->publish(msg);

        std_msgs::msg::String log_msg;
        std::stringstream s;

        s << "[VirtualCamera] virtual camera publish a image : "
        << "(height: " << msg.height
        << ", width: " << msg.width
        << ", encoding: " << msg.encoding << ")";

        log_msg.data = s.str();
        log_pub_->publish(log_msg);
    }
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr image_pub_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr log_pub_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<VirtualCamera>("virtual_camera");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
