/**
 * SunriseSunsetCalc.cpp
 *
 * Copyright 2016-2021 Heartland Software Solutions Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the license at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the LIcense is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "times_internal.h"

#ifdef HAVE_HSS_MATH
#include "mathhss.h"
#elif defined(INTEL_COMPILER)
#include <mathimf.h>
#else
#include <cmath>

#ifdef _MSC_VER
void sincos(double val, double* p_sin, double *p_cos)
{
	*p_sin = ::sin(val);
	*p_cos = ::cos(val);
}
#endif
#endif

#include "SunriseSunsetCalc.h"


using namespace HSS_Time_Private;


CSunriseSunsetCalc::CSunriseSunsetCalc()
{
}

///<summary>
///isLeapYear returns 1 if the 4-digit yr is a leap year, 0 if it is not
///</summary>
bool CSunriseSunsetCalc::isLeapYear(INTNM::int32_t yr) 
{
	return ((yr % 4 == 0 && yr % 100 != 0) || yr % 400 == 0);
}

// Convert radian angle to degrees
double CSunriseSunsetCalc::radToDeg(double angleRad) 
{
//	return (angleRad * 180.0) / Pi;
	return (angleRad * 180.0) * 0.318309886183790671537768;
}

// Convert degree angle to radians
double CSunriseSunsetCalc::degToRad(double angleDeg) 
{
//	return (angleDeg / 180.0) * Pi;
	return (angleDeg / 180.0) * 3.14159265358979323846264;
}


///<summary>
/// Finds numerical day-of-year from mn, day and lp year info
///</summary>
/// Arguments:
///   month: January = 1
///   day  : 1 - 31
///   lpyr : 1 if leap year, 0 if not
/// Return value:
///   The numerical day of year
INTNM::int32_t CSunriseSunsetCalc::calcDayOfYear(INTNM::int32_t mn, INTNM::int32_t dy, bool lpyr) 
{
	double k = (lpyr ? 1 : 2);
	double doy = floor((275.0 * mn)/9.0) - k * floor((mn + 9.0)/12.0) + dy -30.0;
	return (INTNM::int32_t)doy;
}

//***********************************************************************/
//* Name:    calcDayOfWeek								*/
//* Type:    Function									*/
//* Purpose: Derives weekday from Julian Day					*/
//* Arguments:										*/
//*   juld : Julian Day									*/
//* Return value:										*/
//*   String containing name of weekday						*/
//***********************************************************************/

const std::string CSunriseSunsetCalc::calcDayOfWeek(double juld, INTNM::int32_t *day)
{
	INTNM::int32_t A = ((INTNM::int32_t)(juld + 1.5)) % 7;
	const std::string DOW = (A==0)?"Sunday":(A==1)?"Monday":(A==2)?"Tuesday":(A==3)?"Wednesday":(A==4)?"Thursday":(A==5)?"Friday":"Saturday";
	*day=A;
	return DOW;
}

//***********************************************************************/
//* Name:    calcJD									*/
//* Type:    Function									*/
//* Purpose: Julian day from calendar day						*/
//* Arguments:										*/
//*   year : 4 digit year								*/
//*   month: January = 1								*/
//*   day  : 1 - 31									*/
//* Return value:										*/
//*   The Julian day corresponding to the date					*/
//* Note:											*/
//*   Number is returned for start of day.  Fractional days should be	*/
//*   added later.									*/
//***********************************************************************/

double CSunriseSunsetCalc::calcJD(INTNM::int32_t year, INTNM::int32_t month, INTNM::int32_t day)
{
	if (month <= 2) 
	{
		year -= 1;
		month += 12;
	}
	double A = floor(year/100.0);
	double B = 2 - A + floor(A/4.0);

	double JD = floor(365.25*(year + 4716)) + floor(30.6001*(month+1)) + day + B - 1524.5;
	return JD;
}

//***********************************************************************/
//* Name:    calcDayFromJD								*/
//* Type:    Function									*/
//* Purpose: Calendar day (minus year) from Julian Day			*/
//* Arguments:										*/
//*   jd   : Julian Day									*/
//* Return value:										*/
//*   String date in the form DD-MONTH						*/
//***********************************************************************/

void CSunriseSunsetCalc::calcDayFromJD(double jd, INTNM::int32_t *yearOut, INTNM::int32_t *monthOut, INTNM::int32_t *dayOut)
{
	double z = floor(jd + 0.5);
	double f = (jd + 0.5) - z;

	double A,B,C,D,E;

	double alpha;

	if (z < 2299161) 
	{
		A = z;
	} 
	else 
	{
		alpha = floor((z - 1867216.25)/36524.25);
		A = z + 1 + alpha - floor(alpha/4.0);
	}

	B = A + 1524;
	C = floor((B - 122.1)/365.25);
	D = floor(365.25 * C);
	E = floor((B - D)/30.6001);

	INTNM::int32_t day = (INTNM::int32_t)(B - D - floor(30.6001 * E) + f);
	INTNM::int32_t month =(INTNM::int32_t)( (E < 14) ? E - 1 : E - 13 );
	INTNM::int32_t year = (INTNM::int32_t)( (month > 2) ? C - 4716 : C - 4715);

	*yearOut=year;
	*monthOut=month;
	*dayOut=day;
}

