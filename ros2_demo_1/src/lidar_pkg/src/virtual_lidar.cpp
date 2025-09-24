#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/point_cloud2.hpp"
#include "sensor_msgs/point_cloud2_iterator.hpp"
#include "std_msgs/msg/string.hpp"

#include <random>


class VirtualLidar : public rclcpp::Node {
public:
    VirtualLidar(std::string name) : Node(name) {
        pub_ = this->create_publisher<sensor_msgs::msg::PointCloud2>("lidar/points", 10);
        log_pub_ = this->create_publisher<std_msgs::msg::String>("system/log", 10);

        std_msgs::msg::String msg;
        msg.data = "[VirtualLidar] [Node " + name + "] 创建";
        log_pub_->publish(msg);

        using namespace std::chrono_literals;
        timer_ = this->create_wall_timer(1000ms, std::bind(&VirtualLidar::publish_cloud, this));
    }

private:
    void publish_cloud() {
        // 随机生成点云
        auto cloud = sensor_msgs::msg::PointCloud2();
        cloud.header.stamp = this->now();
        cloud.header.frame_id = "lidar";

        const int num_points = 1000; 
        cloud.height = 1;
        cloud.width = num_points;

        sensor_msgs::PointCloud2Modifier modifier(cloud);
        modifier.setPointCloud2FieldsByString(1, "xyz"); 
        modifier.resize(num_points);

        sensor_msgs::PointCloud2Iterator<float> iter_x(cloud, "x");
        sensor_msgs::PointCloud2Iterator<float> iter_y(cloud, "y");
        sensor_msgs::PointCloud2Iterator<float> iter_z(cloud, "z");

        for (int i = 0; i < num_points; ++i, ++iter_x, ++iter_y, ++iter_z) {
            *iter_x = static_cast<float>(std::rand() % 100) / 10.0f; 
            *iter_y = static_cast<float>(std::rand() % 100) / 10.0f; 
            *iter_z = static_cast<float>(std::rand() % 50) / 10.0f;  
        }

        pub_->publish(cloud);

        // 发送日志信息
        std_msgs::msg::String log_msg;
        std::stringstream s;

        s << "[VirtualLidar] 雷达发送一张点云 "
        << "(points: " << num_points
        << ", height: " << cloud.height
        << ", width: " << cloud.width
        << ", frame_id: " << cloud.header.frame_id << ")";

        log_msg.data = s.str();
        log_pub_->publish(log_msg);
    }
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr pub_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr log_pub_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<VirtualLidar>("virtual_lidar");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
