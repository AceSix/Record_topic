#include <iostream>
#include <ros/ros.h>
#include <sensor_msgs/NavSatFix.h>
#include <sensor_msgs/Imu.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/Vector3Stamped.h>
#include <nav_msgs/Odometry.h>

#include <string>
#include <vector>
#include <fstream>
#include <sstream>


using namespace std;


double t_gps;
double latitude;
double longitude;
double altitude;
double gps_covariance;

double t_imu;
geometry_msgs::Vector3 ang_imu; 
geometry_msgs::Vector3 lin_imu; 

double t_odom;
double vel_x;



ros::Subscriber imu_bias_sub;
bool imu_bias_initialized = false;



ros::Publisher pose_pub;

void gpsCallback(sensor_msgs::NavSatFix msg)                 //GPSdata
{
	t_gps = msg.header.stamp.toSec();
	latitude = msg.latitude;
        longitude = msg.longitude;
	altitude = msg.altitude;
	gps_covariance = msg.position_covariance[0];
}


void imuCallback(sensor_msgs::Imu msg)			//imudata
{
	t_imu = msg.header.stamp.toSec();
	ang_imu = msg.angular_velocity; 
	lin_imu = msg.linear_acceleration;
}

void OdomCallback(nav_msgs::Odometry msg)		//Odometrydata
{
	t_odom = msg.header.stamp.toSec();
	vel_x = msg.twist.twist.linear.x;
}


int main(int argc, char **argv)
{
	ros::init(argc, argv, "JDD_node");
	ros::NodeHandle n;
	ros::Subscriber imu_sub = n.subscribe("/compensated_imu", 1000, imuCallback);
	ros::Subscriber gps_sub = n.subscribe("/fix", 1000, gpsCallback);
	ros::Subscriber odom_sub = n.subscribe("/odom", 1000, OdomCallback);

	ofstream outFile;
	outFile.setf(ios::fixed, ios::floatfield);  // 设定为 fixed 模式，以小数点表示浮点数
	outFile.precision(6); 
	outFile.open("/media/aceliu/AceLIU、/preliminary.csv", ios::out); // 打开模式可省略 test1 or test2	
	ros::Rate loop_rate(20);
	double tpre = 0;
	while(ros::ok())
	{
		ros::spinOnce();
		if(t_odom!=0 && t_odom!= tpre)
		{
			tpre = t_odom;
			outFile << t_gps <<','<< latitude <<','<< longitude <<','<< altitude <<','<< gps_covariance <<','
			<< t_imu <<','<< ang_imu.x <<','<< ang_imu.y <<','<< ang_imu.z <<','<< lin_imu.x <<','<< lin_imu.y <<','<< lin_imu.z <<','
			<< t_odom <<','<< vel_x
			<< endl;
		}


		loop_rate.sleep();
	}
	
	return 0;
}