//***********************************************************************/
//* Name:    calcTimeJulianCent							*/
//* Type:    Function									*/
//* Purpose: convert Julian Day to centuries since J2000.0.			*/
//* Arguments:										*/
//*   jd : the Julian Day to convert						*/
//* Return value:										*/
//*   the T value corresponding to the Julian Day				*/
//***********************************************************************/

double CSunriseSunsetCalc::calcTimeJulianCent(double jd)
{
	double T = (jd - 2451545.0)/36525.0;
	return T;
}

//***********************************************************************/
//* Name:    calcJDFromJulianCent							*/
//* Type:    Function									*/
//* Purpose: convert centuries since J2000.0 to Julian Day.			*/
//* Arguments:										*/
//*   t : number of Julian centuries since J2000.0				*/
//* Return value:										*/
//*   the Julian Day corresponding to the t value				*/
//***********************************************************************/

double CSunriseSunsetCalc::calcJDFromJulianCent(double t)
{
	double JD = t * 36525.0 + 2451545.0;
	return JD;
}

//***********************************************************************/
//* Name:    calGeomMeanLongSun							*/
//* Type:    Function									*/
//* Purpose: calculate the Geometric Mean Longitude of the Sun		*/
//* Arguments:										*/
//*   t : number of Julian centuries since J2000.0				*/
//* Return value:										*/
//*   the Geometric Mean Longitude of the Sun in degrees			*/
//***********************************************************************/

double CSunriseSunsetCalc::calcGeomMeanLongSun(double t)
{
	double L0 = 280.46646 + t * (36000.76983 + 0.0003032 * t);
	while(L0 > 360.0)
	{
		L0 -= 360.0;
	}
	while(L0 < 0.0)
	{
		L0 += 360.0;
	}
	return L0;		// in degrees
}

//***********************************************************************/
//* Name:    calGeomAnomalySun							*/
//* Type:    Function									*/
//* Purpose: calculate the Geometric Mean Anomaly of the Sun		*/
//* Arguments:										*/
//*   t : number of Julian centuries since J2000.0				*/
//* Return value:										*/
//*   the Geometric Mean Anomaly of the Sun in degrees			*/
//***********************************************************************/

double CSunriseSunsetCalc::calcGeomMeanAnomalySun(double t)
{
	double M = 357.52911 + t * (35999.05029 - 0.0001537 * t);
	return M;		// in degrees
}

//***********************************************************************/
//* Name:    calcEccentricityEarthOrbit						*/
//* Type:    Function									*/
//* Purpose: calculate the eccentricity of earth's orbit			*/
//* Arguments:										*/
//*   t : number of Julian centuries since J2000.0				*/
//* Return value:										*/
//*   the unitless eccentricity							*/
//***********************************************************************/

double CSunriseSunsetCalc::calcEccentricityEarthOrbit(double t)
{
	double e = 0.016708634 - t * (0.000042037 + 0.0000001267 * t);
	return e;		// unitless
}

//***********************************************************************/
//* Name:    calcSunEqOfCenter							*/
//* Type:    Function									*/
//* Purpose: calculate the equation of center for the sun			*/
//* Arguments:										*/
//*   t : number of Julian centuries since J2000.0				*/
//* Return value:										*/
//*   in degrees										*/
//***********************************************************************/

double CSunriseSunsetCalc::calcSunEqOfCenter(double t)
{
	double m = calcGeomMeanAnomalySun(t);

	double mrad = degToRad(m);
	double sinm = sin(mrad);
	double sin2m = sin(mrad+mrad);
	double sin3m = sin(mrad+mrad+mrad);

	double C = sinm * (1.914602 - t * (0.004817 + 0.000014 * t)) + sin2m * (0.019993 - 0.000101 * t) + sin3m * 0.000289;
	return C;		// in degrees
}

//***********************************************************************/
//* Name:    calcSunTrueLong								*/
//* Type:    Function									*/
//* Purpose: calculate the true longitude of the sun				*/
//* Arguments:										*/
//*   t : number of Julian centuries since J2000.0				*/
//* Return value:										*/
//*   sun's true longitude in degrees						*/
//***********************************************************************/

double CSunriseSunsetCalc::calcSunTrueLong(double t)
{
	double l0 = calcGeomMeanLongSun(t);
	double c = calcSunEqOfCenter(t);

	double O = l0 + c;
	return O;		// in degrees
}

//***********************************************************************/
//* Name:    calcSunTrueAnomaly							*/
//* Type:    Function									*/
//* Purpose: calculate the true anamoly of the sun				*/
//* Arguments:										*/
//*   t : number of Julian centuries since J2000.0				*/
//* Return value:										*/
//*   sun's true anamoly in degrees							*/
//***********************************************************************/

double CSunriseSunsetCalc::calcSunTrueAnomaly(double t)
{
	double m = calcGeomMeanAnomalySun(t);
	double c = calcSunEqOfCenter(t);

	double v = m + c;
	return v;		// in degrees
}

//***********************************************************************/
//* Name:    calcSunRadVector								*/
//* Type:    Function									*/
//* Purpose: calculate the distance to the sun in AU				*/
//* Arguments:										*/
//*   t : number of Julian centuries since J2000.0				*/
//* Return value:										*/
//*   sun radius vector in AUs							*/
//***********************************************************************/

