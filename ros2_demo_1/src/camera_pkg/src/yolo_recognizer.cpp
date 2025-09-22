#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "std_msgs/msg/string.hpp"

class YoloRecognizer : public rclcpp::Node {
public:
     YoloRecognizer(std::string name) : Node(name) {
        sub_ = this->create_subscription<sensor_msgs::msg::Image>(
            "camera/image", 10,
            std::bind(&YoloRecognizer::image_callback, this, std::placeholders::_1));

        log_pub_ = this->create_publisher<std_msgs::msg::String>("system/log", 10);
        std_msgs::msg::String msg;
        msg.data = "[YoloRecognizer] [Node " + name + "] 创建";
        log_pub_->publish(msg);

    }

private:
    void image_callback(const sensor_msgs::msg::Image::SharedPtr msg) {
        std_msgs::msg::String result;

        if (msg->data.empty()) return;

        uint8_t value = msg->data[0]; 
        std::string color;

        if (value > 170) {
            color = "white";
        } else if (value > 85) {
            color = "gray";
        } else {
            color = "black";
        }

        std::stringstream s;
        s << "[YoloRecognizer] yolo recognizer [" << color << "] "
        << ", image value: " << static_cast<int>(value);

        result.data = s.str();
        log_pub_->publish(result);
    }

    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr sub_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr log_pub_;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<YoloRecognizer>("yolo_recognizer");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
