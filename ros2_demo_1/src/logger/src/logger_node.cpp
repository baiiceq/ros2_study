#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include <fstream>

class SystemLogger : public rclcpp::Node {
public:
    SystemLogger(std::string name) : Node(name) {
        sub_ = this->create_subscription<std_msgs::msg::String>(
            "system/log", 10,
            std::bind(&SystemLogger::callback, this, std::placeholders::_1));
	
	// 打开日志文件
        log_file_.open("logs/system_log.txt", std::ios::app);
        if (!log_file_.is_open()) {
            RCLCPP_ERROR(this->get_logger(), "Failed to open log file!");
        }
    }

    ~SystemLogger() {
        if (log_file_.is_open()) {
            log_file_.close();
        }
    }

private:
    void callback(const std_msgs::msg::String::SharedPtr msg) {
    	// 终端打印日志
        RCLCPP_INFO(this->get_logger(), "LOG: %s", msg->data.c_str());
	
	// 文件输出日志
        if (log_file_.is_open()) {
            log_file_ << msg->data << std::endl;
        }
    }

    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_;
    std::ofstream log_file_;
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<SystemLogger>("system_logger");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