double CSunriseSunsetCalc::calcSunRadVector(double t)
{
	double v = calcSunTrueAnomaly(t);
	double e = calcEccentricityEarthOrbit(t);

	double R = (1.000001018 * (1 - e * e)) / (1 + e * cos(degToRad(v)));
	return R;		// in AUs
}

//***********************************************************************/
//* Name:    calcSunApparentLong							*/
//* Type:    Function									*/
//* Purpose: calculate the apparent longitude of the sun			*/
//* Arguments:										*/
//*   t : number of Julian centuries since J2000.0				*/
//* Return value:										*/
//*   sun's apparent longitude in degrees						*/
//***********************************************************************/

double CSunriseSunsetCalc::calcSunApparentLong(double t)
{
	double o = calcSunTrueLong(t);

	double omega = 125.04 - 1934.136 * t;
	double lambda = o - 0.00569 - 0.00478 * sin(degToRad(omega));
	return lambda;		// in degrees
}

//***********************************************************************/
//* Name:    calcMeanObliquityOfEcliptic						*/
//* Type:    Function									*/
//* Purpose: calculate the mean obliquity of the ecliptic			*/
//* Arguments:										*/
//*   t : number of Julian centuries since J2000.0				*/
//* Return value:										*/
//*   mean obliquity in degrees							*/
//***********************************************************************/

double CSunriseSunsetCalc::calcMeanObliquityOfEcliptic(double t)
{
	double seconds = 21.448 - t*(46.8150 + t*(0.00059 - t*(0.001813)));
	double e0 = 23.0 + (26.0 + (seconds/60.0))/60.0;
	return e0;		// in degrees
}

//***********************************************************************/
//* Name:    calcObliquityCorrection						*/
//* Type:    Function									*/
//* Purpose: calculate the corrected obliquity of the ecliptic		*/
//* Arguments:										*/
//*   t : number of Julian centuries since J2000.0				*/
//* Return value:										*/
//*   corrected obliquity in degrees						*/
//***********************************************************************/

double CSunriseSunsetCalc::calcObliquityCorrection(double t)
{
	double e0 = calcMeanObliquityOfEcliptic(t);

	double omega = 125.04 - 1934.136 * t;
	double e = e0 + 0.00256 * cos(degToRad(omega));
	return e;		// in degrees
}

//***********************************************************************/
//* Name:    calcSunRtAscension							*/
//* Type:    Function									*/
//* Purpose: calculate the right ascension of the sun				*/
//* Arguments:										*/
//*   t : number of Julian centuries since J2000.0				*/
//* Return value:										*/
//*   sun's right ascension in degrees						*/
//***********************************************************************/

double CSunriseSunsetCalc::calcSunRtAscension(double t)
{
	double e = calcObliquityCorrection(t);
	double lambda = calcSunApparentLong(t);

	double rad_lambda = degToRad(lambda);
	double sin_lambda, cos_lambda;
	::sincos(rad_lambda, &sin_lambda, &cos_lambda);
	double tananum = (cos(degToRad(e)) * sin_lambda);
	double tanadenom = cos_lambda;
	double alpha = radToDeg(atan2(tananum, tanadenom));
	return alpha;		// in degrees
}

//***********************************************************************/
//* Name:    calcSunDeclination							*/
//* Type:    Function									*/
//* Purpose: calculate the declination of the sun				*/
//* Arguments:										*/
//*   t : number of Julian centuries since J2000.0				*/
//* Return value:										*/
//*   sun's declination in degrees							*/
//***********************************************************************/

double CSunriseSunsetCalc::calcSunDeclination(double t)
{
	double e = calcObliquityCorrection(t);
	double lambda = calcSunApparentLong(t);

	double sint = sin(degToRad(e)) * sin(degToRad(lambda));
	double theta = radToDeg(asin(sint));
	return theta;		// in degrees
}

//***********************************************************************/
//* Name:    calcEquationOfTime							*/
//* Type:    Function									*/
//* Purpose: calculate the difference between true solar time and mean	*/
//*		solar time									*/
//* Arguments:										*/
//*   t : number of Julian centuries since J2000.0				*/
//* Return value:										*/
//*   equation of time in minutes of time						*/
//***********************************************************************/

double CSunriseSunsetCalc::calcEquationOfTime(double t)
{
	double epsilon = calcObliquityCorrection(t);
	double l0 = calcGeomMeanLongSun(t);
	double e = calcEccentricityEarthOrbit(t);
	double m = calcGeomMeanAnomalySun(t);

	if(epsilon==180)
		return -9999;
	double y = tan(degToRad(epsilon)/2.0);
	y *= y;

	double sin2l0, cos2l0;
	::sincos(2.0 * degToRad(l0), &sin2l0, &cos2l0);
	double sinm   = sin(degToRad(m));
	double sin4l0 = sin(4.0 * degToRad(l0));
	double sin2m  = sin(2.0 * degToRad(m));

	double Etime = y * sin2l0 - 2.0 * e * sinm + 4.0 * e * y * sinm * cos2l0
			- 0.5 * y * y * sin4l0 - 1.25 * e * e * sin2m;

	return radToDeg(Etime)*4.0;	// in minutes of time
}

//***********************************************************************/
//* Name:    calcHourAngleSunrise							*/
//* Type:    Function									*/
//* Purpose: calculate the hour angle of the sun at sunrise for the	*/
//*			latitude								*/
//* Arguments:										*/
//*   lat : latitude of observer in degrees					*/
//*	solarDec : declination angle of sun in degrees				*/
//* Return value:										*/
//*   hour angle of sunrise in radians						*/
//***********************************************************************/

