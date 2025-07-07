#include <ros/ros.h>
#include <franka_msgs/SetLoad.h>

int main(int argc, char** argv) {
  ros::init(argc, argv, "set_load_client");
  ros::NodeHandle nh("~");

  double mass;
  std::vector<double> center_load(3);
  std::vector<double> load_inertia(9);

  nh.getParam("mass", mass);
  nh.getParam("F_x_center_load", center_load);
  if (!nh.getParam("F_x_center_load", center_load)) {
    ROS_ERROR("Failed to get param 'F_x_center_load'");
  }
  nh.getParam("load_inertia", load_inertia);

  // Print the acquired parameters
  std::cout << "Mass: " << mass << std::endl;
  std::cout << "Center Load: ";
  for (const auto& val : center_load) {
    std::cout << val << " ";
  }
  std::cout << std::endl;
  std::cout << "Load Inertia: ";
  for (const auto& val : load_inertia) {
    std::cout << val << " ";
  }
  std::cout << std::endl;

  ros::ServiceClient client = nh.serviceClient<franka_msgs::SetLoad>("/franka_control/set_load");

  // Wait for the service to be available
  ROS_INFO("Waiting for service /franka_control/set_load to be available...");
  client.waitForExistence();
  ROS_INFO("Service /franka_control/set_load is now available.");

  franka_msgs::SetLoad srv;
  srv.request.mass = mass;
  for (int i = 0; i < 3; ++i) {
    srv.request.F_x_center_load[i] = center_load[i];
  }
  for (int i = 0; i < 9; ++i) {
    srv.request.load_inertia[i] = load_inertia[i];
  }
  if (client.call(srv)) {
    if (srv.response.success) {
      ROS_INFO("Load set successfully");
    } else {
      ROS_ERROR("Failed to set load: %s", srv.response.error.c_str());
    }
  } else {
    ROS_ERROR("Failed to call service set_load");
    return 1;
  }

  return 0;
}