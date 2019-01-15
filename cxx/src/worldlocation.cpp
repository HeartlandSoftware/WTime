/**
 * WorldLocation.cpp
 *
 * Copyright 2016-2018 Heartland Software Solutions Inc.
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
#include "worldlocation.h"
#include "SunriseSunsetCalc.h"

#include <cmath>
#include <boost/algorithm/string/predicate.hpp>

extern bool insideCanadaDetail(class Canada **canada, const double latitude, const double longitude);
extern void insideCanadaCleanup(class Canada *canada);

using namespace HSS_Time;
using namespace HSS_Time_Private;


#define Pi 3.14159265358979323846264
#define TwoPi 6.28318530717958647692529

static __inline double DEGREE_TO_RADIAN(const double X) {
	return (X / 180.0) * 3.14159265358979323846264;
}


static __inline double RADIAN_TO_DEGREE(const double X) {
	return (X * 180.0) * 0.318309886183790671537768;
}


// this list is generated from: http://www.timeanddate.com/library/abbreviations/timezones/

const TimeZoneInfo WorldLocation::m_std_timezones[] = {
	{ WTimeSpan(0, 9, 30, 0),	WTimeSpan(0),			"ACST",	"Australian Central Standard Time" },
	{ WTimeSpan(0, 10, 0, 0),	WTimeSpan(0),			"AEST",	"Australian Eastern Standard Time" },
	{ WTimeSpan(0, -9, 0, 0),	WTimeSpan(0),			"AKST",	"Alaska Standard Time" },
	{ WTimeSpan(0, -4, 0, 0),	WTimeSpan(0),			"AST",	"Atlantic Standard Time" },
	{ WTimeSpan(0, 8, 0, 0),	WTimeSpan(0),			"AWST", "Australian Western Standard Time" },
	{ WTimeSpan(0, 1, 0, 0),	WTimeSpan(0),			"CET",	"Central European Time" },	// 5
	{ WTimeSpan(0, -6, 0, 0),	WTimeSpan(0),			"CST",	"Central Standard Time" },
	{ WTimeSpan(0, 7, 0, 0),	WTimeSpan(0),			"CXT",	"Christmas Island Time" },
	{ WTimeSpan(0, 2, 0, 0),	WTimeSpan(0),			"EET",	"Eastern European Time" },
	{ WTimeSpan(0, -5, 0, 0),	WTimeSpan(0),			"EST",	"Eastern Standard Time" },
	{ WTimeSpan(0, -10, 0, 0),	WTimeSpan(0),			"HAST",	"Hawaii-Aleutian Standard Time" },	// 10
	{ WTimeSpan(0, 3, 0, 0),	WTimeSpan(0),			"MSK",	"Moscow Standard Time" },
	{ WTimeSpan(0, -7, 0, 0),	WTimeSpan(0),			"MST",	"Mountain Standard Time" },
	{ WTimeSpan(0, 11, 30, 0),	WTimeSpan(0),			"NFT",	"Norfolk (Island) Time" },
	{ WTimeSpan(0, -3, -30, 0),	WTimeSpan(0),			"NST",	"Newfoundland Standard Time" },
	{ WTimeSpan(0, 12, 0, 0),	WTimeSpan(0),			"NZST",	"New Zealand Standard Time" },	// 15
	{ WTimeSpan(0, -8, 0, 0),	WTimeSpan(0),			"PST",	"Pacific Standard Time" },
	{ WTimeSpan(0, 0, 0, 0),	WTimeSpan(0),			"UTC",	"Universal Coordinated Time" },
	{ WTimeSpan(0, 2, 0, 0),	WTimeSpan(0),			"RZ1",	"Russian Zone 1" },
	{ WTimeSpan(0, 3, 0, 0),	WTimeSpan(0),			"RZ2",	"Russian Zone 2" },
	{ WTimeSpan(0, 4, 0, 0),	WTimeSpan(0),			"RZ3",	"Russian Zone 3" },	// 20
	{ WTimeSpan(0, -1, 0, 0),	WTimeSpan(0),			"WAT",	"West African Time" },
	{ WTimeSpan(0, -2, 0, 0),	WTimeSpan(0),			"AT",	"Azores Time" },
	{ WTimeSpan(0, -11, 0, 0),	WTimeSpan(0),			"NT",	"Nome Time" },
	{ WTimeSpan(0, 5, 30, 0),	WTimeSpan(0),			"IST",	"Indian Standard Time" },
	{ WTimeSpan(0, 8, 0, 0),	WTimeSpan(0),			"CCT",	"China Coast Time" },	// 25
	{ WTimeSpan(0, 9, 0, 0),	WTimeSpan(0),			"JST",	"Japan Standard Time" },
	{ WTimeSpan(0, 10, 0, 0),	WTimeSpan(0),			"GST",	"Guam Standard Time" },
	{ WTimeSpan(0, 0, 0, 0),	WTimeSpan(0),			NULL,	NULL }
};


const ::TimeZoneInfo WorldLocation::m_dst_timezones[] = {
	{ WTimeSpan(0, 9, 30, 0),	WTimeSpan(0, 1, 0, 0),	"ACDT",	"Australian Central Daylight Time" },
	{ WTimeSpan(0, -4, 0, 0),	WTimeSpan(0, 1, 0, 0),	"ADT",	"Atlantic Daylight Time" },
	{ WTimeSpan(0, 10, 0, 0),	WTimeSpan(0, 1, 0, 0),	"AEDT",	"Australian Eastern Daylight Time" },
	{ WTimeSpan(0, -9, 0, 0),	WTimeSpan(0, 1, 0, 0),	"AKDT",	"Alaska Daylight Time" },
	{ WTimeSpan(0, 8, 0, 0),	WTimeSpan(0, 1, 0, 0), 	"AWDT", "Australian Western Daylight Time" },
	{ WTimeSpan(0, 0, 0, 0),	WTimeSpan(0, 1, 0, 0),	"BST",	"British Summer Time" },	// 5
	{ WTimeSpan(0, -6, 0, 0),	WTimeSpan(0, 1, 0, 0),	"CDT",	"Central Daylight Time" },
	{ WTimeSpan(0, 1, 0, 0),	WTimeSpan(0, 1, 0, 0),	"CEDT",	"Central European Daylight Time" },
	{ WTimeSpan(0, -5, 0, 0),	WTimeSpan(0, 1, 0, 0),	"EDT",	"Eastern Daylight Time" },
	{ WTimeSpan(0, 2, 0, 0),	WTimeSpan(0, 1, 0, 0),	"EEDT",	"Eastern European Daylight Time" },
	{ WTimeSpan(0, -10, 0, 0),	WTimeSpan(0, 1, 0, 0),	"HADT",	"Hawaii-Aleutian Daylight Time" },	// 10
	{ WTimeSpan(0, 0, 0, 0),	WTimeSpan(0, 1, 0, 0),	"IST",	"Irish Summer Time" },
	{ WTimeSpan(0, -7, 0, 0),	WTimeSpan(0, 1, 0, 0),	"MDT",	"Mountain Daylight Time" },
	{ WTimeSpan(0, 3, 0, 0),	WTimeSpan(0, 1, 0, 0),	"MSD",	"Moscow Daylight Time" },
	{ WTimeSpan(0, -3, -30, 0),	WTimeSpan(0, 1, 0, 0),	"NDT",	"Newfoundland Daylight Time" },
	{ WTimeSpan(0, 12, 0, 0),	WTimeSpan(0, 1, 0, 0),	"NZDT",	"New Zealand Daylight Time" },	// 15
	{ WTimeSpan(0, -8, 0, 0),	WTimeSpan(0, 1, 0, 0),	"PDT",	"Pacific Daylight Time" },
	{ WTimeSpan(0, 0, 0, 0),	WTimeSpan(0, 1, 0, 0),	"WEDT",	"Western European Daylight Time" },
	{ WTimeSpan(0, 0, 0, 0),	WTimeSpan(0),		NULL,	NULL }
};


const ::TimeZoneInfo WorldLocation::m_mil_timezones[] = {
	{ WTimeSpan(0, 0, 0, 0),	WTimeSpan(0),		"Z",	"Zulu Time Zone" },
	{ WTimeSpan(0, 1, 0, 0),	WTimeSpan(0),		"A",	"Alpha Time Zone" },
	{ WTimeSpan(0, 2, 0, 0),	WTimeSpan(0),		"B",	"Bravo Time Zone" },
	{ WTimeSpan(0, 3, 0, 0),	WTimeSpan(0),		"C",	"Charlie Time Zone" },
	{ WTimeSpan(0, 4, 0, 0),	WTimeSpan(0),		"D",	"Delta Time Zone" },
	{ WTimeSpan(0, 5, 0, 0),	WTimeSpan(0),		"E",	"Echo Time Zone" },	// 5
	{ WTimeSpan(0, 6, 0, 0),	WTimeSpan(0),		"F",	"Foxtrot Time Zone" },
	{ WTimeSpan(0, 7, 0, 0),	WTimeSpan(0),		"G",	"Golf Time Zone" },
	{ WTimeSpan(0, 8, 0, 0),	WTimeSpan(0),		"H",	"Hotel Time Zone" },
	{ WTimeSpan(0, 9, 0, 0),	WTimeSpan(0),		"I",	"India Time Zone" },
	{ WTimeSpan(0, 10, 0, 0),	WTimeSpan(0),		"K",	"Kilo Time Zone" },	// 10
	{ WTimeSpan(0, 11, 0, 0),	WTimeSpan(0),		"L",	"Lima Time Zone" },
	{ WTimeSpan(0, 12, 0, 0),	WTimeSpan(0),		"M",	"Mike Time Zone" },
	{ WTimeSpan(0, -1, 0, 0),	WTimeSpan(0),		"N",	"November Time Zone" },
	{ WTimeSpan(0, -2, 0, 0),	WTimeSpan(0),		"O",	"Oscar Time Zone" },
	{ WTimeSpan(0, -3, 0, 0),	WTimeSpan(0),		"P",	"Papa Time Zone" },	// 15
	{ WTimeSpan(0, -4, 0, 0),	WTimeSpan(0),		"Q",	"Quebec Time Zone" },
	{ WTimeSpan(0, -5, 0, 0),	WTimeSpan(0),		"R",	"Romeo Time Zone" },
	{ WTimeSpan(0, -6, 0, 0),	WTimeSpan(0),		"S",	"Sierra Time Zone" },
	{ WTimeSpan(0, -7, 0, 0),	WTimeSpan(0),		"T",	"Tango Time Zone" },
	{ WTimeSpan(0, -8, 0, 0),	WTimeSpan(0),		"U",	"Uniform Time Zone" }, // 20
	{ WTimeSpan(0, -9, 0, 0),	WTimeSpan(0),		"V",	"Vector Time Zone" },
	{ WTimeSpan(0, -10, 0, 0),	WTimeSpan(0),		"W",	"Whiskey Time Zone" },
	{ WTimeSpan(0, -11, 0, 0),	WTimeSpan(0),		"X",	"X-ray Time Zone" },
	{ WTimeSpan(0, -12, 0, 0),	WTimeSpan(0),		"Y",	"Yankee Time Zone" },
	{ WTimeSpan(0),		WTimeSpan(0),		NULL,	NULL }
};


bool WorldLocation::InsideCanada() const {
	return ((WorldLocation *)this)->InsideCanada(m_latitude, m_longitude);
}


bool WorldLocation::InsideCanada(const double latitude, const double longitude) {
	if (latitude < DEGREE_TO_RADIAN(41.0))		return false;
	if (latitude > DEGREE_TO_RADIAN(83.0))		return false;
	if (longitude < DEGREE_TO_RADIAN(-141.0))	return false;
	if (longitude > DEGREE_TO_RADIAN(-52.0))	return false;

#if defined(_MSC_VER) || defined(_USE_CANADA)
	return insideCanadaDetail(&canada, RADIAN_TO_DEGREE(latitude), RADIAN_TO_DEGREE(longitude));
#else
	if (longitude < DEGREE_TO_RADIAN(-122.8)) {
		if (latitude < DEGREE_TO_RADIAN(48.3))	return false;
	}
	else if (longitude < DEGREE_TO_RADIAN(-95.153)) {
		if (latitude < DEGREE_TO_RADIAN(49.0))	return false;
	}
	else if (longitude < DEGREE_TO_RADIAN(-88.0)) {
		if (latitude < DEGREE_TO_RADIAN(48.0))	return false;
	}
	else if (longitude < DEGREE_TO_RADIAN(-83.5)) {
		if (latitude < DEGREE_TO_RADIAN(45.5))	return false;
	}
	else if (longitude < DEGREE_TO_RADIAN(-78.7)) {
		if (latitude < DEGREE_TO_RADIAN(41.66))	return false;
	}
	else if (longitude < DEGREE_TO_RADIAN(-74.75)) {
		if (latitude < DEGREE_TO_RADIAN(43.65))	return false;
	}
	else if (longitude < DEGREE_TO_RADIAN(-67.31)) {
		if (latitude < DEGREE_TO_RADIAN(45))	return false;
	}
	else {
		if (latitude < DEGREE_TO_RADIAN(43.25))	return false;
	}
	return true;
#endif
}


bool WorldLocation::InsideNewZealand() const {
	if ((m_longitude > DEGREE_TO_RADIAN(172.5)) && (m_longitude < DEGREE_TO_RADIAN(178.6))) {
		if ((m_latitude > DEGREE_TO_RADIAN(-41.75)) && (m_latitude < DEGREE_TO_RADIAN(-34.3))) {	// general extents of New Zealand's north island
			return true;
		}
	}

	if ((m_longitude > DEGREE_TO_RADIAN(166.3)) && (m_longitude < DEGREE_TO_RADIAN(174.5))) {
		if ((m_latitude > DEGREE_TO_RADIAN(-47.35)) && (m_latitude < DEGREE_TO_RADIAN(40.4))) {	// general extents of New Zealand's south island
			return true;
		}
	}
	return false;
}


bool WorldLocation::InsideTasmania() const {
	if ((m_longitude > DEGREE_TO_RADIAN(143.5)) && (m_longitude < DEGREE_TO_RADIAN(149.0))) {
		if ((m_latitude > DEGREE_TO_RADIAN(-44.0)) && (m_latitude < DEGREE_TO_RADIAN(-39.5))) {	// general extents of Tasmania
			return true;
		}
	}

	return false;
}


bool WorldLocation::InsideAustraliaMainland() const {
	if ((m_longitude > DEGREE_TO_RADIAN(113.15)) && (m_longitude < DEGREE_TO_RADIAN(153.633333))) {
		if ((m_latitude > DEGREE_TO_RADIAN(-39.133333)) && (m_latitude < DEGREE_TO_RADIAN(-10.683333))) {	// general extents of continental Australia
			return true;
		}
	}

	return false;
}


const ::TimeZoneInfo *WorldLocation::GuessTimeZone(INTNM::int16_t set) const {
	bool valid;
	const TimeZoneInfo *tzi = WorldLocation::TimeZoneFromLatLon(m_latitude, m_longitude, set, &valid);
	if (!valid)
	{
		if (InsideNewZealand()) {
			if (set == 0)
				return &m_std_timezones[15];
			else if (set == 1)
				return &m_dst_timezones[15];
		} else if (InsideTasmania()) {
			if (set == 0)
				return &m_std_timezones[1];
			else if (set == 1)
				return &m_dst_timezones[2];
		}

		double longitude = m_longitude;
		while (longitude < -Pi)
			longitude += TwoPi;
		while (longitude > Pi)
			longitude -= TwoPi;

		const ::TimeZoneInfo *tz;
		double variation = TwoPi;
		if (set == 1)		tz = m_dst_timezones;
		else if (set == -1)	tz = m_mil_timezones;
		else if (set == 0)	tz = m_std_timezones;
		else			return NULL;

		while (tz->m_name) {
			double ideal_longitude = ((double)tz->m_timezone.GetTotalSeconds()) / (double)(12.0 * 60.0 * 60.0) * Pi;
			double offset_longitude = fabs(longitude - ideal_longitude);
			if (variation > offset_longitude) {
				variation = offset_longitude;
				tzi = tz;
			}
			tz++;
		}
	}
	return tzi;
}


const ::TimeZoneInfo *WorldLocation::CurrentTimeZone(INTNM::int16_t set) const {
	const ::TimeZoneInfo *tz;
	if (set == -1)	tz = m_mil_timezones;
	else if (m_startDST == m_endDST)	tz = m_std_timezones;
	else					tz = m_dst_timezones;

	while (tz->m_code) {
		if (tz->m_timezone == m_timezone)
			break;
		tz++;
	}
	if (tz->m_code)
		return tz;
	return NULL;
}

const ::TimeZoneInfo* WorldLocation::TimeZoneFromName(const std::string& name, INTNM::int16_t set)
{
	const ::TimeZoneInfo *tz;
	if (set == -1)	tz = m_mil_timezones;
	else if (set)	tz = m_dst_timezones;
	else			tz = m_std_timezones;

	while (tz->m_code)
	{
		if (boost::iequals(tz->m_code, name) || boost::iequals(tz->m_name, name))
			break;
		tz++;
	}
	if (tz->m_code)
		return tz;
	return nullptr;
}


WorldLocation::WorldLocation()
#ifdef HSS_USE_CACHING
	: m_sunCache(2), m_solarCache(2)
#endif
{
	m_latitude = 1000.0;
	m_longitude = 1000.0;
	m_timezone = WTimeSpan(0);
	m_startDST = WTimeSpan(0);
	m_endDST = WTimeSpan(0);
	m_amtDST = WTimeSpan(0, 1, 0, 0);

	canada = nullptr;
}


WorldLocation::WorldLocation(const WorldLocation &wl)
#ifdef HSS_USE_CACHING
	: m_sunCache(2), m_solarCache(2)
#endif
{
	*this = wl;

	canada = nullptr;
}


WorldLocation::WorldLocation(double latitude, double longitude, bool guessTimezone)
#ifdef HSS_USE_CACHING
	: m_sunCache(2), m_solarCache(2)
#endif
{
	m_latitude = DEGREE_TO_RADIAN(latitude);
	m_longitude = DEGREE_TO_RADIAN(longitude);
	if (guessTimezone)
	{
		const TimeZoneInfo *info = GuessTimeZone(0);
		m_timezone = info->m_timezone;
		if (info->m_dst.GetTotalSeconds() > 0)
		{
			m_startDST = WTimeSpan(0);
			m_endDST = WTimeSpan(366, 0, 0, 0);
			m_amtDST = info->m_dst;
		}
	}

	canada = nullptr;
}


WorldLocation::~WorldLocation() {
#if defined(_MSC_VER) || defined(_USE_CANADA)
	insideCanadaCleanup(canada);
#endif
}


WorldLocation &WorldLocation::operator=(const WorldLocation &wl) {
	if (&wl != this) {
		m_latitude = wl.m_latitude;
		m_longitude = wl.m_longitude;
		m_timezone = wl.m_timezone;
		m_startDST = wl.m_startDST;
		m_endDST = wl.m_endDST;
		m_amtDST = wl.m_amtDST;

#ifdef HSS_USE_CACHING
		m_sunCache.Clear();
		m_solarCache.Clear();
#endif

	}
	return *this;
}


bool WorldLocation::operator==(const WorldLocation &wl) const {
	if (&wl == this)
		return true;
	if ((m_latitude == wl.m_latitude) &&
	    (m_longitude == wl.m_longitude) &&
	    (m_timezone == wl.m_timezone) &&
	    (m_startDST == wl.m_startDST) &&
	    (m_endDST == wl.m_endDST) &&
	    (m_amtDST == wl.m_amtDST))
		return true;
	return false;
}


bool WorldLocation::operator!=(const WorldLocation &wl) const {
	return !(*this == wl);
}


WTimeSpan WorldLocation::m_solar_timezone(const WTime &solar_time) const {
#ifdef HSS_USE_CACHING
	struct sun_key sk;
	sk.m_sun_cache_lat = m_latitude;
	sk.m_sun_cache_long = m_longitude;
	sk.m_sun_cache_tm = solar_time.GetTime(0);
	WTimeSpan retval;
	if (m_solarCache.Retrieve(&sk, &retval))
		return retval;
#endif

	INTNM::int32_t	day = solar_time.GetDay(WTIME_FORMAT_AS_LOCAL),
		year = solar_time.GetYear(WTIME_FORMAT_AS_LOCAL),
		month = solar_time.GetMonth(WTIME_FORMAT_AS_LOCAL);

	CSunriseSunsetCalc calculator;
	RISESET_IN_STRUCT sInput;
	sInput.Latitude = RADIAN_TO_DEGREE(m_latitude);
	sInput.Longitude = -RADIAN_TO_DEGREE(m_longitude);
	sInput.timezone = 0;
	sInput.DaytimeSaving = false;
	sInput.year = year;
	sInput.month = month;
	sInput.day = day;
	RISESET_OUT_STRUCT sOut;
	calculator.calcSun(sInput,&sOut);

	WTimeSpan solarTime(0, sOut.SolarNoonHour - 12, sOut.SolarNoonMin, (INTNM::int32_t)sOut.SolarNoonSec);
	WTimeSpan result((INTNM::int64_t)0 - solarTime.GetTotalSeconds());
	
#ifdef HSS_USE_CACHING
	m_solarCache.Store(&sk, &result);
#endif
	return result;
}


#if defined(TIMES_WINDOWS) && !defined(_NO_MFC)
CArchive& HSS_Time::operator>>(CArchive& is, WorldLocation &wl) {
	union {
		short svalue[4];
		double dvalue;
	} loader;
	is >> loader.svalue[0] >> loader.svalue[1];
	if (loader.svalue[0] == -1) {
		if ((loader.svalue[1] != 1) && (loader.svalue[1] != 2) && (loader.svalue[1] != 3) && (loader.svalue[1] != 4))
			AfxThrowArchiveException(CArchiveException::badSchema, _T("World Location"));
		is >> wl.m_latitude >> wl.m_longitude;
		if (loader.svalue[1] < 3) {
			INTNM::int32_t timezone;
			is >> timezone;
			wl.m_timezone = WTimeSpan(timezone);
		} else	is >> wl.m_timezone;
		if (loader.svalue[1] < 4) {
			INTNM::int16_t spheroid;
			is >> spheroid;
		}
		if (loader.svalue[1] == 2) {
			INTNM::int32_t ts;
			is >> ts; wl.m_startDST = WTimeSpan(ts);
			is >> ts; wl.m_endDST = WTimeSpan(ts);
			is >> ts; wl.m_amtDST = WTimeSpan(ts);
		} else if (loader.svalue[1] >= 3)
			is >> wl.m_startDST >> wl.m_endDST >> wl.m_amtDST;
	} else {
		is >> loader.svalue[2] >> loader.svalue[3];
		wl.m_latitude = loader.dvalue;
		is >> wl.m_longitude >> wl.m_timezone;
	}
	return is;
}


CArchive& HSS_Time::operator<<(CArchive& os, const WorldLocation &wl) {
	short hickup = -1;
	short version = 4;		// 4 removed m_spheroid
	os << hickup << version << wl.m_latitude << wl.m_longitude << wl.m_timezone;
	os << wl.m_startDST << wl.m_endDST << wl.m_amtDST;
	return os;
}
#endif


INTNM::int16_t WorldLocation::m_sun_rise_set(const WTime &daytime, WTime *Rise, WTime *Set, WTime *Noon) const {
#ifdef HSS_USE_CACHING
	struct sun_key sk;
	sk.m_sun_cache_lat = m_latitude;
	sk.m_sun_cache_long = m_longitude;
	sk.m_sun_cache_tm = daytime.GetTime(0);
	struct sun_val sv;
	if (m_sunCache.Retrieve(&sk, &sv)) {
		*Rise = WTime(sv.m_sun_cache_rise, Rise->GetTimeManager());
		*Set = WTime(sv.m_sun_cache_set, Set->GetTimeManager());
		*Noon = WTime(sv.m_sun_cache_noon, Noon->GetTimeManager());
		return sv.m_success;
	}
#endif

	INTNM::int32_t	day = daytime.GetDay(WTIME_FORMAT_AS_SOLAR),
		year = daytime.GetYear(WTIME_FORMAT_AS_SOLAR),
		month = daytime.GetMonth(WTIME_FORMAT_AS_SOLAR);

	CSunriseSunsetCalc calculator;
	RISESET_IN_STRUCT sInput;
	sInput.Latitude = RADIAN_TO_DEGREE(m_latitude);
	sInput.Longitude = -RADIAN_TO_DEGREE(m_longitude);
	sInput.timezone = 0;
	sInput.DaytimeSaving = false;
	sInput.year = year;
	sInput.month = month;
	sInput.day = day;
	RISESET_OUT_STRUCT sOut;
	INTNM::int16_t success = calculator.calcSun(sInput,&sOut);

	WTime riseTime(0ULL, Rise->GetTimeManager()),
		setTime(0ULL, Rise->GetTimeManager()),
		noonTime(0ULL, Noon->GetTimeManager());
	if (!(success & NO_SUNRISE))
		riseTime = WTime(sOut.YearRise,sOut.MonthRise,sOut.DayRise,sOut.HourRise,sOut.MinRise,(INTNM::int32_t)sOut.SecRise,Rise->GetTimeManager());
	*Rise = riseTime;
	if (!(success & NO_SUNSET))
		setTime = WTime(sOut.YearSet,sOut.MonthSet,sOut.DaySet,sOut.HourSet,sOut.MinSet,(INTNM::int32_t)sOut.SecSet,Set->GetTimeManager());
		*Set = setTime;
	noonTime = WTime(sInput.year,sInput.month,sInput.day, sOut.SolarNoonHour, sOut.SolarNoonMin, (INTNM::int32_t)sOut.SolarNoonSec,Noon->GetTimeManager());
		*Noon = noonTime;
		
#ifdef HSS_USE_CACHING
	sk.m_sun_cache_tm = daytime.GetTime(0);
	sk.m_sun_cache_lat = m_latitude;
	sk.m_sun_cache_long = m_longitude;
	sv.m_sun_cache_rise = Rise->GetTotalSeconds();
	sv.m_sun_cache_set = Set->GetTotalSeconds();
	sv.m_sun_cache_noon = Noon->GetTotalSeconds();
	sv.m_success = success;
	m_sunCache.Store(&sk, &sv);
#endif
	return success;
}