double CSunriseSunsetCalc::calcHourAngleSunrise(double lat, double solarDec)
{
	double latRad = degToRad(lat);
	double sdRad  = degToRad(solarDec);

	double HAarg = (cos(degToRad(90.833))/(cos(latRad)*cos(sdRad))-tan(latRad) * tan(sdRad));

	if(fabs(HAarg)>1)
		return -9999;
	double divider;
	divider=(cos(latRad)*cos(sdRad));
	if(fabs(divider)<0.0000001)
		return -9999;
	divider=cos(degToRad(90.833))/(cos(latRad)*cos(sdRad))-tan(latRad) * tan(sdRad);
	if(fabs(divider)>1)
		return -9999;
	double HA = (acos(divider));

	return HA;		// in radians
}

//***********************************************************************/
//* Name:    calcHourAngleSunset							*/
//* Type:    Function									*/
//* Purpose: calculate the hour angle of the sun at sunset for the	*/
//*			latitude								*/
//* Arguments:										*/
//*   lat : latitude of observer in degrees					*/
//*	solarDec : declination angle of sun in degrees				*/
//* Return value:										*/
//*   hour angle of sunset in radians						*/
//***********************************************************************/

double CSunriseSunsetCalc::calcHourAngleSunset(double lat, double solarDec)
{
	double latRad = degToRad(lat);
	double sdRad  = degToRad(solarDec);

	double HAarg = (cos(degToRad(90.833))/(cos(latRad)*cos(sdRad))-tan(latRad) * tan(sdRad));

	if(fabs(HAarg)>1)
		return -9999;
	double divider=cos(degToRad(90.833))/(cos(latRad)*cos(sdRad))-tan(latRad) * tan(sdRad);
	if(fabs(divider)>1)
		return -9999;
	double HA = (acos(divider));

	return -HA;		// in radians
}

//***********************************************************************/
//* Name:    calcSunriseUTC								*/
//* Type:    Function									*/
//* Purpose: calculate the Universal Coordinated Time (UTC) of sunrise	*/
//*			for the given day at the given location on earth	*/
//* Arguments:										*/
//*   JD  : julian day									*/
//*   latitude : latitude of observer in degrees				*/
//*   longitude : longitude of observer in degrees				*/
//* Return value:										*/
//*   time in minutes from zero Z							*/
//***********************************************************************/

double CSunriseSunsetCalc::calcSunriseUTC(double JD, double latitude, double longitude)
{
	double t = calcTimeJulianCent(JD);

	// *** First pass to approximate sunrise

	double eqTime = calcEquationOfTime(t);
	if(eqTime==-9999)
		return -9999;
	double solarDec = calcSunDeclination(t);
	double hourAngle = calcHourAngleSunrise(latitude, solarDec);
	if(hourAngle==-9999)
		return -9999;

	double delta = longitude - radToDeg(hourAngle);
	double timeDiff = 4 * delta;	// in minutes of time
	double timeUTC = 720 + timeDiff - eqTime;	// in minutes

	// *** Second pass includes fractional jday in gamma calc

	double newt = calcTimeJulianCent(calcJDFromJulianCent(t) + timeUTC/1440.0); 
	eqTime = calcEquationOfTime(newt);
	if(eqTime==-9999)
		return -9999;
	solarDec = calcSunDeclination(newt);
	hourAngle = calcHourAngleSunrise(latitude, solarDec);
	if(hourAngle==-9999)
		return -9999;
	delta = longitude - radToDeg(hourAngle);
	timeDiff = 4 * delta;
	timeUTC = 720 + timeDiff - eqTime; // in minutes

	return timeUTC;
}

//***********************************************************************/
//* Name:    calcSolNoonUTC								*/
//* Type:    Function									*/
//* Purpose: calculate the Universal Coordinated Time (UTC) of solar	*/
//*		noon for the given day at the given location on earth		*/
//* Arguments:										*/
//*   t : number of Julian centuries since J2000.0				*/
//*   longitude : longitude of observer in degrees				*/
//* Return value:										*/
//*   time in minutes from zero Z							*/
//***********************************************************************/

double CSunriseSunsetCalc::calcSolNoonUTC(double t, double longitude)
{
	/*double newt =*/ calcTimeJulianCent(calcJDFromJulianCent(t) + 0.5 + longitude/360.0); 

	double eqTime = calcEquationOfTime(t);
	calcSunDeclination(t);
	double solNoonUTC = 720 + (longitude * 4) - eqTime; // min
	
	return solNoonUTC;
}

//***********************************************************************/
//* Name:    calcSunsetUTC								*/
//* Type:    Function									*/
//* Purpose: calculate the Universal Coordinated Time (UTC) of sunset	*/
//*			for the given day at the given location on earth	*/
//* Arguments:										*/
//*   JD  : julian day									*/
//*   latitude : latitude of observer in degrees				*/
//*   longitude : longitude of observer in degrees				*/
//* Return value:										*/
//*   time in minutes from zero Z							*/
//***********************************************************************/

