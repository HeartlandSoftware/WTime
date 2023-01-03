/**
 * WorldLocation.cpp
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

#include "TimeZoneMapper.h"
#include "times_internal.h"
#include "worldlocation.h"
#include "SunriseSunsetCalc.h"
#include "str_printf.h"
#include "Borders.h"

#include <cmath>
#include <vector>
#include <boost/algorithm/string/predicate.hpp>
#include "boost_bimap.h"


using namespace HSS_Time;
using namespace HSS_Time_Private;


#define Pi 3.14159265358979323846264
#define TwoPi 6.28318530717958647692529

static double DEGREE_TO_RADIAN(const double X) {
	return (X / 180.0) * 3.14159265358979323846264;
}


static double RADIAN_TO_DEGREE(const double X) {
	return (X * 180.0) * 0.318309886183790671537768;
}


// this list is generated from: http://www.timeanddate.com/library/abbreviations/timezones/

constexpr int STD_TIMEZONE_ID = 0x10000;
constexpr int DST_TIMEZONE_ID = 0x20000;
constexpr int MIL_TIMEZONE_ID = 0x40000;
constexpr std::uint32_t MAKE_ID(int type, int id) { return type | id; }
constexpr bool IS_STD(int id) { return (STD_TIMEZONE_ID & id) != 0; }
constexpr bool IS_DST(int id) { return (DST_TIMEZONE_ID & id) != 0; }
constexpr bool IS_MIL(int id) { return (MIL_TIMEZONE_ID & id) != 0; }


const TimeZoneInfo WorldLocation::m_std_timezones[] = {
	{ WTimeSpan(0, 9, 30, 0),	WTimeSpan(0),			"ACST",	"Australian Central Standard Time", MAKE_ID(STD_TIMEZONE_ID, 0) },
	{ WTimeSpan(0, 10, 0, 0),	WTimeSpan(0),			"AEST",	"Australian Eastern Standard Time", MAKE_ID(STD_TIMEZONE_ID, 1) },
	{ WTimeSpan(0, -9, 0, 0),	WTimeSpan(0),			"AKST",	"Alaska Standard Time",				MAKE_ID(STD_TIMEZONE_ID, 2) },
	{ WTimeSpan(0, -4, 0, 0),	WTimeSpan(0),			"AST",	"Atlantic Standard Time",			MAKE_ID(STD_TIMEZONE_ID, 3) },
	{ WTimeSpan(0, 8, 0, 0),	WTimeSpan(0),			"AWST", "Australian Western Standard Time", MAKE_ID(STD_TIMEZONE_ID, 4) },
	{ WTimeSpan(0, 1, 0, 0),	WTimeSpan(0),			"CET",	"Central European Time",			MAKE_ID(STD_TIMEZONE_ID, 5) },	// 5
	{ WTimeSpan(0, -6, 0, 0),	WTimeSpan(0),			"CST",	"Central Standard Time",			MAKE_ID(STD_TIMEZONE_ID, 6) },
	{ WTimeSpan(0, 7, 0, 0),	WTimeSpan(0),			"CXT",	"Christmas Island Time",			MAKE_ID(STD_TIMEZONE_ID, 7) },
	{ WTimeSpan(0, 2, 0, 0),	WTimeSpan(0),			"EET",	"Eastern European Time",			MAKE_ID(STD_TIMEZONE_ID, 8) },
	{ WTimeSpan(0, -5, 0, 0),	WTimeSpan(0),			"EST",	"Eastern Standard Time",			MAKE_ID(STD_TIMEZONE_ID, 9) },
	{ WTimeSpan(0, -10, 0, 0),	WTimeSpan(0),			"HAST",	"Hawaii-Aleutian Standard Time",	MAKE_ID(STD_TIMEZONE_ID, 10) },	// 10
	{ WTimeSpan(0, 3, 0, 0),	WTimeSpan(0),			"MSK",	"Moscow Standard Time",				MAKE_ID(STD_TIMEZONE_ID, 11) },
	{ WTimeSpan(0, -7, 0, 0),	WTimeSpan(0),			"MST",	"Mountain Standard Time",			MAKE_ID(STD_TIMEZONE_ID, 12) },
	{ WTimeSpan(0, 11, 30, 0),	WTimeSpan(0),			"NFT",	"Norfolk (Island) Time",			MAKE_ID(STD_TIMEZONE_ID, 13) },
	{ WTimeSpan(0, -3, -30, 0),	WTimeSpan(0),			"NST",	"Newfoundland Standard Time",		MAKE_ID(STD_TIMEZONE_ID, 14) },
	{ WTimeSpan(0, 12, 0, 0),	WTimeSpan(0),			"NZST",	"New Zealand Standard Time",		MAKE_ID(STD_TIMEZONE_ID, 15) },	// 15
	{ WTimeSpan(0, -8, 0, 0),	WTimeSpan(0),			"PST",	"Pacific Standard Time",			MAKE_ID(STD_TIMEZONE_ID, 16) },
	{ WTimeSpan(0, 0, 0, 0),	WTimeSpan(0),			"UTC",	"Universal Coordinated Time",		MAKE_ID(STD_TIMEZONE_ID, 17) },
	{ WTimeSpan(0, 2, 0, 0),	WTimeSpan(0),			"RZ1",	"Russian Zone 1",					MAKE_ID(STD_TIMEZONE_ID, 18) },
	{ WTimeSpan(0, 3, 0, 0),	WTimeSpan(0),			"RZ2",	"Russian Zone 2",					MAKE_ID(STD_TIMEZONE_ID, 19) },
	{ WTimeSpan(0, 4, 0, 0),	WTimeSpan(0),			"RZ3",	"Russian Zone 3",					MAKE_ID(STD_TIMEZONE_ID, 20) },	// 20
	{ WTimeSpan(0, -1, 0, 0),	WTimeSpan(0),			"WAT",	"West African Time",				MAKE_ID(STD_TIMEZONE_ID, 21) },
	{ WTimeSpan(0, -2, 0, 0),	WTimeSpan(0),			"AT",	"Azores Time",						MAKE_ID(STD_TIMEZONE_ID, 22) },
	{ WTimeSpan(0, -11, 0, 0),	WTimeSpan(0),			"NT",	"Nome Time",						MAKE_ID(STD_TIMEZONE_ID, 23) },
	{ WTimeSpan(0, 5, 30, 0),	WTimeSpan(0),			"IST",	"Indian Standard Time",				MAKE_ID(STD_TIMEZONE_ID, 24) },
	{ WTimeSpan(0, 8, 0, 0),	WTimeSpan(0),			"CCT",	"China Coast Time",					MAKE_ID(STD_TIMEZONE_ID, 25) },	// 25
	{ WTimeSpan(0, 9, 0, 0),	WTimeSpan(0),			"JST",	"Japan Standard Time",				MAKE_ID(STD_TIMEZONE_ID, 26) },
	{ WTimeSpan(0, 10, 0, 0),	WTimeSpan(0),			"GST",	"Guam Standard Time",				MAKE_ID(STD_TIMEZONE_ID, 27) },
	{ WTimeSpan(0, 0, 0, 0),	WTimeSpan(0),			NULL,	NULL,								0 }
};


const ::TimeZoneInfo WorldLocation::m_dst_timezones[] = {
	{ WTimeSpan(0, 9, 30, 0),	WTimeSpan(0, 1, 0, 0),	"ACDT",	"Australian Central Daylight Time", MAKE_ID(DST_TIMEZONE_ID, 0) },
	{ WTimeSpan(0, -4, 0, 0),	WTimeSpan(0, 1, 0, 0),	"ADT",	"Atlantic Daylight Time",			MAKE_ID(DST_TIMEZONE_ID, 1) },
	{ WTimeSpan(0, 10, 0, 0),	WTimeSpan(0, 1, 0, 0),	"AEDT",	"Australian Eastern Daylight Time", MAKE_ID(DST_TIMEZONE_ID, 2) },
	{ WTimeSpan(0, -9, 0, 0),	WTimeSpan(0, 1, 0, 0),	"AKDT",	"Alaska Daylight Time",				MAKE_ID(DST_TIMEZONE_ID, 3) },
	{ WTimeSpan(0, 8, 0, 0),	WTimeSpan(0, 1, 0, 0), 	"AWDT", "Australian Western Daylight Time", MAKE_ID(DST_TIMEZONE_ID, 4) },
	{ WTimeSpan(0, 0, 0, 0),	WTimeSpan(0, 1, 0, 0),	"BST",	"British Summer Time",				MAKE_ID(DST_TIMEZONE_ID, 5) },	// 5
	{ WTimeSpan(0, -6, 0, 0),	WTimeSpan(0, 1, 0, 0),	"CDT",	"Central Daylight Time",			MAKE_ID(DST_TIMEZONE_ID, 6) },
	{ WTimeSpan(0, 1, 0, 0),	WTimeSpan(0, 1, 0, 0),	"CEDT",	"Central European Daylight Time",	MAKE_ID(DST_TIMEZONE_ID, 7) },
	{ WTimeSpan(0, -5, 0, 0),	WTimeSpan(0, 1, 0, 0),	"EDT",	"Eastern Daylight Time",			MAKE_ID(DST_TIMEZONE_ID, 8) },
	{ WTimeSpan(0, 2, 0, 0),	WTimeSpan(0, 1, 0, 0),	"EEDT",	"Eastern European Daylight Time",	MAKE_ID(DST_TIMEZONE_ID, 9) },
	{ WTimeSpan(0, -10, 0, 0),	WTimeSpan(0, 1, 0, 0),	"HADT",	"Hawaii-Aleutian Daylight Time",	MAKE_ID(DST_TIMEZONE_ID, 10) },	// 10
	{ WTimeSpan(0, 0, 0, 0),	WTimeSpan(0, 1, 0, 0),	"IST",	"Irish Summer Time",				MAKE_ID(DST_TIMEZONE_ID, 11) },
	{ WTimeSpan(0, -7, 0, 0),	WTimeSpan(0, 1, 0, 0),	"MDT",	"Mountain Daylight Time",			MAKE_ID(DST_TIMEZONE_ID, 12) },
	{ WTimeSpan(0, 3, 0, 0),	WTimeSpan(0, 1, 0, 0),	"MSD",	"Moscow Daylight Time",				MAKE_ID(DST_TIMEZONE_ID, 13) },
	{ WTimeSpan(0, -3, -30, 0),	WTimeSpan(0, 1, 0, 0),	"NDT",	"Newfoundland Daylight Time",		MAKE_ID(DST_TIMEZONE_ID, 14) },
	{ WTimeSpan(0, 12, 0, 0),	WTimeSpan(0, 1, 0, 0),	"NZDT",	"New Zealand Daylight Time",		MAKE_ID(DST_TIMEZONE_ID, 15) },	// 15
	{ WTimeSpan(0, -8, 0, 0),	WTimeSpan(0, 1, 0, 0),	"PDT",	"Pacific Daylight Time",			MAKE_ID(DST_TIMEZONE_ID, 16) },
	{ WTimeSpan(0, 0, 0, 0),	WTimeSpan(0, 1, 0, 0),	"WEDT",	"Western European Daylight Time",	MAKE_ID(DST_TIMEZONE_ID, 17) },
	{ WTimeSpan(0, 0, 0, 0),	WTimeSpan(0),			NULL,	NULL,								0 }
};


const ::TimeZoneInfo WorldLocation::m_mil_timezones[] = {
	{ WTimeSpan(0, 0, 0, 0),	WTimeSpan(0),		"Z",	"Zulu Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 0) },
	{ WTimeSpan(0, 1, 0, 0),	WTimeSpan(0),		"A",	"Alpha Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 1) },
	{ WTimeSpan(0, 2, 0, 0),	WTimeSpan(0),		"B",	"Bravo Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 2) },
	{ WTimeSpan(0, 3, 0, 0),	WTimeSpan(0),		"C",	"Charlie Time Zone",	MAKE_ID(MIL_TIMEZONE_ID, 3) },
	{ WTimeSpan(0, 4, 0, 0),	WTimeSpan(0),		"D",	"Delta Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 4) },
	{ WTimeSpan(0, 5, 0, 0),	WTimeSpan(0),		"E",	"Echo Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 5) },	// 5
	{ WTimeSpan(0, 6, 0, 0),	WTimeSpan(0),		"F",	"Foxtrot Time Zone",	MAKE_ID(MIL_TIMEZONE_ID, 6) },
	{ WTimeSpan(0, 7, 0, 0),	WTimeSpan(0),		"G",	"Golf Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 7) },
	{ WTimeSpan(0, 8, 0, 0),	WTimeSpan(0),		"H",	"Hotel Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 8) },
	{ WTimeSpan(0, 9, 0, 0),	WTimeSpan(0),		"I",	"India Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 9) },
	{ WTimeSpan(0, 10, 0, 0),	WTimeSpan(0),		"K",	"Kilo Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 10) },	// 10
	{ WTimeSpan(0, 11, 0, 0),	WTimeSpan(0),		"L",	"Lima Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 11) },
	{ WTimeSpan(0, 12, 0, 0),	WTimeSpan(0),		"M",	"Mike Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 12) },
	{ WTimeSpan(0, -1, 0, 0),	WTimeSpan(0),		"N",	"November Time Zone",	MAKE_ID(MIL_TIMEZONE_ID, 13) },
	{ WTimeSpan(0, -2, 0, 0),	WTimeSpan(0),		"O",	"Oscar Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 14) },
	{ WTimeSpan(0, -3, 0, 0),	WTimeSpan(0),		"P",	"Papa Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 15) },	// 15
	{ WTimeSpan(0, -4, 0, 0),	WTimeSpan(0),		"Q",	"Quebec Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 16) },
	{ WTimeSpan(0, -5, 0, 0),	WTimeSpan(0),		"R",	"Romeo Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 17) },
	{ WTimeSpan(0, -6, 0, 0),	WTimeSpan(0),		"S",	"Sierra Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 18) },
	{ WTimeSpan(0, -7, 0, 0),	WTimeSpan(0),		"T",	"Tango Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 19) },
	{ WTimeSpan(0, -8, 0, 0),	WTimeSpan(0),		"U",	"Uniform Time Zone",	MAKE_ID(MIL_TIMEZONE_ID, 20) }, // 20
	{ WTimeSpan(0, -9, 0, 0),	WTimeSpan(0),		"V",	"Vector Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 21) },
	{ WTimeSpan(0, -10, 0, 0),	WTimeSpan(0),		"W",	"Whiskey Time Zone",	MAKE_ID(MIL_TIMEZONE_ID, 22) },
	{ WTimeSpan(0, -11, 0, 0),	WTimeSpan(0),		"X",	"X-ray Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 23) },
	{ WTimeSpan(0, -12, 0, 0),	WTimeSpan(0),		"Y",	"Yankee Time Zone",		MAKE_ID(MIL_TIMEZONE_ID, 24) },
	{ WTimeSpan(0),				WTimeSpan(0),		NULL,	NULL,					0 }
};


bool WorldLocation::InsideCanada() const {
	return ((WorldLocation *)this)->InsideCanada(_latitude, _longitude);
}


bool WorldLocation::InsideCanada(const double latitude, const double longitude) const {
	if (latitude < DEGREE_TO_RADIAN(41.0))		return false;
	if (latitude > DEGREE_TO_RADIAN(83.0))		return false;
	if (longitude < DEGREE_TO_RADIAN(-141.0))	return false;
	if (longitude > DEGREE_TO_RADIAN(-52.0))	return false;

#if defined(_MSC_VER) || defined(__GEOGRAPHY_BORDERS_H)
	XY_Point pt(RADIAN_TO_DEGREE(longitude), RADIAN_TO_DEGREE(latitude));
	Borders borders;
	if (borders.Canada())
		return borders.Canada()->PointInArea(pt, 0.000001);
	return false;
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


bool WorldLocation::InsideAlaska() const {
	return ((WorldLocation*)this)->InsideAlaska(_latitude, _longitude);
}


bool WorldLocation::InsideAlaska(const double latitude, const double longitude) const {
	if (latitude < DEGREE_TO_RADIAN(51.219861999999999))		return false;
	if (latitude > DEGREE_TO_RADIAN(71.352560999999994))		return false;

	double __longitude = longitude;
	if (__longitude > 0.0)
		__longitude -= TwoPi;
	if (__longitude < DEGREE_TO_RADIAN(-187.53833299999999))	return false;
	if (__longitude > DEGREE_TO_RADIAN(-129.97416699999999))	return false;

#if defined(_MSC_VER) || defined(__GEOGRAPHY_BORDERS_H)
	XY_Point pt(RADIAN_TO_DEGREE(longitude), RADIAN_TO_DEGREE(latitude));
	Borders borders;
	if (borders.AK())
		return borders.AK()->PointInArea(pt, 0.000001);
	return false;
#else
	return true;
#endif
}


bool WorldLocation::InsideWashington() const {
	return ((WorldLocation*)this)->InsideWashington(_latitude, _longitude);
}


bool WorldLocation::InsideWashington(const double latitude, const double longitude) const {
	if (latitude < DEGREE_TO_RADIAN(45.543540999999998))		return false; 
	if (latitude > DEGREE_TO_RADIAN(49.002493999999999))		return false; 
	if (longitude < DEGREE_TO_RADIAN(-124.73317400000001))	return false; 
	if (longitude > DEGREE_TO_RADIAN(-116.91598900000000))		return false;

#if defined(_MSC_VER) || defined(__GEOGRAPHY_BORDERS_H)
	XY_Point pt(RADIAN_TO_DEGREE(longitude), RADIAN_TO_DEGREE(latitude));
	Borders borders;
	if (borders.WA())
		return borders.WA()->PointInArea(pt, 0.000001);
	return false;
#else
	return true;
#endif
}


bool WorldLocation::InsideOregon() const {
	return ((WorldLocation*)this)->InsideOregon(_latitude, _longitude);
}


bool WorldLocation::InsideOregon(const double latitude, const double longitude) const {
	if (latitude < DEGREE_TO_RADIAN(41.991793999999999))	return false;
	if (latitude > DEGREE_TO_RADIAN(46.271003999999998))	return false;
	if (longitude < DEGREE_TO_RADIAN(-124.55244100000000))	return false;
	if (longitude > DEGREE_TO_RADIAN(-116.46350400000000))	return false;

#if defined(_MSC_VER) || defined(__GEOGRAPHY_BORDERS_H)
	XY_Point pt(RADIAN_TO_DEGREE(longitude), RADIAN_TO_DEGREE(latitude));
	Borders borders;
	if (borders.OR())
		return borders.OR()->PointInArea(pt, 0.000001);
	return false;
#else
	return true;
#endif
}


bool WorldLocation::InsideIdaho() const {
	return ((WorldLocation*)this)->InsideIdaho(_latitude, _longitude);
}


bool WorldLocation::InsideIdaho(const double latitude, const double longitude) const {
	if (latitude < DEGREE_TO_RADIAN(41.988056999999998))		return false;
	if (latitude > DEGREE_TO_RADIAN(49.001145999999999))		return false;
	if (longitude < DEGREE_TO_RADIAN(-117.24302700000000))		return false;
	if (longitude > DEGREE_TO_RADIAN(-111.04356400000000))		return false;

#if defined(_MSC_VER) || defined(__GEOGRAPHY_BORDERS_H)
	XY_Point pt(RADIAN_TO_DEGREE(longitude), RADIAN_TO_DEGREE(latitude));
	Borders borders;
	if (borders.ID())
		return borders.ID()->PointInArea(pt, 0.000001);
	return false;
#else
	return true;
#endif
}


bool WorldLocation::InsideMontana() const {
	return ((WorldLocation*)this)->InsideMontana(_latitude, _longitude);
}


bool WorldLocation::InsideMontana(const double latitude, const double longitude) const {
	if (latitude < DEGREE_TO_RADIAN(44.357990999999998))	return false;
	if (latitude > DEGREE_TO_RADIAN(49.0))					return false;
	if (longitude < DEGREE_TO_RADIAN(-116.04989399999999))	return false;
	if (longitude > DEGREE_TO_RADIAN(-104.03913799999999))	return false;

#if defined(_MSC_VER) || defined(__GEOGRAPHY_BORDERS_H)
	XY_Point pt(RADIAN_TO_DEGREE(longitude), RADIAN_TO_DEGREE(latitude));
	Borders borders;
	if (borders.MT())
		return borders.MT()->PointInArea(pt, 0.000001);
	return false;
#else
	return true;
#endif
}


bool WorldLocation::InsideNorthDakota() const {
	return ((WorldLocation*)this)->InsideNorthDakota(_latitude, _longitude);
}


bool WorldLocation::InsideNorthDakota(const double latitude, const double longitude) const {
	if (latitude < DEGREE_TO_RADIAN(45.935054000000001))	return false;
	if (latitude > DEGREE_TO_RADIAN(49.0))					return false;
	if (longitude < DEGREE_TO_RADIAN(-104.04890000000000))	return false;
	if (longitude > DEGREE_TO_RADIAN(-96.554507000000001))	return false;

#if defined(_MSC_VER) || defined(__GEOGRAPHY_BORDERS_H)
	XY_Point pt(RADIAN_TO_DEGREE(longitude), RADIAN_TO_DEGREE(latitude));
	Borders borders;
	if (borders.ND())
		return borders.ND()->PointInArea(pt, 0.000001);
	return false;
#else
	return true;
#endif
}


bool WorldLocation::InsideMinnesota() const {
	return ((WorldLocation*)this)->InsideMinnesota(_latitude, _longitude);
}


bool WorldLocation::InsideMinnesota(const double latitude, const double longitude) const {
	if (latitude < DEGREE_TO_RADIAN(43.499355999999999))	return false;
	if (latitude > DEGREE_TO_RADIAN(49.384357999999999))	return false;
	if (longitude < DEGREE_TO_RADIAN(-97.239209000000002))	return false;
	if (longitude > DEGREE_TO_RADIAN(-89.489226000000002))	return false;

#if defined(_MSC_VER) || defined(__GEOGRAPHY_BORDERS_H)
	XY_Point pt(RADIAN_TO_DEGREE(longitude), RADIAN_TO_DEGREE(latitude));
	Borders borders;
	if (borders.MN())
		return borders.MN()->PointInArea(pt, 0.000001);
	return false;
#else
	return true;
#endif
}


bool WorldLocation::InsideYukon() const {
	return ((WorldLocation*)this)->InsideYukon(_latitude, _longitude);
}


bool WorldLocation::InsideYukon(const double latitude, const double longitude) const {
	if (latitude < DEGREE_TO_RADIAN(60.0))	return false;
	if (latitude > DEGREE_TO_RADIAN(69.647455275000098))	return false;
	if (_longitude < DEGREE_TO_RADIAN(-141.01807315799999))	return false;
	if (_longitude > DEGREE_TO_RADIAN(-123.78932483600001))	return false;

#if defined(_MSC_VER) || defined(__GEOGRAPHY_BORDERS_H)
	XY_Point pt(RADIAN_TO_DEGREE(longitude), RADIAN_TO_DEGREE(latitude));
	Borders borders;
	if (borders.YT())
		return borders.YT()->PointInArea(pt, 0.000001);
	return false;
#else
	return true;
#endif
}


bool WorldLocation::InsideAlberta() const {
	return ((WorldLocation*)this)->InsideAlberta(_latitude, _longitude);
}


bool WorldLocation::InsideAlberta(const double latitude, const double longitude) const {
	if (latitude < DEGREE_TO_RADIAN(49.0))		return false;
	if (latitude > DEGREE_TO_RADIAN(60.0))		return false;
	if (longitude < DEGREE_TO_RADIAN(-120.0))	return false;
	if (longitude > DEGREE_TO_RADIAN(-110.0))	return false;

#if defined(_MSC_VER) || defined(__GEOGRAPHY_BORDERS_H)
	XY_Point pt(RADIAN_TO_DEGREE(longitude), RADIAN_TO_DEGREE(latitude));
	Borders borders;
	if (borders.AB())
		return borders.AB()->PointInArea(pt, 0.000001);
	return false;
#else
	return true;
#endif
}


bool WorldLocation::InsideSaskatchewan() const {
	return ((WorldLocation*)this)->InsideSaskatchewan(_latitude, _longitude);
}


bool WorldLocation::InsideSaskatchewan(const double latitude, const double longitude) const {
	if (latitude < DEGREE_TO_RADIAN(49.0))					return false;
	if (latitude > DEGREE_TO_RADIAN(60.0))					return false;
	if (longitude < DEGREE_TO_RADIAN(-110.0))				return false;
	if (longitude > DEGREE_TO_RADIAN(-101.36230470900000))	return false;

#if defined(_MSC_VER) || defined(__GEOGRAPHY_BORDERS_H)
	XY_Point pt(RADIAN_TO_DEGREE(longitude), RADIAN_TO_DEGREE(latitude));
	Borders borders;
	if (borders.SK())
		return borders.SK()->PointInArea(pt, 0.000001);
	return false;
#else
	return true;
#endif
}


bool WorldLocation::InsideManitoba() const {
	return ((WorldLocation*)this)->InsideManitoba(_latitude, _longitude);
}


bool WorldLocation::InsideManitoba(const double latitude, const double longitude) const {
	if (latitude < DEGREE_TO_RADIAN(49.0))						return false;
	if (latitude > DEGREE_TO_RADIAN(60.0))						return false;
	if (longitude < DEGREE_TO_RADIAN(-102.0))					return false;
	if (longitude > DEGREE_TO_RADIAN(-88.986493339999896))		return false; 

#if defined(_MSC_VER) || defined(__GEOGRAPHY_BORDERS_H)
	XY_Point pt(RADIAN_TO_DEGREE(longitude), RADIAN_TO_DEGREE(latitude));
	Borders borders;
	if (borders.MB())
		return borders.MB()->PointInArea(pt, 0.000001);
	return false;
#else
	return true;
#endif
}


bool WorldLocation::InsideNewBrunswick() const {
	return ((WorldLocation*)this)->InsideNewBrunswick(_latitude, _longitude);
}


bool WorldLocation::InsideNewBrunswick(const double latitude, const double longitude) const {
	if (latitude < DEGREE_TO_RADIAN(44.570312889000000))	return false;
	if (latitude > DEGREE_TO_RADIAN(48.089339133000102))	return false;
	if (longitude < DEGREE_TO_RADIAN(-69.053278519999907))	return false;
	if (longitude > DEGREE_TO_RADIAN(-63.772017501999898))	return false;

#if defined(_MSC_VER) || defined(__GEOGRAPHY_BORDERS_H)
	XY_Point pt(RADIAN_TO_DEGREE(longitude), RADIAN_TO_DEGREE(latitude));
	Borders borders;
	if (borders.NB())
		return borders.NB()->PointInArea(pt, 0.000001);
	return false;
#else
	return true;
#endif
}


bool WorldLocation::InsidePEI() const {
	return ((WorldLocation*)this)->InsidePEI(_latitude, _longitude);
}


bool WorldLocation::InsidePEI(const double latitude, const double longitude) const {
	if (latitude < DEGREE_TO_RADIAN(45.948259503000102))		return false;
	if (latitude > DEGREE_TO_RADIAN(47.061222030000103))		return false;
	if (longitude < DEGREE_TO_RADIAN(-64.413458053999904))		return false;
	if (longitude > DEGREE_TO_RADIAN(-61.970709676999903))		return false;
	
#if defined(_MSC_VER) || defined(__GEOGRAPHY_BORDERS_H)
	XY_Point pt(RADIAN_TO_DEGREE(longitude), RADIAN_TO_DEGREE(latitude));
	Borders borders;
	if (borders.PEI())
		return borders.PEI()->PointInArea(pt, 0.000001);
	return false;
#else
	return true;
#endif
}


bool WorldLocation::InsideNewZealand(const double latitude, const double longitude) const {

#if defined(_MSC_VER) || defined(__GEOGRAPHY_BORDERS_H)
	XY_Point pt(RADIAN_TO_DEGREE(longitude), RADIAN_TO_DEGREE(latitude));
	Borders borders;
	if (borders.NewZealand())
		return borders.NewZealand()->PointInArea(pt, 0.000001);
	return false;
#else
	if ((longitude > DEGREE_TO_RADIAN(172.5)) && (longitude < DEGREE_TO_RADIAN(178.6))) {
		if ((latitude > DEGREE_TO_RADIAN(-41.75)) && (latitude < DEGREE_TO_RADIAN(-34.3))) {	// general extents of New Zealand's north island
			return true;
		}
	}

	if ((longitude > DEGREE_TO_RADIAN(166.3)) && (longitude < DEGREE_TO_RADIAN(174.5))) {
		if ((latitude > DEGREE_TO_RADIAN(-47.35)) && (latitude < DEGREE_TO_RADIAN(-40.4))) {	// general extents of New Zealand's south island
			return true;
		}
	}
	return false;
#endif
}


bool WorldLocation::InsideNewZealand() const {
	return InsideNewZealand(_latitude, _longitude);
}


bool WorldLocation::InsideTasmania(const double latitude, const double longitude) const {
	if ((_longitude > DEGREE_TO_RADIAN(143.5)) && (_longitude < DEGREE_TO_RADIAN(149.0))) {
		if ((_latitude > DEGREE_TO_RADIAN(-44.0)) && (_latitude < DEGREE_TO_RADIAN(-39.5))) {	// general extents of Tasmania
			return true;
		}
	}

#if defined(_MSC_VER) || defined(__GEOGRAPHY_BORDERS_H)
	XY_Point pt(RADIAN_TO_DEGREE(longitude), RADIAN_TO_DEGREE(latitude));
	Borders borders;
	if (borders.Tasmania())
		return borders.Tasmania()->PointInArea(pt, 0.000001);
	return false;
#else
	return false;
#endif
}


bool WorldLocation::InsideTasmania() const {
	return InsideNewZealand(_latitude, _longitude);
}


bool WorldLocation::InsideAustraliaMainland() const {
	if ((_longitude > DEGREE_TO_RADIAN(113.15)) && (_longitude < DEGREE_TO_RADIAN(153.633333))) {
		if ((_latitude > DEGREE_TO_RADIAN(-39.133333)) && (_latitude < DEGREE_TO_RADIAN(-10.683333))) {	// general extents of continental Australia
			return true;
		}
	}

	return false;
}


void WorldLocation::SetTimeZoneOffset(const TimeZoneInfo* timezone)
{
	if (timezone) {
		_amtDST = timezone->m_dst;
		__timezone = timezone->m_timezone;
		if (_startDST.GetTotalMicroSeconds() != 0)
			_startDST = WTimeSpan(0);
		if (_amtDST.GetTotalMicroSeconds() != 0)
			_endDST = WTimeSpan(366, 0, 0, 0);
	}
	_timezoneInfo = timezone;
}


bool WorldLocation::SetTimeZoneOffset(std::uint32_t id)
{
	auto tz = TimeZoneFromId(id);
	if (tz)
	{
		SetTimeZoneOffset(tz);
		return true;
	}
	return false;
}


bool WorldLocation::UpdateTimeZone() {
	if (_timezoneInfo) {
		if (strchr(_timezoneInfo->m_name, '/'))
			return true;
	}
	bool valid;
	const TimeZoneInfo* tzi = WorldLocation::TimeZoneFromLatLon(_latitude, _longitude, dstExists(), &valid);
	if ((tzi->m_timezone == m_timezone()) && (tzi->m_dst == m_amtDST())) {
		_timezoneInfo = tzi;
		return true;
	}
	return false;
}


const TimeZoneInfo* WorldLocation::DowngradeTimeZone() const {
	if (!_timezoneInfo) {
		const TimeZoneInfo* tzi = GuessTimeZone(dstExists());
		return tzi;
	}
	if (_timezoneInfo->m_id & 0x70000)
		return _timezoneInfo;
	const TimeZoneInfo* tz;
	if (dstExists())
		tz = m_dst_timezones;
	else
		tz = m_std_timezones;
	while (tz->m_name) {
		if (tz->m_timezone == _timezoneInfo->m_timezone)
			return tz;
		tz++;
	}
	return nullptr;
}


const ::TimeZoneInfo *WorldLocation::GuessTimeZone(INTNM::int16_t set) const {
	bool valid;
	const TimeZoneInfo *tzi = WorldLocation::TimeZoneFromLatLon(_latitude, _longitude, set, &valid);
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

		double longitude = _longitude;
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


const ::TimeZoneInfo *WorldLocation::CurrentTimeZone(INTNM::int16_t set, bool* hidden) const
{
	if (_timezoneInfo != nullptr)
	{
		if (hidden)
		{
			if (IS_STD(_timezoneInfo->m_id))
			{
				*hidden = true;
				const ::TimeZoneInfo* tz = m_std_timezones;
				while (tz->m_code)
				{
					if (tz->m_id == _timezoneInfo->m_id)
					{
						*hidden = false;
						break;
					}
					tz++;
				}
			}
			else if (IS_DST(_timezoneInfo->m_id))
			{
				*hidden = true;
				const ::TimeZoneInfo* tz = m_dst_timezones;
				while (tz->m_code)
				{
					if (tz->m_id == _timezoneInfo->m_id)
					{
						*hidden = false;
						break;
					}
					tz++;
				}
			}
		}
		return _timezoneInfo;
	}
	else
	{
		const ::TimeZoneInfo* tz;
		if (set == -1)
			tz = m_mil_timezones;
		else if (_startDST == _endDST)
		{
			tz = m_std_timezones;
		}
		else
		{
			tz = m_dst_timezones;
		}

		while (tz->m_code)
		{
			if (tz->m_timezone == __timezone)
				break;
			tz++;
		}
		if (tz->m_code)
		{
			if (hidden)
				*hidden = false;
			return tz;
		}
		return nullptr;
	}
}


const ::TimeZoneInfo* WorldLocation::TimeZoneFromName(const std::string& name, INTNM::int16_t set, bool* hidden)
{
	if (name.find_first_of('/') == -1) {
		if (set != -2) {
			const ::TimeZoneInfo* tz;
			if (set == -1)
				tz = m_mil_timezones;
			else if (set)
			{
				tz = m_dst_timezones;
			}
			else
			{
				tz = m_std_timezones;
			}

			while (tz->m_code)
			{
				if (boost::iequals(tz->m_code, name) || boost::iequals(tz->m_name, name))
					break;
				tz++;
			}
			if (tz->m_code)
			{
				if (hidden)
					*hidden = false;
				return tz;
			}
		}
	}
	const ::TimeZoneInfo* tz;
	tz = TimezoneMapper::fromName(name.c_str(), set);
	return tz;
}


const ::TimeZoneInfo* WorldLocation::TimeZoneFromId(std::uint32_t id, bool* hidden)
{
	if (IS_STD(id))
	{
		const TimeZoneInfo* tz = m_std_timezones;
		while (tz->m_name)
		{
			if (tz->m_id == id)
			{
				if (hidden)
					*hidden = false;
				return tz;
			}
			tz++;
		}
	}
	else if (IS_DST(id))
	{
		const TimeZoneInfo* tz = m_dst_timezones;
		while (tz->m_name)
		{
			if (tz->m_id == id)
			{
				if (hidden)
					*hidden = false;
				return tz;
			}
			tz++;
		}
	}
	else
	{
		const TimeZoneInfo* tz = m_mil_timezones;
		while (tz->m_name)
		{
			if (tz->m_id == id)
			{
				if (hidden)
					*hidden = false;
				return tz;
			}
			tz++;
		}
	}
	return TimezoneMapper::fromId(id, -2);
}


const TimeZoneInfo* WorldLocation::GetDaylightSavingsTimeZone(const TimeZoneInfo* info)
{
	if (info->m_dst.GetTotalMilliSeconds())
		return info;
	info = TimezoneMapper::fromName(info->m_name, 1);
	return info;
}


const TimeZoneInfo* WorldLocation::GetStandardTimeZone(const TimeZoneInfo* info)
{
	if (!info->m_dst.GetTotalMilliSeconds())
		return info;
	info = TimezoneMapper::fromName(info->m_name, 0);
	return info;
}


WorldLocation::WorldLocation()
	: _timezoneInfo(nullptr)
#ifdef HSS_USE_CACHING
	, m_sunCache(4), m_solarCache(4)
#endif
{
	_latitude = 1000.0;
	_longitude = 1000.0;
	__timezone = WTimeSpan(0);
	_startDST = WTimeSpan(0);
	_endDST = WTimeSpan(0);
	_amtDST = WTimeSpan(0, 1, 0, 0);
}


WorldLocation::WorldLocation(const WorldLocation &wl)
	: _timezoneInfo(nullptr)
#ifdef HSS_USE_CACHING
	, m_sunCache(4), m_solarCache(4)
#endif
{
	*this = wl;
}


WorldLocation::WorldLocation(double latitude, double longitude, bool guessTimezone)
	: _timezoneInfo(nullptr)
#ifdef HSS_USE_CACHING
	, m_sunCache(4), m_solarCache(4)
#endif
{
	_latitude = DEGREE_TO_RADIAN(latitude);
	_longitude = DEGREE_TO_RADIAN(longitude);
	if (guessTimezone)
	{
		_timezoneInfo = GuessTimeZone(0);
		__timezone = _timezoneInfo->m_timezone;
		if (_timezoneInfo->m_dst.GetTotalSeconds() > 0)
		{
			_startDST = WTimeSpan(0);
			_endDST = WTimeSpan(366, 0, 0, 0);
			_amtDST = _timezoneInfo->m_dst;
		}
	}
}


WorldLocation::~WorldLocation() {
}


WorldLocation &WorldLocation::operator=(const WorldLocation &wl) {
	if (&wl != this) {
		_latitude = wl._latitude;
		_longitude = wl._longitude;
		__timezone = wl.__timezone;
		_startDST = wl._startDST;
		_endDST = wl._endDST;
		_amtDST = wl._amtDST;

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
	if ((_latitude == wl._latitude) &&
	    (_longitude == wl._longitude) &&
	    (__timezone == wl.__timezone) &&
	    (_startDST == wl._startDST) &&
	    (_endDST == wl._endDST) &&
	    (_amtDST == wl._amtDST))
		return true;
	return false;
}


bool WorldLocation::operator!=(const WorldLocation &wl) const {
	return !(*this == wl);
}


WTimeSpan WorldLocation::m_solar_timezone(const WTime &solar_time) const {
#ifdef HSS_USE_CACHING
	struct sun_key sk;
	sk.m_sun_cache_lat = _latitude;
	sk.m_sun_cache_long = _longitude;
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
	sInput.Latitude = RADIAN_TO_DEGREE(_latitude);
	sInput.Longitude = -RADIAN_TO_DEGREE(_longitude);
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
		if ((loader.svalue[1] < 1) || (loader.svalue[1] > 6))
			AfxThrowArchiveException(CArchiveException::badSchema, _T("World Location"));
		bool found = false;
		if (loader.svalue[1] >= 5) {
			uint32_t id;
			is >> id;
			if (id != (uint32_t)-1) {
				wl.SetTimeZoneOffset(id);
				found = true;
			}
		}
		is >> wl._latitude >> wl._longitude;
		if (!found) {
			if (loader.svalue[1] < 3) {
				INTNM::int32_t timezone;
				is >> timezone;
				wl.__timezone = WTimeSpan(timezone);
			} else	is >> wl.__timezone;
			if (loader.svalue[1] < 4) {
				INTNM::int16_t spheroid;
				is >> spheroid;
			}
			if (loader.svalue[1] == 2) {
				INTNM::int32_t ts;
				is >> ts; wl._startDST = WTimeSpan(ts);
				is >> ts; wl._endDST = WTimeSpan(ts);
				is >> ts; wl._amtDST = WTimeSpan(ts);
			} else if (loader.svalue[1] >= 3)
				is >> wl._startDST >> wl._endDST >> wl._amtDST;

			// try to guess what the timezone ID is
			const ::TimeZoneInfo* tz;
			if ((wl._amtDST.GetTotalSeconds() == 0) && (!wl.dstExists()))	tz = wl.m_std_timezones;
			else															tz = wl.m_dst_timezones;
			while (tz->m_name) {
				if ((tz->m_timezone == wl.__timezone) && (tz->m_dst == wl._amtDST)) {
					wl._timezoneInfo = tz;
					break;
				}
				tz++;
			}
		}
	} else {
		is >> loader.svalue[2] >> loader.svalue[3];
		wl._latitude = loader.dvalue;
		is >> wl._longitude >> wl.__timezone;
	}
	return is;
}


CArchive& HSS_Time::operator<<(CArchive& os, const WorldLocation &wl) {
	short hickup = -1;
	short version = 6;		// 4 removed m_spheroid
							// 5 added id
							// 6 re-added latitude, longitude
	os << hickup << version;
	uint32_t id;
	if (wl.m_timezoneInfo()) {
		id = wl.m_timezoneInfo()->m_id;
		os << id;
		os << wl._latitude << wl._longitude;
	} else {
		id = (uint32_t)-1;
		os << id << wl._latitude << wl._longitude << wl.__timezone;
		os << wl._startDST << wl._endDST << wl._amtDST;
	}
	return os;
}
#endif


INTNM::int16_t WorldLocation::m_sun_rise_set(const WTime &daytime, WTime *Rise, WTime *Set, WTime *Noon) const {

#ifdef HSS_USE_CACHING
	struct sun_key sk;
	sk.m_sun_cache_lat = _latitude;
	sk.m_sun_cache_long = _longitude;
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
	sInput.Latitude = RADIAN_TO_DEGREE(_latitude);
	sInput.Longitude = -RADIAN_TO_DEGREE(_longitude);
	sInput.timezone = 0;
	sInput.DaytimeSaving = false;
	sInput.year = year;
	sInput.month = month;
	sInput.day = day;
	RISESET_OUT_STRUCT sOut;
	INTNM::int16_t success = calculator.calcSun(sInput,&sOut);

	WTime riseTime(0ULL, Rise->GetTimeManager()),
		  setTime(0ULL, Set->GetTimeManager()),
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
	sk.m_sun_cache_lat = _latitude;
	sk.m_sun_cache_long = _longitude;
	sv.m_sun_cache_rise = Rise->GetTotalSeconds();
	sv.m_sun_cache_set = Set->GetTotalSeconds();
	sv.m_sun_cache_noon = Noon->GetTotalSeconds();
	sv.m_success = success;
	m_sunCache.Store(&sk, &sv);
#endif

	return success;
}


INTNM::int16_t WorldLocation::m_sun_rise_set(double latitude, double longitude, const WTime& daytime, WTime* Rise, WTime* Set, WTime* Noon) const {

#ifdef HSS_USE_CACHING
	struct sun_key sk;
	sk.m_sun_cache_lat = latitude;
	sk.m_sun_cache_long = longitude;
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
	sInput.Latitude = RADIAN_TO_DEGREE(latitude);
	sInput.Longitude = -RADIAN_TO_DEGREE(longitude);
	sInput.timezone = 0;
	sInput.DaytimeSaving = false;
	sInput.year = year;
	sInput.month = month;
	sInput.day = day;
	RISESET_OUT_STRUCT sOut;
	INTNM::int16_t success = calculator.calcSun(sInput, &sOut);

	WTime riseTime(0ULL, Rise->GetTimeManager()),
		setTime(0ULL, Set->GetTimeManager()),
		noonTime(0ULL, Noon->GetTimeManager());
	if (!(success & NO_SUNRISE))
		riseTime = WTime(sOut.YearRise, sOut.MonthRise, sOut.DayRise, sOut.HourRise, sOut.MinRise, (INTNM::int32_t)sOut.SecRise, Rise->GetTimeManager());
	*Rise = riseTime;
	if (!(success & NO_SUNSET))
		setTime = WTime(sOut.YearSet, sOut.MonthSet, sOut.DaySet, sOut.HourSet, sOut.MinSet, (INTNM::int32_t)sOut.SecSet, Set->GetTimeManager());
	*Set = setTime;
	noonTime = WTime(sInput.year, sInput.month, sInput.day, sOut.SolarNoonHour, sOut.SolarNoonMin, (INTNM::int32_t)sOut.SolarNoonSec, Noon->GetTimeManager());
	*Noon = noonTime;

#ifdef HSS_USE_CACHING
	sk.m_sun_cache_tm = daytime.GetTime(0);
	sk.m_sun_cache_lat = latitude;
	sk.m_sun_cache_long = longitude;
	sv.m_sun_cache_rise = Rise->GetTotalSeconds();
	sv.m_sun_cache_set = Set->GetTotalSeconds();
	sv.m_sun_cache_noon = Noon->GetTotalSeconds();
	sv.m_success = success;
	m_sunCache.Store(&sk, &sv);
#endif

	return success;
}


WorldLocation WorldLocation::FromLatLon(const double lat, const double lon, INTNM::int16_t set, bool* valid) {
	const ::TimeZoneInfo* info = TimezoneMapper::getTz(RADIAN_TO_DEGREE(lat), RADIAN_TO_DEGREE(lon), set, valid);
	WorldLocation wld;
	wld._amtDST = info->m_dst;
	wld.__timezone = info->m_timezone;
	wld._latitude = lat;
	wld._longitude = lon;
	if (wld._amtDST.GetTotalHours() > 0)
		wld._endDST = WTimeSpan(366, 0, 0, 0);
	return wld;
}


const ::TimeZoneInfo* WorldLocation::TimeZoneFromLatLon(const double lat, const double lon, INTNM::int16_t set, bool* valid) {
	return TimezoneMapper::getTz(RADIAN_TO_DEGREE(lat), RADIAN_TO_DEGREE(lon), set, valid);
}

