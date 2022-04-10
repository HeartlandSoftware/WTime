/**
 * SunriseSunsetCalc.h
 *
 * Copyright 2016-2022 Heartland Software Solutions Inc.
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

#pragma once

#ifdef MSVC_COMPILER
#pragma managed(push, off)
#endif


#include "times_internal.h"

#include <string>

#ifdef HSS_SHOULD_PRAGMA_PACK
#pragma pack(push, 4)
#endif

namespace HSS_Time_Private {

typedef struct TIMES_API _RiseSetInput_struct{
	double Latitude, Longitude;		// west is positive, east is negative
	INTNM::int32_t year, month, day;
	INTNM::int32_t timezone;		// west is positive, east is negative
	bool DaytimeSaving;
} RISESET_IN_STRUCT;

typedef struct TIMES_API _RiseSetOut_struct{
	INTNM::int32_t YearRise,MonthRise,DayRise;
	INTNM::int32_t YearSet, MonthSet, DaySet;
	INTNM::int32_t HourRise, MinRise, SecRise;
	INTNM::int32_t HourSet, MinSet, SecSet;
	INTNM::int32_t SolarNoonHour, SolarNoonMin, SolarNoonSec;
	double eqTime;
	double solarDec;
	_RiseSetOut_struct() = default;
} RISESET_OUT_STRUCT;

};


namespace HSS_Time_Private {
class TIMES_API CSunriseSunsetCalc
{
public:
	CSunriseSunsetCalc();
	~CSunriseSunsetCalc(){};
	INTNM::int16_t calcSun(RISESET_IN_STRUCT &latLongForm, RISESET_OUT_STRUCT *riseSetForm);
		#define NO_SUNRISE	0x0001
		#define NO_SUNSET	0x0002
	static bool isLeapYear(INTNM::int32_t yr); 

private:
	double radToDeg(double angleRad);
	double degToRad(double angleDeg);
	INTNM::int32_t calcDayOfYear(INTNM::int32_t mn, INTNM::int32_t dy, bool lpyr);
	const std::string calcDayOfWeek(double juld,INTNM::int32_t *day);
	double calcJD(INTNM::int32_t year, INTNM::int32_t month, INTNM::int32_t day);
	void calcDayFromJD(double jd, INTNM::int32_t *yearOut, INTNM::int32_t *monthOut, INTNM::int32_t *dayOut);
	double calcTimeJulianCent(double jd);
	double calcJDFromJulianCent(double t);
	double calcGeomMeanLongSun(double t);
	double calcGeomMeanAnomalySun(double t);
	double calcEccentricityEarthOrbit(double t);
	double calcSunEqOfCenter(double t);
	double calcSunTrueLong(double t);
	double calcSunTrueAnomaly(double t);
	double calcSunRadVector(double t);
	double calcSunApparentLong(double t);
	double calcMeanObliquityOfEcliptic(double t);
	double calcObliquityCorrection(double t);
	double calcSunRtAscension(double t);
	double calcSunDeclination(double t);
	double calcEquationOfTime(double t);
	double calcHourAngleSunrise(double lat, double solarDec);
	double calcHourAngleSunset(double lat, double solarDec);
	double calcSunriseUTC(double JD, double latitude, double longitude);
	double calcSolNoonUTC(double t, double longitude);
	double calcSunsetUTC(double JD, double latitude, double longitude);
	double findRecentSunrise(double jd, double latitude, double longitude);
	double findRecentSunset(double jd, double latitude, double longitude);
	double findNextSunrise(double jd, double latitude, double longitude);
	double findNextSunset(double jd, double latitude, double longitude);
	void timeString(double minutes, INTNM::int32_t *hourOut, INTNM::int32_t *minOut, INTNM::int32_t *secOut);
	void timeStringShortAMPM(double minutes, double JD, INTNM::int32_t *yearOut, INTNM::int32_t *monthOut, INTNM::int32_t *dayOut, INTNM::int32_t *hourOut, INTNM::int32_t *minOut, INTNM::int32_t *secOut);
	void timeStringAMPMDate(double minutes, double JD, INTNM::int32_t *yearOut, INTNM::int32_t *monthOut, INTNM::int32_t *dayOut, INTNM::int32_t *hourOut, INTNM::int32_t *minOut, INTNM::int32_t *secOut);
	void timeStringDate(double minutes, double JD, INTNM::int32_t *yearOut, INTNM::int32_t *monthOut, INTNM::int32_t *dayOut, INTNM::int32_t *hourOut, INTNM::int32_t *minOut, INTNM::int32_t *secOut);
};

}

#ifdef HSS_SHOULD_PRAGMA_PACK
#pragma pack(pop)
#endif

#ifdef MSVC_COMPILER
#pragma managed(pop)
#endif