double CSunriseSunsetCalc::calcSunsetUTC(double JD, double latitude, double longitude)
{
	double t = calcTimeJulianCent(JD);

	// First calculates sunrise and approx length of day

	double eqTime = calcEquationOfTime(t);
	if(eqTime==-9999)
		return -9999;
	double solarDec = calcSunDeclination(t);
	double hourAngle = calcHourAngleSunset(latitude, solarDec);

	if(hourAngle==-9999)
		return -9999;
	double delta = longitude - radToDeg(hourAngle);
	double timeDiff = 4 * delta;
	double timeUTC = 720 + timeDiff - eqTime;

	// first pass used to include fractional day in gamma calc

	double newt = calcTimeJulianCent(calcJDFromJulianCent(t) + timeUTC/1440.0); 
	eqTime = calcEquationOfTime(newt);
	if(eqTime==-9999)
		return -9999;
	solarDec = calcSunDeclination(newt);
	hourAngle = calcHourAngleSunset(latitude, solarDec);
	if(hourAngle==-9999)
		return -9999;

	delta = longitude - radToDeg(hourAngle);
	timeDiff = 4 * delta;
	timeUTC = 720 + timeDiff - eqTime; // in minutes

	return timeUTC;
}


//***********************************************************************/
//* Name:    findRecentSunrise							*/
//* Type:    Function									*/
//* Purpose: calculate the julian day of the most recent sunrise		*/
//*		starting from the given day at the given location on earth	*/
//* Arguments:										*/
//*   JD  : julian day									*/
//*   latitude : latitude of observer in degrees				*/
//*   longitude : longitude of observer in degrees				*/
//* Return value:										*/
//*   julian day of the most recent sunrise					*/
//***********************************************************************/

double CSunriseSunsetCalc::findRecentSunrise(double jd, double latitude, double longitude)
{
	double julianday = jd;

	double time = calcSunriseUTC(julianday, latitude, longitude);
	while(time==-9999)//!isNumber(time))
	{
		julianday -= 1.0;
		time = calcSunriseUTC(julianday, latitude, longitude);
	}

	return julianday;
}

//***********************************************************************/
//* Name:    findRecentSunset								*/
//* Type:    Function									*/
//* Purpose: calculate the julian day of the most recent sunset		*/
//*		starting from the given day at the given location on earth	*/
//* Arguments:										*/
//*   JD  : julian day									*/
//*   latitude : latitude of observer in degrees				*/
//*   longitude : longitude of observer in degrees				*/
//* Return value:										*/
//*   julian day of the most recent sunset					*/
//***********************************************************************/

double CSunriseSunsetCalc::findRecentSunset(double jd, double latitude, double longitude)
{
	double julianday = jd;

	double time = calcSunsetUTC(julianday, latitude, longitude);
	while(time==-9999)//!isNumber(time))
	{
		julianday -= 1.0;
		time = calcSunsetUTC(julianday, latitude, longitude);
	}

	return julianday;
}

//***********************************************************************/
//* Name:    findNextSunrise								*/
//* Type:    Function									*/
//* Purpose: calculate the julian day of the next sunrise			*/
//*		starting from the given day at the given location on earth	*/
//* Arguments:										*/
//*   JD  : julian day									*/
//*   latitude : latitude of observer in degrees				*/
//*   longitude : longitude of observer in degrees				*/
//* Return value:										*/
//*   julian day of the next sunrise						*/
//***********************************************************************/

double CSunriseSunsetCalc::findNextSunrise(double jd, double latitude, double longitude)
{
	double julianday = jd;

	double time = calcSunriseUTC(julianday, latitude, longitude);
	while(time==-9999)//!isNumber(time))
	{
		julianday += 1.0;
		time = calcSunriseUTC(julianday, latitude, longitude);
	}

	return julianday;
}

//***********************************************************************/
//* Name:    findNextSunset								*/
//* Type:    Function									*/
//* Purpose: calculate the julian day of the next sunset			*/
//*		starting from the given day at the given location on earth	*/
//* Arguments:										*/
//*   JD  : julian day									*/
//*   latitude : latitude of observer in degrees				*/
//*   longitude : longitude of observer in degrees				*/
//* Return value:										*/
//*   julian day of the next sunset							*/
//***********************************************************************/

double CSunriseSunsetCalc::findNextSunset(double jd, double latitude, double longitude)
{
	double julianday = jd;

	double time = calcSunsetUTC(julianday, latitude, longitude);
	while(time==-9999)//!isNumber(time))
	{
		julianday += 1.0;
		time = calcSunsetUTC(julianday, latitude, longitude);
	}

	return julianday;
}

//***********************************************************************/
//* Name:    timeString									*/
//* Type:    Function									*/
//* Purpose: convert time of day in minutes to a zero-padded string	*/
//*		suitable for printing to the form text fields			*/
//* Arguments:										*/
//*   minutes : time of day in minutes						*/
//* Return value:										*/
//*   string of the format HH:MM:SS, minutes and seconds are zero padded*/
//***********************************************************************/

void CSunriseSunsetCalc::timeString(double minutes, INTNM::int32_t *hourOut, INTNM::int32_t *minOut, INTNM::int32_t *secOut)
// timeString returns a zero-padded string (HH:MM:SS) given time in minutes
{
	double floatHour = minutes / 60;
	double hour = floor(floatHour);
	double floatMinute = 60 * (floatHour - floor(floatHour));
	double minute = floor(floatMinute);
	double floatSec = 60 * (floatMinute - floor(floatMinute));
	double second = floor(floatSec);
	
	*hourOut=(INTNM::int32_t)hour;
	*minOut=(INTNM::int32_t)minute;
	*secOut=(INTNM::int32_t)second;

//	return timeStr;
}

