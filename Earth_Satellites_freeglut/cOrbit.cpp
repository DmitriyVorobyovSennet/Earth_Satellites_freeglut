#include "cOrbit.h"

cOrbit::cOrbit()
{
}

cOrbit::cOrbit(double EpochYear, double EpochDay, double OrbitNum,
			   double Incl, double RAAN, double Eccentr, 
			   double ArgPer, double MeanAnom0, double N)
{
	this->EpochYear = EpochYear;
	this->EpochDay = EpochDay;
	this->OrbitNum = OrbitNum;
	this->Incl = degreeToRad(Incl);
	this->RAAN = degreeToRad(RAAN);
	this->Eccentr = Eccentr;
	this->ArgPer = degreeToRad(ArgPer);
	this->MeanAnom0 = degreeToRad(MeanAnom0);
	this->MeanMotion = 2*M_PI*N;

	double temp = pow(N, 2.0/3);
	this->SMA = 6.6228f/temp;

	TimePeriod = 1/N; // N - revolutions per day
}

void cOrbit::orbitInit(double EpochYear, double EpochDay, double OrbitNum,
					   double Incl, double RAAN, double Eccentr, 
					   double ArgPer, double MeanAnom0, double N)
{
	this->EpochYear = EpochYear;
	this->EpochDay = EpochDay;
	this->OrbitNum = OrbitNum;
	this->Incl = degreeToRad(Incl);
	this->RAAN = degreeToRad(RAAN);
	this->Eccentr = Eccentr;
	this->ArgPer = degreeToRad(ArgPer);
	this->MeanAnom0 = degreeToRad(MeanAnom0);
	this->MeanMotion = 2*M_PI*N;

	this->SMA = 6.6228f/pow(N, 2.0/3);

	TimePeriod = 1/N; // N - revolutions per day
}

cOrbit::~cOrbit(void)
{
}

double cOrbit::degreeToRad(double degree)
{
	return degree/180*M_PI;
}

coord cOrbit::calculateCoord(double time)
{
	double MeanAnom = MeanAnom0 + MeanMotion*(time - EpochDay);
	MeanAnom = fmod(MeanAnom, 2*M_PI);

	double dt = 1;
	double E0 = MeanAnom, E = 0; // E - Eccentric Anomaly
	while(abs(dt) > 1e-5) // may crash
	{
		E = Eccentr*sin(E0) + MeanAnom;
		dt = E - E0;
		E0 = E;
	}

	double temp = atan(sqrt((1-Eccentr)/(1+Eccentr))*tan(E/2));
	while(temp < 0) temp += 2*M_PI;
	double theta = 2*temp;
	theta = fmod(theta, 2*M_PI);

	double absR = (SMA * (1 - pow(Eccentr,2))) / (1 + Eccentr*cos(theta));
	double u = ArgPer + theta;
	u = fmod(u, 2*M_PI);

	coord satCoord;
	satCoord.x = (float) absR * (cos(u)*cos(RAAN) - cos(Incl)*sin(u)*sin(RAAN));
	satCoord.y = (float) absR * (cos(u)*sin(RAAN) + cos(Incl)*sin(u)*cos(RAAN));
	satCoord.z = (float) sin(Incl)*sin(u);

	return satCoord;
}
