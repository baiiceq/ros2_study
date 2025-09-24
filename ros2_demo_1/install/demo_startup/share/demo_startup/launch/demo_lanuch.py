from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package="can_pkg",   
            executable="can_simulator",
            name="can_simulator",
            output="screen"
        ),
        Node(
            package="can_pkg", 
            executable="system_monitor",
            name="system_monitor",
            output="screen"
        ),
        Node(
            package="logger",
            executable="logger_node",
            name="logger_node",
            output="screen"
        ),
        Node(
            package="camera_pkg",
            executable="virtual_camera",
            name="virtual_camera",
            output="screen"
        ),
        Node(
            package="camera_pkg",
            executable="yolo_recognizer",
            name="yolo_recognizer",
            output="screen"
        ),
        Node(
            package="lidar_pkg",
            executable="pointcloud_processor",
            name="pointcloud_processor",
            output="screen"
        ),
        Node(
            package="lidar_pkg",
            executable="virtual_lidar",
            name="virtual_lidar",
            output="screen"
        ),
        Node(
            package="control_pkg",
            executable="control_decision",
            name="control_decision",
            output="screen"
        ),
        Node(
            package="control_pkg",
            executable="control_signal_generator",
            name="control_signal_generator",
            output="screen"
        )
    ])