//***********************************************************************/
//* Name:    timeStringShortAMPM							*/
//* Type:    Function									*/
//* Purpose: convert time of day in minutes to a zero-padded string	*/
//*		suitable for printing to the form text fields.  If time	*/
//*		crosses a day boundary, date is appended.				*/
//* Arguments:										*/
//*   minutes : time of day in minutes						*/
//*   JD  : julian day									*/
//* Return value:										*/
//*   string of the format HH:MM[AM/PM] (DDMon)					*/
//***********************************************************************/

// timeStringShortAMPM returns a zero-padded string (HH:MM *M) given time in 
// minutes and appends short date if time is > 24 or < 0, resp.

void CSunriseSunsetCalc::timeStringShortAMPM(double minutes, double JD, INTNM::int32_t *yearOut, INTNM::int32_t *monthOut, INTNM::int32_t *dayOut, INTNM::int32_t *hourOut, INTNM::int32_t *minOut, INTNM::int32_t *secOut)
{
	double julianday = JD;
	double floatHour = minutes / 60;
	double hour = floor(floatHour);
	double floatMinute = 60 * (floatHour - floor(floatHour));
	double minute = floor(floatMinute);
	double floatSec = 60 * (floatMinute - floor(floatMinute));
	double second = floor(floatSec);
	bool PM = false;

	minute += (second >= 30)? 1 : 0;

	if (minute >= 60) 
	{
		minute -= 60;
		hour ++;
	}

	bool daychange = false;
	if (hour > 23) 
	{
		hour -= 24;
		daychange = true;
		julianday += 1.0;
	}

	if (hour < 0)
	{
		hour += 24;
		daychange = true;
		julianday -= 1.0;
	}

	*hourOut=(INTNM::int32_t)hour;
	*minOut=(INTNM::int32_t)minute;
	*secOut=(INTNM::int32_t)second;

	if (hour > 11)
	{
		hour -= 12;
		PM = true;
	}

	if (hour == 0)
	{
		PM = false;
		hour = 12;
	}

	calcDayFromJD(julianday,yearOut,monthOut,dayOut);
}

//***********************************************************************/
//* Name:    timeStringAMPMDate							*/
//* Type:    Function									*/
//* Purpose: convert time of day in minutes to a zero-padded string	*/
//*		suitable for printing to the form text fields, and appends	*/
//*		the date.									*/
//* Arguments:										*/
//*   minutes : time of day in minutes						*/
//*   JD  : julian day									*/
//* Return value:										*/
//*   string of the format HH:MM[AM/PM] DDMon					*/
//***********************************************************************/

// timeStringAMPMDate returns a zero-padded string (HH:MM[AM/PM]) given time 
// in minutes and julian day, and appends the short date

void CSunriseSunsetCalc::timeStringAMPMDate(double minutes, double JD, INTNM::int32_t *yearOut, INTNM::int32_t *monthOut, INTNM::int32_t *dayOut, INTNM::int32_t *hourOut, INTNM::int32_t *minOut, INTNM::int32_t *secOut)
{
	double julianday = JD;
	double floatHour = minutes / 60;
	double hour = floor(floatHour);
	double floatMinute = 60 * (floatHour - floor(floatHour));
	double minute = floor(floatMinute);
	double floatSec = 60 * (floatMinute - floor(floatMinute));
	double second = floor(floatSec);

	minute += (second >= 30)? 1 : 0;

	if (minute >= 60) 
	{
		minute -= 60;
		hour ++;
	}

	if (hour > 23) 
	{
		hour -= 24;
		julianday += 1.0;
	}

	if (hour < 0)
	{
		hour += 24;
		julianday -= 1.0;
	}

	*hourOut=(INTNM::int32_t)hour;
	*minOut=(INTNM::int32_t)minute;
	*secOut=(INTNM::int32_t)second;

	bool PM = false;
	if (hour > 11)
	{
		hour -= 12;
		PM = true;
	}

	if (hour == 0)
	{
		PM = false;
		hour = 12;
	}

	calcDayFromJD(julianday,yearOut,monthOut,dayOut);
}

//***********************************************************************/
//* Name:    timeStringDate								*/
//* Type:    Function									*/
//* Purpose: convert time of day in minutes to a zero-padded 24hr time	*/
//*		suitable for printing to the form text fields.  If time	*/
//*		crosses a day boundary, date is appended.				*/
//* Arguments:										*/
//*   minutes : time of day in minutes						*/
//*   JD  : julian day									*/
//* Return value:										*/
//*   string of the format HH:MM (DDMon)						*/
//***********************************************************************/

// timeStringDate returns a zero-padded string (HH:MM) given time in minutes
// and julian day, and appends the short date if time crosses a day boundary

