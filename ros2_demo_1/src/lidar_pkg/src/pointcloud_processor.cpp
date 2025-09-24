#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/point_cloud2.hpp"
#include "std_msgs/msg/string.hpp"
#include "sensor_msgs/point_cloud2_iterator.hpp"

class PointCloudProcessor : public rclcpp::Node {
public:
    PointCloudProcessor(std::string name) : Node(name) {
        sub_ = this->create_subscription<sensor_msgs::msg::PointCloud2>(
            "lidar/points", 10,
            std::bind(&PointCloudProcessor::process_point_cloud, this, std::placeholders::_1));
        log_pub_ = this->create_publisher<std_msgs::msg::String>("system/log", 10);

        std_msgs::msg::String msg;
        msg.data = "[PointCloudProcessor] [Node " + name + "] 创建";
        log_pub_->publish(msg);
    }

private:
    void process_point_cloud(const sensor_msgs::msg::PointCloud2::SharedPtr msg) {
        if (!msg || msg->data.empty()) return;

        sensor_msgs::PointCloud2Iterator<float> iter_x(*msg, "x");
        sensor_msgs::PointCloud2Iterator<float> iter_y(*msg, "y");
        sensor_msgs::PointCloud2Iterator<float> iter_z(*msg, "z");

        float z_max = std::numeric_limits<float>::lowest();
        float z_min = std::numeric_limits<float>::max();
        float z_sum = 0.0;
        int count = 0;

        for (; iter_x != iter_x.end(); ++iter_x, ++iter_y, ++iter_z) {
            float z = *iter_z;
            if (z > z_max) z_max = z;
            if (z < z_min) z_min = z;
            z_sum += z;
            count++;
        }

        float z_avg = count > 0 ? z_sum / count : 0.0;

        // 简单分类模拟处理结果
        std::string height_category;
        if (z_avg < 1.5) height_category = "low";
        else if (z_avg < 3.0) height_category = "medium";
        else height_category = "high";

        // 日志输出
        std::stringstream s;
        std_msgs::msg::String log_msg;
        s << "[PointCloudProcessor] 点云处理结果: " << count
        << ", z_min: " << z_min
        << ", z_max: " << z_max
        << ", z_avg: " << z_avg
        << ", category: " << height_category;

        log_msg.data = s.str();
        log_pub_->publish(log_msg);
    }
    rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr sub_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr log_pub_;
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<PointCloudProcessor>("pointcloud_processor");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
