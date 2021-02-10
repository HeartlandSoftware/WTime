/**
 * worldlocation.h
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


#pragma once


namespace HSS_Time {
	class WorldLocation;
	class WTimeSpan;
	class WTime;
};

#include "times_internal.h"
#include "Times.h"
#include <string>
#ifdef HSS_USE_CACHING
#include "valuecache_mt.h"
#include "objectcache_mt.h"
#endif

#if defined(TIMES_WINDOWS) && !defined(_NO_MFC)
#include <afx.h>
#endif


class Canada;


namespace HSS_Time {


#ifdef MSVC_COMPILER
#pragma managed(push, off)
#pragma pack(push, 4)
#endif


struct TIMES_API TimeZoneInfo {
	WTimeSpan	m_timezone;
	WTimeSpan	m_dst;
	const char	*m_code;
	const char	*m_name;
};


class TIMES_API WorldLocation {		// this is what we have to define what a plot's location in the world is
    public:
	double		m_latitude;			// stored as radians!!!
	double		m_longitude;
	WTimeSpan	m_timezone;			// time zone
	WTimeSpan	m_startDST,			// when daylight savings turns on - from start of year
			m_endDST,			// when daylight savings turns off - from start of year
							// if you want to disable DST, then just set these to the same value
			m_amtDST;			// amount to adjust for DST

	WTimeSpan	m_solar_timezone(const WTime &solar_time) const;
							// used for GMT->solar time math based on longitude only, but don't assume that solar noon means the sun is right over you!
	INTNM::int16_t m_sun_rise_set(const WTime &local_day, WTime *Rise, WTime *Set, WTime *Noon) const;
							// any time during the local "solar" day will glean the right times - suggestion is to use local noon time
	Canada *canada;

    public:
	WorldLocation();
	WorldLocation(const WorldLocation &wl);
	///<summary>
	/// Construct a new timezone at the given latitude and longitude. Optionally guess the timezone from those coordinates.
	///</summary>
	///<param name="latitude">The latitude in degrees</param>
	///<param name="longitude">The longitude in degrees</param>
	///<param name="guessTimezone">True if the timezone should be guessed from the latitude and longitude</param>
	WorldLocation(double latitude, double longitude, bool guessTimezone);
	~WorldLocation();


				// ***** input/output...
#if defined(TIMES_WINDOWS) && !defined(_NO_MFC)
	friend TIMES_API CArchive& operator>>(CArchive& is, WorldLocation &wl);
	friend TIMES_API CArchive& operator<<(CArchive& os, const WorldLocation &wl);
#endif

	WorldLocation &operator=(const WorldLocation &wl);
	bool operator==(const WorldLocation &wl) const;
	bool operator!=(const WorldLocation &wl) const;

	const struct TimeZoneInfo *GuessTimeZone(INTNM::int16_t set) const;		// 0 for std time zones, 1 for dst time zones, -1 for military time zones
	const struct TimeZoneInfo *CurrentTimeZone(INTNM::int16_t set) const;		// 0 for std time zones, 1 for dst time zones, -1 for military time zones

	bool InsideCanada(const double latitude, const double longitude);
	bool InsideCanada() const;
	bool InsideNewZealand() const;
	bool InsideTasmania() const;
	bool InsideAustraliaMainland() const;

	static const struct TimeZoneInfo m_std_timezones[];
	static const struct TimeZoneInfo m_dst_timezones[];
	static const struct TimeZoneInfo m_mil_timezones[];

public:
	///<summary>
	///Get a WorldLocation object with the timezone data set correctly for the given latitude/longitude. If the area uses
	///daylight savings time at any point in the year it will be enabled.
	///</summary>
	///<param name="latitude">The locations latitude (in radians).</param>
	///<param name="longitude">The locations longitude (in radians).</param>
	///<param name="set">Whether we want back a STD or DST timezone</param>
	///<param name="valid">If supplied, will be set to 1 if a timezone was found for the corresponding location, and 0 otherwise</param>
	static WorldLocation FromLatLon(const double latitude, const double longitude, INTNM::int16_t set, bool* valid = NULL);
	///<summary>
	///Get a TimeZoneInfo object with the timezone data set correctly for the given latitude/longitude. If the area uses
	///daylight savings time at any point in the year it will be enabled.
	///</summary>
	///<param name="latitude">The locations latitude.</param>
	///<param name="longitude">The locations longitude.</param>
	///<param name="set">Whether we want back a STD or DST timezone</param>
	///<param name="valid">If supplied, will be set to 1 if a timezone was found for the corresponding location, and 0 otherwise</param>
	static const TimeZoneInfo* TimeZoneFromLatLon(const double latitude, const double longitude, INTNM::int16_t set, bool* valid = NULL);
	///<summary>
	///Get a TimeZoneInfo object with the timezone data set correctly for the given location name. If the area uses
	///daylight savings time at any point in the year it will be enabled.
	///</summary>
	///<param name="name">The locations name (eg. America/Winnipeg).</param>
	///<param name="set">Whether we want back a STD or DST timezone</param>
	///<param name="valid">If supplied, will be set to 1 if a timezone was found for the corresponding location, and 0 otherwise</param>
	static const TimeZoneInfo* TimeZoneFromLocation(const std::string& name, INTNM::int16_t set, bool* valid = NULL);

	static const TimeZoneInfo* TimeZoneFromName(const std::string& name, INTNM::int16_t set);

    private:
	struct sun_key {
		INTNM::uint64_t	m_sun_cache_tm;
		double		m_sun_cache_lat,
				m_sun_cache_long;
	};
	struct sun_val {
		INTNM::uint64_t	m_sun_cache_rise,
				m_sun_cache_set,
				m_sun_cache_noon;
		INTNM::int32_t		m_success;
	};
#ifdef HSS_USE_CACHING
	mutable ValueCacheTempl_MT<sun_key, sun_val>	m_sunCache;
	mutable ValueCacheTempl_MT<sun_key, WTimeSpan>m_solarCache;
#endif
};


#ifdef MSVC_COMPILER
#pragma pack(pop)
#pragma managed(pop)
#endif

};