void CSunriseSunsetCalc::timeStringDate(double minutes, double JD, INTNM::int32_t *yearOut, INTNM::int32_t *monthOut, INTNM::int32_t *dayOut, INTNM::int32_t *hourOut, INTNM::int32_t *minOut, INTNM::int32_t *secOut)
{
	double julianday = JD;
	double floatHour = minutes / 60;
	double hour = floor(floatHour);
	double floatMinute = 60 * (floatHour - floor(floatHour));
	double minute = floor(floatMinute);
	double floatSec = 60 * (floatMinute - floor(floatMinute));
	double second = floor(floatSec);

	minute += (second >= 30)? 1 : 0;

	if (minute >= 60) 
	{
		minute -= 60;
		hour ++;
	}

	bool daychange = false;
	if (hour > 23) 
	{
		hour -= 24;
		julianday += 1.0;
		daychange = true;
	}

	if (hour < 0)
	{
		hour += 24;
		julianday -= 1.0;
		daychange = true;
	}

	*hourOut=(INTNM::int32_t)hour;
	*minOut=(INTNM::int32_t)minute;
	*secOut=(INTNM::int32_t)second;
	calcDayFromJD(julianday,yearOut,monthOut,dayOut);
}

//***********************************************************************/
//* Name:    calcSun									*/
//* Type:    Main Function called by form controls				*/
//* Purpose: calculate time of sunrise and sunset for the entered date	*/
//*		and location.  In the special cases near earth's poles, 	*/
//*		the date of nearest sunrise and set are reported.		*/
//* Arguments:										*/
//*   riseSetForm : for displaying results					*/
//*   latLongForm : for reading latitude and longitude data			*/
//*   index : daylight saving yes/no select					*/
//*   index2 : city select index							*/
//* Return value:										*/
//*   none											*/
//*	(fills riseSetForm text fields with results of calculations)	*/
//***********************************************************************/

