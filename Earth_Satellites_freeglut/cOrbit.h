#pragma once
#define _USE_MATH_DEFINES
#include <cmath>

struct coord
{
	float x;
	float y;
	float z;
};

class cOrbit
{
private:
	double SMA; // semi-major axis (earth radii)
	double EpochYear; // Epoch: Last two digits of year
	double EpochDay; // Epoch: Fractional Julian Day of year
	double OrbitNum; // Orbit at epoch
	double Incl; // Inclination (degree)
	double RAAN; // Right Ascension of the Ascending Node (degree)
	double Eccentr; // Eccentricity
	double ArgPer; // Argument of perigee (degree)
	double MeanAnom0; // Mean anomaly (degree)
	double MeanMotion; // Mean motion (2*PI*rev/day)
	double TimePeriod;
public:
	cOrbit();
	cOrbit(double EpochYear, double EpochDay, double OrbitNum,
		double Incl, double RAAN, double Eccentr, 
		double ArgPer, double MeanAnom0, double N);
	~cOrbit(void);

	void orbitInit(double EpochYear, double EpochDay, double OrbitNum,
		double Incl, double RAAN, double Eccentr, 
		double ArgPer, double MeanAnom0, double N);

	double getEpochDay() {return EpochDay;};
	double getTimePeriod() {return TimePeriod;};

	double degreeToRad(double degree);
	coord calculateCoord(double time);
};