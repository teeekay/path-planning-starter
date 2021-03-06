//
// Trajectory.h
// Created by Anthony M Knight 30/01/2018
//
#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include "PathPlanner.h"
#include "RoadMap.h"
#include "JMT.h"
#include "PathTracking.h"
#include "spdlog/spdlog.h"

#include "spline.h"

#define G_FORCE_MPS 9.8

const double JMTMaxSpeedMpH = 49.0;// 48.0;//
const double JMTMaxSpeedinLaneChangeMpH = 47.0;//

struct Acc_Jerk
{
	double Max_Vel;
	double Max_Total_Acc;
	double Max_Pos_Acc; //Acceleration
	double Max_Neg_Acc; //braking
	double Max_Fwd_Acc;
	double Max_Lat_Acc;
	double Max_Total_Jerk;
	double Max_Fwd_Jerk;
	double Max_Lat_Jerk;
};


class Trajectory
{
public:
	explicit Trajectory(const HighwayMap &map): map2(map), 
		MaxSpeedMpS(MphToMetersPerSecond(JMTMaxSpeedMpH)), 
		MaxSpeedInLaneChangeMpS(MphToMetersPerSecond(JMTMaxSpeedinLaneChangeMpH))
	{ 
		_TL = spdlog::get("Traj"); 
		_TL->info("Trajectory startup.");
	}
	
	std::vector<CartesianPoint> InitiateTrajectory(PathPlannerInput input);
	std::vector<CartesianPoint> GenerateJMTLaneChangeTrajectory(PathPlannerInput input, int TargetLane, double DesiredVelocity, bool truncate=false);
	std::vector<CartesianPoint> GenerateKeepInLaneTrajectory(PathPlannerInput input, double DesiredVelocity , bool truncate=false,
		double FinalFPtDOffset=0.0);
	std::vector<CartesianPoint> RecalcTrajectory(PathPlannerInput input, double DOffset);
	double GetAcceleration();
	FrenetPoint GetFrenetLocation(int pathsize);
	void LogFPath(std::vector<FrenetPoint> FPath);
	void LogCPath(std::vector<CartesianPoint> CPath);
	const double MaxSpeedInLaneChangeMpS;
	const double MaxSpeedMpS;

private:
	std::vector<FrenetDescriptors> GenerateJMTDPath(FrenetPoint LastFPt, FrenetPoint DestFPt, FrenetPoint LastSpeed, FrenetPoint TargetSpeed, double T,
		                             bool GenerateZero = false, FrenetPoint LastAccel={ 0.0,0.0 }, FrenetPoint TargetAccel={ 0.0,0.0 });

	Acc_Jerk CheckPath(std::vector<CartesianPoint> Path, double time_increment, double v_init = 0.0, double a_init = 0.0);
	
	inline double MphToMetersPerSecond(double mphValue) { return mphValue * (1609.34 / 3600.0); }

	double GetSafeAcceleration(double accel);
	
	const double MaxFwdAccelerationMpSsq = 0.225 * G_FORCE_MPS; /* 0.25 times force of gravity */
	const double MaxBrakingAccelerationMpSsq = -0.225 * G_FORCE_MPS; /* -0.25 times force of gravity */
	const double SimulatorRunloopPeriod = 0.02;

	const HighwayMap& map2;
	PathTracking BuiltPath;

	double Acceleration;
	double MaxVelocityReported;
	
    //spdlog pointer
	std::shared_ptr<spdlog::logger> _TL;

};


#endif //TRAJECTORY_H