INTNM::int16_t CSunriseSunsetCalc::calcSun(RISESET_IN_STRUCT &latLongForm, RISESET_OUT_STRUCT *riseSetForm)
{
	INTNM::int16_t retval = 0;
	INTNM::int32_t yearOut,monthOut, dayOut, hourOut, minOut, secOut;
	double latitude = latLongForm.Latitude;
	double longitude = latLongForm.Longitude;
// lat/lon are in degrees

// indexRS is the index of a month, which is the actual number minus 1.

	if (true)
	{
		if((latitude >= -90) && (latitude < -89.8))
		{
			latLongForm.Latitude = -89.8;
			latitude = -89.8;
		}
		if ((latitude <= 90) && (latitude > 89.8))
		{
			latLongForm.Latitude= 89.8;
			latitude = 89.8;
		}
		
		//*****	Calculate the time of sunrise			

//*********************************************************************/
//****************   NEW STUFF   ******   January, 2001   ****************
//*********************************************************************/

		double JD = calcJD(latLongForm.year, latLongForm.month, latLongForm.day);
		double doy = calcDayOfYear(latLongForm.month, latLongForm.day, isLeapYear(latLongForm.year));
		double T = calcTimeJulianCent(JD);
		calcSunRtAscension(T);
		double theta = calcSunDeclination(T);
		double Etime = calcEquationOfTime(T);

//*********************************************************************/

		double eqTime = Etime;
		double solarDec = theta;

		riseSetForm->eqTime= (floor(100*eqTime))/100;
		riseSetForm->solarDec = (floor(100*(solarDec)))/100;

		// Calculate sunrise for this date
		// if no sunrise is found, set flag nosunrise

		bool nosunrise = false;

		double riseTimeGMT = calcSunriseUTC(JD, latitude, longitude);
		if (riseTimeGMT==-9999)
		{
			nosunrise = true;
		}

		// Calculate sunset for this date
		// if no sunset is found, set flag nosunset

		bool nosunset = false;
		double setTimeGMT = calcSunsetUTC(JD, latitude, longitude);
		if (setTimeGMT==-9999)
		{
			nosunset = true;
		}

		double daySavings=latLongForm.DaytimeSaving?60:0;
		INTNM::int32_t zone = latLongForm.timezone;
		if(zone > 12 || zone < -12)
		{
			zone = 0;
			latLongForm.timezone = zone;
		}

		if (!nosunrise)		// Sunrise was found
		{
			double riseTimeLST = riseTimeGMT - (60 * zone) + daySavings;	
				//	in minutes
			timeStringShortAMPM(riseTimeLST, JD, &yearOut, &monthOut, &dayOut, &hourOut, &minOut, &secOut);
			riseSetForm->YearRise=yearOut;
			riseSetForm->MonthRise=monthOut;
			riseSetForm->DayRise=dayOut;
			riseSetForm->HourRise=hourOut;
			riseSetForm->MinRise=minOut;
			riseSetForm->SecRise=secOut;
			timeStringDate(riseTimeGMT, JD,&yearOut,&monthOut,&dayOut, &hourOut, &minOut,&secOut);
		}

		if (!nosunset)		// Sunset was found
		{
			double setTimeLST = setTimeGMT - (60 * zone) + daySavings;
			timeStringShortAMPM(setTimeLST, JD,&yearOut, &monthOut, &dayOut, &hourOut, &minOut, &secOut);
			riseSetForm->YearSet=yearOut;
			riseSetForm->MonthSet=monthOut;
			riseSetForm->DaySet=dayOut;
			riseSetForm->HourSet=hourOut;
			riseSetForm->MinSet=minOut;
			riseSetForm->SecSet=secOut;
			timeStringDate(setTimeGMT, JD,&yearOut,&monthOut,&dayOut, &hourOut,&minOut,&secOut);
		}

		// Calculate solar noon for this date

		double solNoonGMT = calcSolNoonUTC(T, longitude);
		double solNoonLST = solNoonGMT - (60 * zone) + daySavings;

		timeString(solNoonLST,&hourOut,&minOut,&secOut);
		riseSetForm->SolarNoonHour=hourOut;
		riseSetForm->SolarNoonMin=minOut;
		riseSetForm->SolarNoonSec=secOut;

		timeString(solNoonGMT,&hourOut,&minOut,&secOut);
		
		// report special cases of no sunrise

		if(nosunrise)
		{
			// if Northern hemisphere and spring or summer, OR  
			// if Southern hemisphere and fall or winter, use 
			// previous sunrise and next sunset

			if ( ((latitude > 66.4) && (doy > 79) && (doy < 267)) ||
			   ((latitude < -66.4) && ((doy < 83) || (doy > 263))) )
			{
				double newjd = findRecentSunrise(JD, latitude, longitude);
				double tempnewTime=calcSunriseUTC(newjd, latitude, longitude);
				double newtime = tempnewTime - (60 * zone) + daySavings;
				if (newtime > 1440)
				{
					newtime -= 1440;
					newjd += 1.0;
				}
				if (newtime < 0)
				{
					newtime += 1440;
					newjd -= 1.0;
				}
				timeStringAMPMDate(newtime, newjd,&yearOut,&monthOut,&dayOut,&hourOut,&minOut,&secOut);
				riseSetForm->YearRise=yearOut;
				riseSetForm->MonthRise=monthOut;
				riseSetForm->DayRise=dayOut;
				riseSetForm->HourRise=hourOut;
				riseSetForm->MinRise=minOut;
				riseSetForm->SecRise=secOut;
			}

			// if Northern hemisphere and fall or winter, OR 
			// if Southern hemisphere and spring or summer, use 
			// next sunrise and previous sunset

			else if ( ((latitude > 66.4) && ((doy < 83) || (doy > 263))) ||
				((latitude < -66.4) && (doy > 79) && (doy < 267)) )
			{
				double newjd = findNextSunrise(JD, latitude, longitude);
				double newtime = calcSunriseUTC(newjd, latitude, longitude)
					- (60 * zone) + daySavings;
				if (newtime > 1440)
				{
					newtime -= 1440;
					newjd += 1.0;
				}
				if (newtime < 0)
				{
					newtime += 1440;
					newjd -= 1.0;
				}
				timeStringAMPMDate(newtime, newjd,&yearOut,&monthOut,&dayOut,&hourOut,&minOut,&secOut);
				riseSetForm->YearRise=yearOut;
				riseSetForm->MonthRise=monthOut;
				riseSetForm->DayRise=dayOut;
				riseSetForm->HourRise=hourOut;
				riseSetForm->MinRise=minOut;
				riseSetForm->SecRise=secOut;
			}
			else 
			{
				retval |= NO_SUNRISE;
			}
		}

		if(nosunset)
		{ 
			// if Northern hemisphere and spring or summer, OR
			// if Southern hemisphere and fall or winter, use 
			// previous sunrise and next sunset

			if ( ((latitude > 66.4) && (doy > 79) && (doy < 267)) ||
			   ((latitude < -66.4) && ((doy < 83) || (doy > 263))) )
			{
				double newjd = findNextSunset(JD, latitude, longitude);
				double newtime = calcSunsetUTC(newjd, latitude, longitude)
					- (60 * zone) + daySavings;
				if (newtime > 1440)
				{
					newtime -= 1440;
					newjd += 1.0;
				}
				if (newtime < 0)
				{
					newtime += 1440;
					newjd -= 1.0;
				}
				timeStringAMPMDate(newtime, newjd,&yearOut,&monthOut,&dayOut,&hourOut,&minOut,&secOut);
				riseSetForm->YearSet=yearOut;
				riseSetForm->MonthSet=monthOut;
				riseSetForm->DaySet=dayOut;
				riseSetForm->HourSet=hourOut;
				riseSetForm->MinSet=minOut;
				riseSetForm->SecSet=secOut;
			}

			// if Northern hemisphere and fall or winter, OR
			// if Southern hemisphere and spring or summer, use 
			// next sunrise and last sunset

			else if ( ((latitude > 66.4) && ((doy < 83) || (doy > 263))) ||
				((latitude < -66.4) && (doy > 79) && (doy < 267)) )
			{
				double newjd = findRecentSunset(JD, latitude, longitude);
				double newtime = calcSunsetUTC(newjd, latitude, longitude)
					- (60 * zone) + daySavings;
				if (newtime > 1440)
				{
					newtime -= 1440;
					newjd += 1.0;
				}
				if (newtime < 0)
				{
					newtime += 1440;
					newjd -= 1.0;
				}
					timeStringAMPMDate(newtime, newjd,&yearOut,&monthOut,&dayOut,&hourOut,&minOut,&secOut);
				riseSetForm->YearSet=yearOut;
				riseSetForm->MonthSet=monthOut;
				riseSetForm->DaySet=dayOut;
				riseSetForm->HourSet=hourOut;
				riseSetForm->MinSet=minOut;
				riseSetForm->SecSet=secOut;
			}

			else 
			{
				retval |= NO_SUNSET;
			}
		}
	}
	return retval;
}
