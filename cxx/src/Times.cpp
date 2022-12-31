/**
 * Times.cpp
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


#include "times_internal.h"

#if __has_include(<mathimf.h>)
#include <mathimf.h>
#else
#include <cmath>
#endif
#include "worldlocation.h"
#include "Times.h"
#include "types.h"
#include <stdarg.h>
#include <string.h>
#include <memory>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <stdexcept>
#include <locale>
#include <ctime>
#include <codecvt>
#include <assert.h>
#include <optional>
#include <inttypes.h>
#include <sys/stat.h>

#include <boost/algorithm/string.hpp>

#ifdef __GNUC__
#include <unistd.h>
#include <algorithm>
#include <fstream>
#endif

#ifndef S_ISDIR
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif

#ifndef PATH_MAX
#ifdef MAX_PATH
#define PATH_MAX MAX_PATH
#else
#define PATH_MAX 250
#endif
#endif

#ifdef _MSC_VER
#define realpath(N, R) _fullpath((R), (N), sizeof(R))
#endif


#define WTIME_1900	(9467107200000000LL)
#define WTIME_1970	(11676096000000000LL)
#define WTIME_2000	(12622780800000000LL)
#define WTIME_2010	(12938400000000000LL)


using namespace HSS_Time;
using namespace HSS_Time_Private;



#if defined(_MSC_VER) && _MSC_VER < 1900
INTNM::int32_t c99_vsnprintf(char *outBuf, size_t size, const char *format, va_list ap)
{
	int count = -1;

	if (size != 0)
		count = _vsnprintf_s(outBuf, size, _TRUNCATE, format, ap);
	if (count == -1)
		count = _vscprintf(format, ap);

	return count;
}

INTNM::int32_t c99_snprintf(char *outBuf, size_t size, const char *format, ...)
{
	int count;
	va_list ap;

	va_start(ap, format);
	count = c99_vsnprintf(outBuf, size, format, ap);
	va_end(ap);

	return count;
}
#endif


WTimeSpan::WTimeSpan() {
	m_timeSpan = 0;
}


WTimeSpan::WTimeSpan(INTNM::int64_t time, bool units_are_seconds) {
	if (units_are_seconds)
		m_timeSpan = time * 1000000LL;
	else 
		m_timeSpan = time;
}


WTimeSpan::WTimeSpan(INTNM::int32_t lDays, INTNM::int32_t nHours, INTNM::int32_t nMins, INTNM::int32_t nSecs) {
	m_timeSpan = ((INTNM::int64_t)(nSecs + 60 * (nMins + 60 * (nHours + 24 * lDays)))) * 1000000LL;
}


WTimeSpan::WTimeSpan(INTNM::int32_t lDays, INTNM::int32_t nHours, INTNM::int32_t nMins, const double &nSecs) {
	m_timeSpan = ((INTNM::int64_t)(60 * (nMins + 60 * (nHours + 24 * lDays)))) * 1000000LL + (INTNM::int64_t)(nSecs * 1000000.0);
}


WTimeSpan::WTimeSpan(INTNM::int32_t lDays, INTNM::int32_t nHours, INTNM::int32_t nMins, INTNM::int32_t nSecs, INTNM::int32_t uSecs) {
	m_timeSpan = (INTNM::int64_t)uSecs + ((INTNM::int64_t)(nSecs + 60 * (nMins + 60 * (nHours + 24 * lDays)))) * 1000000LL; 
}


WTimeSpan::WTimeSpan(const WTimeSpan &timeSrc) {
	m_timeSpan = timeSrc.m_timeSpan; 
}

#if defined(TIMES_WINDOWS) && !defined(_NO_MFC)
WTimeSpan::WTimeSpan(const CString &timeSrc, INTNM::int16_t *cnt)
{
	CT2CA pszConvertedAnsiString(timeSrc);
	WTimeSpan w(std::string(pszConvertedAnsiString), cnt);
	*this = w;
}
#endif

WTimeSpan::WTimeSpan(const std::string &timeSrc, INTNM::int16_t *cnt) {
	INTNM::int32_t day = 0, hour = 0, minute = 0, read = 0;
	std::string::size_type idx = 0;
	double second = 0.0;
	bool found = false;

	std::string trimmedSrc = boost::trim_copy(timeSrc);

	if (boost::starts_with(trimmedSrc, "P") || boost::starts_with(trimmedSrc, "-P"))
	{
		bool negative = boost::starts_with(trimmedSrc, "-");

		idx = negative ? 2 : 1;
		std::string value = "";
		bool afterT = false;
		bool isFraction = false;
		if (cnt)
			*cnt = 0;
		while (idx < trimmedSrc.length())
		{
			char c = trimmedSrc[idx];
			if (c == 'T' || c == 't')
				afterT = true;
			else if (c == '.' || c == ',')
			{
				isFraction = true;
				value += ".";
			}
			else if (c >= '0' && c <= '9')
				value += c;
			else if (value.length() > 0)
			{
				double val = -1;
				int ival = -1;
				if (isFraction)
					val = std::stod(value);
				else
					ival = std::stoi(value);

				if (val > 0 || ival > 0)
				{
					switch (c)
					{
					case 'Y':
					case 'y':
						//approximate years to days
						if (isFraction)
							second = val * 31'536'000;
						else
							day = ival * 365;
						if (cnt)
							(*cnt)++;
						break;
					case 'M':
					case 'm':
						if (afterT)
						{
							if (isFraction)
								second = val * 60;
							else
								minute = ival;
							if (cnt)
								(*cnt)++;
						}
						else
						{
							//approximate months to days
							if (isFraction)
								second = val * 2'592'000;
							else
								day = ival * 30;
							if (cnt)
								(*cnt)++;
						}
						break;
					case 'D':
					case 'd':
						if (isFraction)
							second = val * 86'400;
						else
							day = ival;
						if (cnt)
							(*cnt)++;
						break;
					case 'W':
					case 'w':
						if (isFraction)
							second = val * 7 * 86'400;
						else
							day = ival * 7;
						if (cnt)
							(*cnt)++;
						break;
					case 'H':
					case 'h':
						if (isFraction)
							second = val * 3600;
						else
							hour = ival;
						if (cnt)
							(*cnt)++;
						break;
					case 'S':
					case 's':
						if (isFraction)
							second = val;
						else
							second = ival;
						if (cnt)
							(*cnt)++;
						break;
					}
				}
				value.clear();
				//only the smallest value is allowed to be a fraction
				if (isFraction)
					break;
			}
			idx++;
		}
		if (negative)
		{
			if (day > 0)
				day = -day;
			else if (hour > 0)
				hour = -hour;
			else if (minute > 0)
				minute = -minute;
			else
				second = -second;
		}
	}
	else
	{
		if (idx = trimmedSrc.find("day")) {
			if (idx == std::string::npos)
				idx = 0;
			else
				found = true;
			if (found) {
				idx += 3;
				if (trimmedSrc[idx] == 's')
					idx++;
			}
		}
		for (std::string::size_type i = idx; i < trimmedSrc.size(); i++) {
			char c = trimmedSrc[i];
			if ((!isdigit(c)) && (c != ':') && (!isspace(c)) && (c != '-') && (c != '.')) {
				if (cnt)
					*cnt = 0;
				return;
			}
		}

#if defined(TIMES_WINDOWS) && !defined(_NO_MFC)
		if (found) {
			read = sscanf_s(trimmedSrc.c_str(), "%d days %d:%d:%lf", &day, &hour, &minute, &second);
			if (read <= 1)
				read = sscanf_s(trimmedSrc.c_str(), "%d day %d:%d:%lf", &day, &hour, &minute, &second);
		}
		if (read <= 1)
			read = sscanf_s(trimmedSrc.c_str(), "%d:%d:%lf", &hour, &minute, &second);
#else
		if (found) {
			read = sscanf(trimmedSrc.c_str(), "%d days %d:%d:%lf", &day, &hour, &minute, &second);
			if (read <= 1)
				read = sscanf(trimmedSrc.c_str(), "%d day %d:%d:%lf", &day, &hour, &minute, &second);
		}
		if (read <= 1)
			read = sscanf(trimmedSrc.c_str(), "%d:%d:%lf", &hour, &minute, &second);
#endif
		if (cnt)
			*cnt = (short)read;
		if (found) {
			switch (read) {
			case -1: if (cnt) *cnt = 0;	// added case to deal with glitch on scenario edit dialog
			case 0:
			case 1: hour = 0;
			case 2:	minute = 0;
			case 3:	second = 0.0;
			}
		}
		else {
			switch (read) {
			case -1: if (cnt) *cnt = 0;	// added case to deal with glitch on scenario edit dialog
			case 0:	return;			// bad timeSrc string
			case 1:	minute = 0;
			case 2:	second = 0.0;
			}
		}
	}

	if (day < 0)
		hour = 0 - hour;
	if (hour < 0)
		minute = 0 - minute;
	if (minute < 0)
		second = 0 - second;
	
	WTimeSpan cts(day, hour, minute, second);
	*this = cts;
}


#if defined(TIMES_WINDOWS) && !defined(_NO_MFC)
WTimeSpan::WTimeSpan(const COleDateTimeSpan &timeSrc) {
	WTimeSpan cts((INTNM::int32_t)timeSrc.GetTotalDays(), timeSrc.GetHours(), timeSrc.GetMinutes(), (std::int32_t)timeSrc.GetSeconds());
	*this = cts;
}
#endif


void WTimeSpan::SetTotalSeconds(INTNM::int64_t secs)				{ m_timeSpan = secs * 1000000LL; }


INTNM::int64_t WTimeSpan::GetYears() const							{ return (INTNM::int64_t)((long double)m_timeSpan / 1000000.0 / 24.0 / 60.0 / 60.0 / 365.25 /*+ 0.75*/); }
INTNM::int64_t WTimeSpan::GetWeeks() const							{ return m_timeSpan / (7LL * 24LL * 60LL * 60LL * 1000000LL); }
INTNM::int64_t WTimeSpan::GetDays() const							{ return m_timeSpan / (24LL * 60LL * 60LL * 1000000LL); }
INTNM::int64_t WTimeSpan::GetTotalHours() const						{ return m_timeSpan / (60LL * 60LL * 1000000LL); }
INTNM::int32_t WTimeSpan::GetHours() const							{ return (INTNM::int32_t)(GetTotalHours() - GetDays() * 24); }
INTNM::int64_t WTimeSpan::GetTotalMinutes() const					{ return m_timeSpan / (60LL * 1000000LL); }
INTNM::int32_t WTimeSpan::GetMinutes() const						{ return (INTNM::int32_t)(GetTotalMinutes() - GetTotalHours() * 60); }
INTNM::int64_t WTimeSpan::GetTotalSeconds() const					{ return m_timeSpan / 1000000LL; }
INTNM::int32_t WTimeSpan::GetSeconds() const						{ return (INTNM::int32_t)(GetTotalSeconds() - GetTotalMinutes() * 60); }
INTNM::int32_t WTimeSpan::GetMilliSeconds() const					{ return (INTNM::int32_t)(GetTotalMilliSeconds() - GetTotalSeconds() * 1000); }
INTNM::int32_t WTimeSpan::GetTotalMilliSeconds() const				{ return m_timeSpan / 1000LL; }
INTNM::int32_t WTimeSpan::GetMicroSeconds() const					{ return m_timeSpan % 1000000LL; }
INTNM::int64_t WTimeSpan::GetTotalMicroSeconds() const				{ return m_timeSpan; }

double WTimeSpan::GetDaysFraction() const							{ return ((double)m_timeSpan) / (24.0 * 60.0 * 60.0 * 1000000.0); }
double WTimeSpan::GetSecondsFraction() const						{ return ((double)m_timeSpan) / 1000000.0; }
double WTimeSpan::GetFractionOfSecond() const						{ return ((double)(m_timeSpan % (1000000LL))) / (1000000.0); }
double WTimeSpan::GetFractionOfMinute() const						{ return ((double)(m_timeSpan % (60LL * 1000000LL))) / (60.0 * 1000000.0); }
double WTimeSpan::GetFractionOfHour() const							{ return ((double)(m_timeSpan % (60LL * 60LL * 1000000LL))) / (60.0 * 60.0 * 1000000.0); }
double WTimeSpan::GetFractionOfDay() const							{ return ((double)(m_timeSpan % (24LL * 60LL * 60LL * 1000000LL))) / (24.0 * 60.0 * 60.0 * 1000000.0); }
INTNM::int32_t WTimeSpan::GetSecondsOfDay() const					{ return m_timeSpan % (24LL * 60LL * 60LL * 1000000LL); }

void WTimeSpan::PurgeToSecond()										{ m_timeSpan = m_timeSpan - (m_timeSpan % (1000000LL)); }
void WTimeSpan::PurgeToMinute()										{ m_timeSpan = m_timeSpan - (m_timeSpan % (60LL * 1000000LL)); }
void WTimeSpan::PurgeToHour()										{ m_timeSpan = m_timeSpan - (m_timeSpan % (60LL * 60LL * 1000000LL)); }
void WTimeSpan::PurgeToDay()										{ m_timeSpan = m_timeSpan - (m_timeSpan % (60LL * 60LL * 24LL * 1000000LL)); }

const WTimeSpan& WTimeSpan::operator=(const WTimeSpan &timeSrc)		{ if (&timeSrc != this) { m_timeSpan = timeSrc.m_timeSpan; } return *this; }
WTimeSpan WTimeSpan::operator-(const WTimeSpan &timeSpan) const		{ return WTimeSpan(m_timeSpan - timeSpan.m_timeSpan, false); }
WTimeSpan WTimeSpan::operator+(const WTimeSpan &timeSpan) const		{ return WTimeSpan(m_timeSpan + timeSpan.m_timeSpan, false); }
const WTimeSpan& WTimeSpan::operator-=(const WTimeSpan &timeSpan)	{ m_timeSpan -= timeSpan.m_timeSpan; return *this; }
const WTimeSpan& WTimeSpan::operator+=(const WTimeSpan &timeSpan)	{ m_timeSpan += timeSpan.m_timeSpan; return *this; }
WTimeSpan WTimeSpan::operator*(INTNM::int32_t factor) const			{ return WTimeSpan(m_timeSpan * factor, false); }
WTimeSpan WTimeSpan::operator/(INTNM::int32_t factor) const			{ return WTimeSpan(m_timeSpan / factor, false); }
double WTimeSpan::operator/(const WTimeSpan &timeSpan) const		{ return (double)(((long double)m_timeSpan) / ((long double)timeSpan.m_timeSpan)); }
WTimeSpan WTimeSpan::operator*(double factor) const					{ return WTimeSpan((INTNM::int64_t)((long double)m_timeSpan * factor), false); }
WTimeSpan WTimeSpan::operator/(double factor) const					{ return WTimeSpan((INTNM::int64_t)((long double)m_timeSpan / factor), false); }
const WTimeSpan WTimeSpan::operator*=(INTNM::int32_t factor)		{ m_timeSpan *= factor; return *this; }
const WTimeSpan WTimeSpan::operator/=(INTNM::int32_t factor)		{ m_timeSpan /= factor; return *this; }
const WTimeSpan WTimeSpan::operator*=(double f)						{ long double ts = (long double)m_timeSpan; ts *= f; m_timeSpan = (INTNM::int64_t)ts; return *this; }
const WTimeSpan WTimeSpan::operator/=(double f)						{ long double ts = (long double)m_timeSpan; ts /= f; m_timeSpan = (INTNM::int64_t)ts; return *this; }

#ifdef __cpp_impl_three_way_comparison
auto WTimeSpan::operator<=>(const WTimeSpan& timeSpan) const		{ return (m_timeSpan <=> timeSpan.m_timeSpan); }
#endif
bool WTimeSpan::operator==(const WTimeSpan &timeSpan) const			{ return (m_timeSpan == timeSpan.m_timeSpan); }
bool WTimeSpan::operator!=(const WTimeSpan &timeSpan) const			{ return (m_timeSpan != timeSpan.m_timeSpan); }
bool WTimeSpan::operator<(const WTimeSpan &timeSpan) const			{ return (m_timeSpan < timeSpan.m_timeSpan); }
bool WTimeSpan::operator>(const WTimeSpan &timeSpan) const			{ return (m_timeSpan > timeSpan.m_timeSpan); }
bool WTimeSpan::operator<=(const WTimeSpan &timeSpan) const			{ return (m_timeSpan <= timeSpan.m_timeSpan); }
bool WTimeSpan::operator>=(const WTimeSpan &timeSpan) const			{ return (m_timeSpan >= timeSpan.m_timeSpan); }

bool WTimeSpan::ParseTime(const TCHAR *lpszTime) {
#ifdef _UNICODE
    std::vector<char> buffer;
    size_t size = wcstombs(NULL, lpszTime, 0);
    if (size > 0)
    {
        buffer.resize(size);
        wcstombs(static_cast<char*>(&buffer[0]), lpszTime, buffer.size());
    }
    std::string time(&buffer[0]);
#else
	std::string time(lpszTime);
#endif
	return ParseTime(time);
}


#if defined(TIMES_WINDOWS) && !defined(_NO_MFC)
bool WTimeSpan::ParseTime(const CString &time)
{
	CT2CA pszConvertedAnsiString(time);
	return ParseTime(std::string(pszConvertedAnsiString));
}
#endif


bool WTimeSpan::ParseTime(const std::string &time) {
	short cnt;
	WTimeSpan ts(time, &cnt);
	if (cnt > 0)
		*this = ts;
	return (cnt > 0);
}


std::string WTimeSpan::ToString(INTNM::uint32_t flags) const {
	std::string str;
	if ((flags & WTIME_FORMAT_STRING_TIMEZONE))
	{
		if (m_timeSpan == 0)
			str = "PT0M";
		else
		{
			INTNM::int32_t	year = (INTNM::int32_t)GetYears(),
				day = (INTNM::int32_t)(GetDays() - ((long double)year * 365.25 /*- 0.75*/)),
				hour = GetHours(),
				minute = GetMinutes(),
				second = GetSeconds(),
				usecs = GetMicroSeconds();

			if (m_timeSpan < 0)
				str = "-";
			else
				str.clear();
			str += "P";
			if (year < 0)
				year = -year;
			if (day < 0)
				day = -day;
			if (hour < 0)
				hour = -hour;
			if (minute < 0)
				minute = -minute;
			if (second < 0)
				second = -second;
			if (usecs < 0)
				usecs = -usecs;

			if (year > 0)
			{
				str += std::to_string(year);
				str += "Y";
			}
			if (day > 0)
			{
				str += std::to_string(day);
				str += "D";
			}
			if (hour > 0 || minute > 0 || second > 0 || usecs > 0)
			{
				str += "T";
				if (hour > 0)
				{
					str += std::to_string(hour);
					str += "H";
				}
				if (minute > 0)
				{
					str += std::to_string(minute);
					str += "M";
				}
				if (second > 0 || usecs > 0)
				{
					str += std::to_string(second);
					if (usecs > 0)
					{
						str += ".";
						std::string temp = "";
						std::string::size_type zeroIdx = 5;
						for (int i = zeroIdx; i >= 0; i--)
						{
							INTNM::int32_t digit = usecs % 10;
							temp = ((char)(digit + '0')) + temp;
							if (zeroIdx == i && digit == 0)
								zeroIdx--;
							usecs /= 10;
						}
						if (zeroIdx != (temp.length() - 1))
						{
							temp = temp.substr(0, zeroIdx + 1);
						}
						str += temp;
					}
					str += "S";
				}
			}
		}
	}
	else
	{
		char buff[128];
		INTNM::int32_t	year = (INTNM::int32_t)GetYears(),
			day = (flags & WTIME_FORMAT_YEAR) ? ((INTNM::int32_t)(GetDays() - ((long double)year * 365.25 /*- 0.75*/))) : ((INTNM::int32_t)GetDays()),
			hour = (flags & WTIME_FORMAT_DAY) ? GetHours() : (INTNM::int32_t)GetTotalHours(),
			minute = GetMinutes(),
			second = GetSeconds(),
			usecs = GetMicroSeconds();
		bool	special_case = false;

		if (m_timeSpan < 0) {
			if (flags & WTIME_FORMAT_EXCLUDE_SECONDS) {
				if (second <= -30)			// perform any rounding
					minute--;
				if (minute == -60) {
					hour--;
					minute = 0;
				}
				if (hour == -24) {
					day--;
					hour = 0;
				}
			}
			if ((day != 0) && (flags & WTIME_FORMAT_DAY))
				hour = 0 - hour;		// if there there are days, then we don't need the sign on the hour
			if (hour == 0)
				special_case = true;		// if no days and no hours, but it's negative, then we have a special print case
			minute = 0 - minute;			// take care of the sign which isn't needed for the minute
			second = 0 - second;			// ...or the seconds
			usecs = 0 - usecs;
		} else if (flags & WTIME_FORMAT_EXCLUDE_SECONDS) {
			if (second >= 30)			// perform any rounding
				minute++;
			if (minute == 60) {
				hour++;
				minute = 0;
			}
			if (hour == 24) {
				day++;
				hour = 0;
			}
		}
		if ((!year) || (!(flags & WTIME_FORMAT_YEAR))) {
			if ((!day) || (!(flags & WTIME_FORMAT_DAY))) {
				if (flags & WTIME_FORMAT_EXCLUDE_SECONDS) {
					if (special_case) { tm_snprintf(buff, 128, "-0:%2d", minute); str = buff; }
					else { tm_snprintf(buff, 128, "%02d:%02d", hour, minute); str = buff; }
				} else if (special_case) {
					if (flags & WTIME_FORMAT_INCLUDE_USECS) { tm_snprintf(buff, 128, "-0:%02d:%02d.%06d", minute, second, usecs); str = buff; }
					else { tm_snprintf(buff, 128, "-0:%02d:%02d", minute, second); str = buff; }
				} else {
					if (flags & WTIME_FORMAT_INCLUDE_USECS) { tm_snprintf(buff, 128, "%02d:%02d:%02d.%06d", hour, minute, second, usecs); str = buff; }
					else { tm_snprintf(buff, 128, "%02d:%02d:%02d", hour, minute, second); str = buff; }
				}
			} else {
				if ((!hour) && (!minute) && (!second) && (flags & WTIME_FORMAT_CONDITIONAL_TIME)) {
					// only day data and we're told to do that, so don't print hours, min's, sec's
					if (day == 1)						str = "1 day";
					else { tm_snprintf(buff, 128, "%d days", day); str = buff; }
				} else if (day == 1) {
					if (flags & WTIME_FORMAT_EXCLUDE_SECONDS) { tm_snprintf(buff, 128, "1 day %02d:%02d", hour, minute); str = buff; }
					else {
						if (flags & WTIME_FORMAT_INCLUDE_USECS) { tm_snprintf(buff, 128, "1 day %02d:%02d:%02d.%06d", hour, minute, second, usecs); str = buff; }
						else { tm_snprintf(buff, 128, "1 day %02d:%02d:%02d", hour, minute, second); str = buff; }
					}
				} else {
					if (flags & WTIME_FORMAT_EXCLUDE_SECONDS) { tm_snprintf(buff, 128, "%d days %02d:%02d", day, hour, minute); str = buff; }
					else {
						if (flags & WTIME_FORMAT_INCLUDE_USECS) { tm_snprintf(buff, 128, "%d days %02d:%02d:%02d.%06d", day, hour, minute, second, usecs); str = buff; }
						else { tm_snprintf(buff, 128, "%d days %02d:%02d:%02d", day, hour, minute, second); str = buff; }
					}
				}
			}
		} else {
			if ((!day) && (!hour) && (!minute) && (!second) && (flags & WTIME_FORMAT_CONDITIONAL_TIME)) {
				// only day data and we're told to do that, so don't print hours, min's, sec's
				if (year == 1)							str = "1 year";
				else { tm_snprintf(buff, 128, "%d years", year); str = buff; }
			} else if (year == 1) {
				if (flags & WTIME_FORMAT_EXCLUDE_SECONDS) { tm_snprintf(buff, 128, "1 year %d days %02d:%02d", day, hour, minute); str = buff; }
				else {
					if (flags & WTIME_FORMAT_INCLUDE_USECS) { tm_snprintf(buff, 128, "1 year %d days %02d:%02d:%02d.%06d", day, hour, minute, second, usecs); str = buff; }
					else { tm_snprintf(buff, 128, "1 year %d days %02d:%02d:%02d", day, hour, minute, second); str = buff; }
				}
			} else {
				if (flags & WTIME_FORMAT_EXCLUDE_SECONDS) { tm_snprintf(buff, 128, "%d years %d days %02d:%02d", year, day, hour, minute); str = buff; }
				else {
					if (flags & WTIME_FORMAT_INCLUDE_USECS) { tm_snprintf(buff, 128, "%d years %d days %02d:%02d:%02d.%06d", year, day, hour, minute, second, usecs); str = buff; }
					else { tm_snprintf(buff, 128, "%d years %d days %02d:%02d:%02d", year, day, hour, minute, second); str = buff; }
				}
			}
		}
	}

	return str;
}


#if defined(TIMES_WINDOWS) && !defined(_NO_MFC)
COleDateTimeSpan WTimeSpan::AsCOleDateTimeSpan() const {
	COleDateTimeSpan ts((INTNM::int32_t)GetDays(), GetHours(), GetMinutes(), GetSeconds());
	return ts;
}
#endif


WTimeManager::WTimeManager(const WorldLocation &worldLocation) : m_worldLocation(worldLocation) {
}


WTimeSpan WTimeManager::TimeForIndex(INTNM::int32_t index) {
	switch (index) {
		case ITERATION_1SEC:	return WTimeSpan(0, 0, 0, 1);
		case ITERATION_1MIN:	return WTimeSpan(0, 0, 1, 0);
		case ITERATION_5MIN:	return WTimeSpan(0, 0, 5, 0);
		case ITERATION_15MIN:	return WTimeSpan(0, 0, 15, 0);
		case ITERATION_30MIN:	return WTimeSpan(0, 0, 30, 0);
		case ITERATION_1HOUR:	return WTimeSpan(0, 1, 0, 0);
		case ITERATION_2HOUR:	return WTimeSpan(0, 2, 0, 0);
		case ITERATION_1DAY:	return WTimeSpan(1, 0, 0, 0);
	}
	return WTimeSpan(7, 0, 0, 0);
}


INTNM::int32_t WTimeManager::IterationIndex(const WTimeSpan &time) {
	const INTNM::int64_t iteration_amt[ITERATION_1WEEK + 1] = {
		1LL * 1000000LL,
		60LL * 1000000LL,
		60LL * 5LL * 1000000LL,
		60LL * 15LL * 1000000LL,
		60LL * 30LL * 1000000LL,
		60LL * 60LL * 1000000LL,
		60LL * 60LL * 2LL * 1000000LL,
		60LL * 60LL * 24LL * 1000000LL,
		60LL * 60LL * 24LL * 7LL * 1000000LL };

	INTNM::int32_t i;
	for (i = 0; i < ITERATION_1WEEK; i++)
		if (iteration_amt[i] >= time.GetTotalMicroSeconds())
			break;
	return i;
}


bool WTimeManager::isLeapYear(INTNM::int16_t year) {
	return CSunriseSunsetCalc::isLeapYear(year); 
}


INTNM::int16_t WTimeManager::daysInMonth(INTNM::int16_t month, INTNM::int16_t year) {
	static const INTNM::int16_t days_in_months[12] = {
		31 /* January */,
		28 /* February */,
		31 /* March */,
		30 /* April */,
		31 /* May */,
		30 /* June */,
		31 /* July */,
		31 /* August */,
		30 /* September */,
		31 /* October */,
		30 /* November */,
		31 /* December */
	};
	if ((month == 2) && (isLeapYear(year)))
		return 29;
	return days_in_months[--month];
}


INTNM::int16_t WTimeManager::GetJulianCount(INTNM::int16_t year) {
	if (isLeapYear(year))
		return 366;
	return 365;
}


static void normalize(INTNM::int16_t *year, INTNM::int16_t *julian) {		// get julian into the range of (0...365or366),
	INTNM::int16_t i;						// adjust the year value accordingly
	if (*julian > 0) {
		i = WTimeManager::GetJulianCount(*year);
		while (*julian > i) {
			(*year)++;
			*julian -= i;
			i = WTimeManager::GetJulianCount(*year);
		}
	} else {
		while (*julian < 0) {
			(*year)--;
			*julian += WTimeManager::GetJulianCount(*year);
		}
	}
}


INTNM::int16_t WTimeManager::ToJulian(INTNM::int16_t year, INTNM::int16_t month, INTNM::int16_t day) {
	short i, calculated = 0;
	if (year < 70)
		year += 2000;
	else if (year < 100)
		year += 1900;
	while (year < 1600)
		year += 100;					// make sure our start time is appropriate for leap year calculations
	if (month > 0) {					// get months into range, adjust year accordingly to deal with leap years
		while (month > 12) {
			year++;
			month -= 12;
		}
	} else {
		while (month < 0) {
			year--;
			month += 12;
		}
	}
	normalize(&year, &day);					// take care of obvious range checking for the days too
	for (i = 1; i < month; i++)
		day += daysInMonth(month, year);
	normalize(&year, &day);
	calculated = day;
								// NOTE if day or month is way out of range, we could have a slight error in calculation
	return calculated;					// due to getting the years correct for leap year calculation...oh well...
}


void WTimeManager::FromJulian(INTNM::int16_t julian, INTNM::int16_t *year, INTNM::int16_t *month, INTNM::int16_t *day) {
	if (*year < 70)
		*year += 2000;
	else if (*year < 100)
		*year += 1900;
	while (*year < 1600)
		*year += 100;
	normalize(year, &julian);
	*month = 1;
	while (julian >= daysInMonth(*month, *year))
		julian -= daysInMonth((short)((*month)++), *year);
//	(*month)--;
	*day = (short)(julian + 1);
}


static std::string_view extractTimezoneFromPath(const char* rp)
{
	std::string_view result = rp;
	std::string_view zoneinfo = "zoneinfo";
	std::size_t pos = result.rfind(zoneinfo);
	if (pos == result.npos)
		return "";
	pos = result.find('/', pos);
	result.remove_prefix(pos + 1);
	return result;
}


static std::string findTimezonePath()
{
	struct stat sb;
	constexpr auto defaultDir = "/usr/share/zoneinfo";
	constexpr auto buildRootDir = "/usr/share/zoneinfo/uclibc";

	if (stat(buildRootDir, &sb) == 0 && S_ISDIR(sb.st_mode))
		return buildRootDir;
	else if (stat(defaultDir, &sb) == 0 && S_ISDIR(sb.st_mode))
		return defaultDir;
	return "";
}


static std::string cachedFindTimezonePath()
{
	static const std::string dir = findTimezonePath();
	return dir;
}


static bool checkValidFile(const char* timezone)
{
	char rp[PATH_MAX + 1] = {};
	if (realpath(timezone, rp) == nullptr)
		return false;
	return extractTimezoneFromPath(rp) != "posixrules";
}


const WTimeManager WTimeManager::GetSystemTimeManager(WorldLocation& location)
{
#ifdef _MSC_VER
	DYNAMIC_TIME_ZONE_INFORMATION dtzi{};
	if (GetDynamicTimeZoneInformation(&dtzi) != TIME_ZONE_ID_INVALID)
	{
		auto len = wcslen(dtzi.TimeZoneKeyName);
		char buffer[128];
		memset(buffer, 0, 128);
		wcstombs(buffer, dtzi.TimeZoneKeyName, len);
		auto zone = WorldLocation::TimeZoneFromName(buffer, dtzi.DaylightBias);

		bool isDST = false;
		if (dtzi.DaylightBias != 0)
		{
			time_t now;
			time(&now);
			tm tnow;
			gmtime_s(&tnow, &now);
			//it is the month that DST starts
			if (tnow.tm_mon == dtzi.DaylightDate.wMonth)
				isDST = (tnow.tm_mday > dtzi.DaylightDate.wDay) || (tnow.tm_mday == dtzi.DaylightDate.wDay && tnow.tm_hour > dtzi.DaylightDate.wHour);
			//it is the month that DST ends
			else if (tnow.tm_mon == dtzi.StandardDate.wMonth)
				isDST = (tnow.tm_mday < dtzi.StandardDate.wDay) || (tnow.tm_mday == dtzi.StandardDate.wDay && tnow.tm_hour < dtzi.StandardDate.wHour);
			//daylight saving starts earlier in the year than standard time
			else if (dtzi.DaylightDate.wMonth < dtzi.StandardDate.wMonth)
				isDST = (tnow.tm_mon > dtzi.DaylightDate.wMonth) && (tnow.tm_mon < dtzi.StandardDate.wMonth);
			//standard time start earlier in the year than daylight savings time
			else
				isDST = (tnow.tm_mon > dtzi.DaylightDate.wMonth) || (tnow.tm_mon < dtzi.StandardDate.wMonth);
		}

		if (zone)
		{
			if (isDST)
				zone = WorldLocation::GetDaylightSavingsTimeZone(zone);
			location.SetTimeZoneOffset(zone);
		}
		else
		{
			location.m_timezone(HSS_Time::WTimeSpan((dtzi.Bias + dtzi.StandardBias) * 60));
			//it is currently DST
			if (isDST)
			{
				location.m_amtDST(HSS_Time::WTimeSpan(dtzi.DaylightBias * 60));
				location.m_startDST(HSS_Time::WTimeSpan(0));
				location.m_endDST(HSS_Time::WTimeSpan(366, 0, 0, 0));
			}
		}
	}
#else
	std::string timezoneRegion;
	bool complete = false;
	//for OSs that store time info in a file like "/usr/share/zoneinfo/America/Winnipeg"
	//using the symlink "/etc/localtime"
	{
		bool found = true;
		struct stat sb;
		constexpr auto timezone = "/etc/localtime";
		if (lstat(timezone, &sb) == 0 && S_ISLNK(sb.st_mode) && sb.st_size > 0) {
			static const bool useRealpath = checkValidFile(timezone);
			char rp[PATH_MAX + 1] = {};
			if (useRealpath) {
				if (realpath(timezone, rp) == nullptr)
					found = false;//error
			}
			else {
				if (readlink(timezone, rp, sizeof(rp) - 1) <= 0)
					found = false;//error
			}

			if (found)
				timezoneRegion = extractTimezoneFromPath(rp);
			complete = true;
		}
	}

	//for OSs that store time info in a file like "/usr/share/zoneinfo/America/Winnipeg"
	//using the symlink "/etc/TZ"
	if (!complete) {
		struct stat sb;
		constexpr auto timezone = "/etc/TZ";
		if (lstat(timezone, &sb) == 0 && S_ISLNK(sb.st_mode) && sb.st_size > 0) {
			char rp[PATH_MAX + 1] = {};
			if (readlink(timezone, rp, sizeof(rp) - 1) > 0) {
				timezoneRegion = std::string(rp);

				const std::size_t pos = timezoneRegion.find(cachedFindTimezonePath());
				if (pos != timezoneRegion.npos)
					timezoneRegion.erase(0, cachedFindTimezonePath().size() + 1 + pos);
			}
			complete = true;
		}
	}

	//for OSs where the timezone is the first line of "/etc/timezone"
	if (!complete) {
		std::ifstream file("/etc/timezone");
		if (file.is_open()) {
			std::getline(file, timezoneRegion);
			if (!timezoneRegion.empty())
				complete = true;
		}
	}

	//for OSs where the timezone is the first line of "/var/db/zoneinfo"
	if (!complete) {
		std::ifstream file("/var/db/zoneinfo");
		if (file.is_open()) {
			std::getline(file, timezoneRegion);
			if (!timezoneRegion.empty())
				complete = true;
		}
	}

	//for OSs where the timezone is in "/etc/sysconfig/clock"
	//and stored as 'ZONE="America/Winnipeg"'
	if (!complete) {
		std::ifstream file("/etc/sysconfig/clock");
		std::string result;
		while (file) {
			std::getline(file, result);
			auto p = result.find("ZONE=\"");
			if (p != std::string::npos) {
				result.erase(p, p + 6);
				result.erase(result.rfind('"'));
				complete = true;
				break;
			}
		}
	}

	if (timezoneRegion.size()) {
		auto zone = WorldLocation::TimeZoneFromName(timezoneRegion);
		if (zone)
			location.SetTimeZoneOffset(zone);
	}
#endif

	return WTimeManager(std::move(location));
}


#if defined(TIMES_WINDOWS) && !defined(_NO_MFC)
WTime::WTime(const COleDateTime& timeSrc, const WTimeManager *tm, INTNM::uint32_t flags) {
	construct_time_t(timeSrc.GetYear(), timeSrc.GetMonth(), timeSrc.GetDay(), timeSrc.GetHour(), timeSrc.GetMinute(), timeSrc.GetSecond() );
	m_tm = tm;
	INTNM::uint64_t atm = adjusted_tm(flags);
	m_time = m_time - (atm - m_time);
}
#endif


WTime WTime::Now(const WTimeManager *tm, INTNM::uint32_t flags)
{
    time_t t = std::time(0);
	struct tm now;

#ifdef _MSC_VER
	gmtime_s(&now, &t);
#else
	gmtime_r(&t, &now);
#endif

	auto seconds = now.tm_sec;
    if (flags & WTIME_FORMAT_EXCLUDE_SECONDS)
        seconds = 0;
    WTime temp(now.tm_year + 1900, now.tm_mon + 1, now.tm_mday, now.tm_hour, now.tm_min, seconds, tm);
    return WTime(temp, flags, -1);
}

#if TIMES_STATIC==1
WTime   WTime::_gmin(1900, 1, 1, 0, 0, 0, nullptr);
WTime	WTime::_gmax(2100, 1, 1, 0, 0, 0, nullptr);
#else
WTime   _gmin(1900, 1, 1, 0, 0, 0, nullptr),
		_gmax(2100, 1, 1, 0, 0, 0, nullptr);
#endif

WTime& WTime::GlobalMin() { return _gmin; }
WTime& WTime::GlobalMax() {	return _gmax; }

WTime WTime::GlobalMin(const WTimeManager* tm) { return WTime(_gmin, tm); }
WTime WTime::GlobalMax(const WTimeManager* tm) { return WTime(_gmax, tm); }


WTime::WTime(const WTimeManager *tm) { 
	m_time = (INTNM::uint64_t)(-1);
	m_tm = tm;
}


WTime::WTime(const WTime& timeSrc) {
	m_time = timeSrc.m_time;
	m_tm = timeSrc.m_tm;
}


WTime::WTime(const WTime& timeSrc, const WTimeManager *tm) {
	m_time = timeSrc.m_time;
	m_tm = tm;
}


WTime::WTime(const WTime& timeSrc, INTNM::uint32_t flags, INTNM::int16_t direction) {
	m_time = timeSrc.m_time;
	m_tm = timeSrc.m_tm;
	if (m_tm) {
		INTNM::uint64_t atm = adjusted_tm(flags);
		if (direction < 0)		m_time = m_time - (atm - m_time);
		else if (direction > 0)		m_time = atm;
	}
}


WTime::WTime(INTNM::uint64_t time, const WTimeManager *tm, bool units_are_seconds) { 
	if ((units_are_seconds) && (time != (INTNM::uint64_t)(-1)))
		m_time = time * 1000000LL; 
	else 
		m_time = time; 
	m_tm = tm;
}


WTime::WTime(const GDALTime &time, const WTimeManager* tm) {
	if (time.theTime != (INTNM::uint64_t)(-1)) {
		m_time = time.theTime * 1000000LL + (time.millisecs % 1000LL) * 1000LL;
		if (time.nTZFlag == 1) {	// 0 is unknown, 1 is local time, 100 is GMT according to GDAL doc's
			WTime gmt(*this, WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST, -1);
			m_time = gmt.m_time;
		}
	} else
		m_time = time.theTime;
	m_tm = tm;
}


WTime::WTime(INTNM::int32_t nYear, INTNM::int32_t nMonth, INTNM::int32_t nDay, INTNM::int32_t nHour, INTNM::int32_t nMin, INTNM::int32_t nSec, const WTimeManager *tm) {
	construct_time_t(nYear, nMonth, nDay, nHour, nMin, nSec); 

	m_tm = tm;
}


WTime::WTime(INTNM::int32_t nYear, INTNM::int32_t nMonth, INTNM::int32_t nDay, INTNM::int32_t nHour, INTNM::int32_t nMin, double nSec, const WTimeManager *tm) {
	construct_time_t(nYear, nMonth, nDay, nHour, nMin, 0L); 
	m_time += (INTNM::uint64_t)(nSec * 1000000.0);
	m_tm = tm;
}


WTime::WTime(INTNM::int32_t nYear, INTNM::int32_t nMonth, INTNM::int32_t nDay, INTNM::int32_t nHour, INTNM::int32_t nMin, INTNM::int32_t nSec, INTNM::int32_t uSec, const WTimeManager *tm) {
	construct_time_t(nYear, nMonth, nDay, nHour, nMin, nSec); 
	m_time += uSec; 
	m_tm = tm;
}

void WTime::construct_time_t(INTNM::int32_t nYear, INTNM::int32_t nMonth, INTNM::int32_t nDay, INTNM::int32_t nHour, INTNM::int32_t nMin, INTNM::int32_t nSec)
{
	if (nMonth <= 2) 
	{
		nYear -= 1;
		nMonth += 12;
	}
	INTNM::int32_t A = nYear/100LL;
	INTNM::int32_t B = 2LL - A + A/4;
	m_time = floor(365.25*(nYear + 4716)) + floor(30.6001*(nMonth+1)) + (nDay+1) + B - 1524.5;
	m_time -= 2305448; //normalize to epoch
	m_time *= 24;
	m_time += nHour;
	m_time *= 60;
	m_time += nMin;
	m_time *= 60;
	m_time += nSec;							// m_time is # secs since Jan 1, 1600
	m_time *= 1000000;
}


INTNM::uint64_t WTime::adjusted_tm(INTNM::uint32_t flags) const {
	weak_assert(m_time != (INTNM::uint64_t)(-1000000));
	if ((!m_time) || (m_time == (INTNM::uint64_t)-1))
		return m_time;
	if (!flags) 
		return m_time; 
	return adjusted_tm_math(flags);
}


#if defined(TIMES_WINDOWS) && !defined(_NO_MFC)
COleDateTime WTime::AsCOleDateTime(INTNM::uint32_t flags) const {
	INTNM::uint64_t atm = adjusted_tm(flags);
	WTime nt(atm, m_tm, false);
	INTNM::int32_t	year = nt.GetYear(0),
		month = nt.GetMonth(0),
		day = nt.GetDay(0),
		hour = nt.GetHour(0),
		min = nt.GetMinute(0),
		sec = nt.GetSecond(0);
	COleDateTime t(year, month, day, hour, min, sec);
	if (t.GetYear() == year)
		return t;
	return COleDateTime((INTNM::int64_t)0);
}
#endif


INTNM::uint64_t WTime::adjusted_tm_math(INTNM::uint32_t mode) const {
	weak_assert(m_time != (INTNM::uint64_t)(-1000000));

	if (mode & WTIME_FORMAT_AS_SOLAR)
		if (mode & (WTIME_FORMAT_WITHDST | WTIME_FORMAT_AS_LOCAL))
			throw std::runtime_error("Operation not supported");
	INTNM::uint64_t time;
	if (m_tm) {
		if (mode & WTIME_FORMAT_AS_LOCAL)
			time = m_time + m_tm->m_worldLocation.m_timezone().GetTotalMicroSeconds();
		else if (mode & WTIME_FORMAT_AS_SOLAR)
			time = m_time + m_tm->m_worldLocation.m_solar_timezone(*this).GetTotalMicroSeconds();
		else	time = m_time;

		if ((mode & WTIME_FORMAT_WITHDST) && (m_tm->m_worldLocation.m_startDST() != m_tm->m_worldLocation.m_endDST())) {
			WTime t(time, nullptr, false);
			INTNM::uint64_t secs = t.GetSecondsIntoYear(0);
			if (m_tm->m_worldLocation.m_startDST() < m_tm->m_worldLocation.m_endDST()) {
				if (((INTNM::uint64_t)m_tm->m_worldLocation.m_startDST().GetTotalSeconds() <= secs) &&
				    (secs < (INTNM::uint64_t)m_tm->m_worldLocation.m_endDST().GetTotalSeconds()))
					time += m_tm->m_worldLocation.m_amtDST().GetTotalMicroSeconds();
			} else {
				if (((INTNM::uint64_t)m_tm->m_worldLocation.m_startDST().GetTotalSeconds() < secs) ||
				    (secs <= (INTNM::uint64_t)m_tm->m_worldLocation.m_endDST().GetTotalSeconds()))
					time += m_tm->m_worldLocation.m_amtDST().GetTotalMicroSeconds();
			}
		}
	} else	time = m_time;
	return time;
}



INTNM::uint64_t WTime::GetTime(INTNM::uint32_t mode) const			{ if (m_time != (INTNM::uint64_t)(-1)) return adjusted_tm(mode) / 1000000LL; return m_time;  };
INTNM::uint64_t WTime::GetTotalSeconds() const						{ if (m_time != (INTNM::uint64_t)(-1)) return m_time / 1000000LL; return m_time; };
INTNM::uint64_t WTime::GetTotalMilliSeconds() const					{ if (m_time != (INTNM::uint64_t)(-1)) return m_time / 1000LL; return m_time; }
INTNM::uint64_t WTime::GetTotalMicroSeconds() const					{ return m_time; };
const WTimeManager* WTime::GetTimeManager() const					{ return m_tm; };
const WTimeManager *WTime::SetTimeManager(const WTimeManager *tm)	{ m_tm = tm; return m_tm; };
bool WTime::IsValid() const											{ return (m_time != (INTNM::uint64_t)-1); }


GDALTime WTime::AsGDALTime(INTNM::uint32_t mode) const {
	GDALTime theTime;
	if (m_time != (INTNM::uint64_t)(-1))
		theTime.theTime = (adjusted_tm(mode) - WTIME_1970) / 1000000LL;
	theTime.millisecs = GetMilliSeconds(mode);
	if (m_tm)
		theTime.nTZFlag = (mode & WTIME_FORMAT_AS_LOCAL) ? 1 : 100;
	else
		theTime.nTZFlag = 0;
	return theTime;
}


INTNM::int32_t WTime::GetYear(INTNM::uint32_t mode) const {
	if (m_time == (INTNM::uint64_t)(-1))
		return -1;

	INTNM::uint64_t z = adjusted_tm(mode) / 24 / 60 / 60 / 1000000;
	z += 2305448;

	INTNM::int32_t a = z + 32044;
	INTNM::int32_t b = (4*a+3)/146097;
	INTNM::int32_t c = a - (b*146097)/4;
	INTNM::int32_t d = (4*c+3)/1461;
	INTNM::int32_t e = c - (1461*d)/4;
	INTNM::int32_t m = (5*e+2)/153;
	INTNM::int32_t nyear = b*100 + d - 4800 + m/10;
	return nyear;
}


INTNM::int32_t WTime::GetMonth(INTNM::uint32_t mode) const {
	if (m_time == (INTNM::uint64_t)(-1))
		return -1;

	INTNM::uint64_t z = adjusted_tm(mode) / 24 / 60 / 60 / 1000000;
	z += 2305448;
	INTNM::int32_t a = z + 32044;
	INTNM::int32_t b = (4*a+3)/146097;
	INTNM::int32_t c = a - (b*146097)/4;
	INTNM::int32_t d = (4*c+3)/1461;
	INTNM::int32_t e = c - (1461*d)/4;
	INTNM::int32_t m = (5*e+2)/153;
	INTNM::int32_t nmonth = m + 3 - 12*(m/10);
	return nmonth;
}


INTNM::int32_t WTime::GetDay(INTNM::uint32_t mode) const {
	if (m_time == (INTNM::uint64_t)(-1))
		return -1;

	INTNM::uint64_t z = adjusted_tm(mode) / 24 / 60 / 60 / 1000000;
	z += 2305448;

	INTNM::int32_t a = z + 32044;
	INTNM::int32_t b = (4*a+3)/146097;
	INTNM::int32_t c = a - (b*146097)/4;
	INTNM::int32_t d = (4*c+3)/1461;
	INTNM::int32_t e = c - (1461*d)/4;
	INTNM::int32_t m = (5*e+2)/153;
	INTNM::int32_t nday = e - (153*m+2)/5 + 1;
	return nday;
}


INTNM::int32_t WTime::GetHour(INTNM::uint32_t flags) const				{ if (m_time == (INTNM::uint64_t)(-1)) return -1; return (INTNM::int32_t)((adjusted_tm(flags) / (60LL * 60LL * 1000000LL)) % 24); };
INTNM::int32_t WTime::GetMinute(INTNM::uint32_t flags) const			{ if (m_time == (INTNM::uint64_t)(-1)) return -1; return (INTNM::int32_t)((adjusted_tm(flags) / (60LL * 1000000LL)) % 60); };
INTNM::int32_t WTime::GetSecond(INTNM::uint32_t flags) const			{ if (m_time == (INTNM::uint64_t)(-1)) return -1; return (INTNM::int32_t)((adjusted_tm(flags) / 1000000LL) % 60); };
INTNM::int32_t WTime::GetMilliSeconds(INTNM::uint32_t flags) const		{ if (m_time == (INTNM::uint64_t)(-1)) return -1; return (INTNM::int32_t)((adjusted_tm(flags) / 1000LL) % 1000); };
INTNM::int32_t WTime::GetMicroSeconds(INTNM::uint32_t flags) const		{ if (m_time == (INTNM::uint64_t)(-1)) return -1; return (INTNM::int32_t)((adjusted_tm(flags) % 1000000LL)); };
double WTime::GetSecondsFraction(INTNM::uint32_t flags) const			{ if (m_time == (INTNM::uint64_t)(-1)) return -1.0; return ((double)(adjusted_tm(flags) % 1000000LL)) / 1000000.0; };
WTimeSpan WTime::GetTimeOfDay(INTNM::uint32_t flags) const				{ if (m_time == (INTNM::uint64_t)(-1)) return WTimeSpan(-1, false); INTNM::int64_t time = adjusted_tm(flags) % (60LL * 60LL * 24LL * 1000000LL); return WTimeSpan(time, false); };
double WTime::GetFractionOfSecond(INTNM::uint32_t flags) const			{ if (m_time == (INTNM::uint64_t)(-1)) return -1.0; INTNM::int64_t time = adjusted_tm(flags) % (1000000LL); return ((double)time) / (1000000.0); };
double WTime::GetFractionOfMinute(INTNM::uint32_t flags) const			{ if (m_time == (INTNM::uint64_t)(-1)) return -1.0; INTNM::int64_t time = adjusted_tm(flags) % (60LL * 1000000LL); return ((double)time) / (60.0 * 1000000.0); };
double WTime::GetFractionOfHour(INTNM::uint32_t flags) const			{ if (m_time == (INTNM::uint64_t)(-1)) return -1.0; INTNM::int64_t time = adjusted_tm(flags) % (60LL * 60LL * 1000000LL); return ((double)time) / (60.0 * 60.0 * 1000000.0); };
double WTime::GetFractionOfDay(INTNM::uint32_t flags) const				{ if (m_time == (INTNM::uint64_t)(-1)) return -1.0; INTNM::int64_t time = adjusted_tm(flags) % (60LL * 60LL * 24LL * 1000000LL); return ((double)time) / (24.0 * 60.0 * 60.0 * 1000000.0); };
INTNM::int32_t WTime::GetDayOfWeek(INTNM::uint32_t flags) const			{ if (m_time == (INTNM::uint64_t)(-1)) return -1; INTNM::int64_t days = adjusted_tm(flags) / (24LL * 60LL * 60LL * 1000000LL) + 0; INTNM::int32_t ret = (INTNM::int32_t)(days % 7); return ret == 0 ? 7L : ret; };


INTNM::uint64_t WTime::GetSecondsIntoYear(INTNM::uint32_t mode) const {
	if (m_time == (INTNM::uint64_t)(-1))
		return -1;

	INTNM::uint64_t atm = adjusted_tm(mode) / 1000000;
	WTime year(GetYear(mode), 1, 1, 0, 0, 0, m_tm);
	return (atm - year.GetTotalSeconds());
}


WTimeSpan WTime::GetWTimeSpanIntoYear(INTNM::uint32_t mode) const {
	if (m_time == (INTNM::uint64_t)(-1))
		return WTimeSpan(-1, false);

	INTNM::uint64_t atm = adjusted_tm(mode);
	WTime year(GetYear(mode), 1, 1, 0, 0, 0, m_tm);
	return WTimeSpan((INTNM::int64_t)(atm - year.m_time), false);
}


bool WTime::IsLeapYear(INTNM::uint32_t flags) const { 
	if (m_time == (INTNM::uint64_t)(-1))
		return false;

	return WTimeManager::isLeapYear((INTNM::int16_t)GetYear(flags));
}


INTNM::int32_t WTime::GetDayOfYear(INTNM::uint32_t mode) const {
	if (m_time == (INTNM::uint64_t)(-1))
		return -1;

	INTNM::uint64_t atm = adjusted_tm(mode) / 24 / 60 / 60 / 1000000;
	WTime year(GetYear(mode), 1, 1, 0, 0, 0.0, m_tm);
	INTNM::uint64_t year_atm = year.adjusted_tm(0) / 24 / 60 / 60 / 1000000;
	return (atm - year_atm) + 1;
}


double WTime::GetDayFractionOfYear(INTNM::uint32_t mode) const {
	if (m_time == (INTNM::uint64_t)(-1))
		return -1.0;

	INTNM::uint64_t atm = adjusted_tm(mode) / 1000000;
	WTime year(GetYear(mode), 1, 1, 0, 0, 0.0, m_tm);
	INTNM::uint64_t total_secs = atm - year.GetTotalSeconds();
	return ((double)total_secs) / (24.0 * 60.0 * 60.0) + 1.0;
}


void WTime::PurgeToSecond(INTNM::uint32_t flags)			{ if (m_time != (INTNM::uint64_t)(-1)) m_time = m_time - (adjusted_tm(flags) % (1000000LL)); };
void WTime::PurgeToMinute(INTNM::uint32_t flags)			{ if (m_time != (INTNM::uint64_t)(-1)) m_time = m_time - (adjusted_tm(flags) % (60LL * 1000000LL)); };
void WTime::PurgeToHour(INTNM::uint32_t flags)				{ if (m_time != (INTNM::uint64_t)(-1)) m_time = m_time - (adjusted_tm(flags) % (60LL * 60LL * 1000000LL)); };
void WTime::PurgeToDay(INTNM::uint32_t flags)				{ if (m_time != (INTNM::uint64_t)(-1)) m_time = m_time - (adjusted_tm(flags) % (60LL * 60LL * 24LL * 1000000LL)); };


void WTime::PurgeToYear(INTNM::uint32_t flags) {
	if (m_time != (INTNM::uint64_t)(-1)) {
		WTimeSpan secs = GetWTimeSpanIntoYear(flags);
		m_time -= secs.GetTotalMicroSeconds();
	}
}


const WTime& WTime::operator++() {
	if (m_time != (INTNM::uint64_t)(-1)) {
		WTimeSpan incr;
		if (IsLeapYear(0))	incr = WTimeSpan(366, 0, 0, 0);
		else			incr = WTimeSpan(365, 0, 0, 0);
		return *this += incr;
	}
	return *this;
}


const WTime& WTime::operator--() {
	if (m_time != (INTNM::uint64_t)(-1)) {
		INTNM::int32_t year = (INTNM::int32_t)(GetYear(0) - 1);
		WTimeSpan decr;
		if (WTimeManager::isLeapYear((INTNM::int16_t)year))	decr = WTimeSpan(366, 0, 0, 0);
		else						decr = WTimeSpan(365, 0, 0, 0);
		return *this -= decr;
	}
	return *this;
}


const WTime& WTime::operator+=(INTNM::int32_t years) {
	if (m_time != (INTNM::uint64_t)(-1)) {
		WTimeSpan yr(365, 0, 0, 0);
		WTimeSpan lyr(366, 0, 0, 0);
		INTNM::int32_t year = (INTNM::int32_t)GetYear(0);
		INTNM::int32_t i;
		for (i = 0; i < years; i++, year++) {
			if (WTimeManager::isLeapYear((INTNM::int16_t)year))
				*this += lyr;
			else	*this += yr;
		}
	}
	return *this;
}


const WTime& WTime::operator-=(INTNM::int32_t years) {
	if (m_time != (INTNM::uint64_t)(-1)) {
		WTimeSpan yr(365, 0, 0, (INTNM::int32_t)0);
		WTimeSpan lyr(366, 0, 0, (INTNM::int32_t)0);
		INTNM::int32_t year = (INTNM::int32_t)GetYear(0);
		INTNM::int32_t i;
		for (i = 0, year--; i < years; i++, year--) {
			if (WTimeManager::isLeapYear((INTNM::int16_t)year))
				*this -= lyr;
			else	*this -= yr;
		}
	}
	return *this;
}


const WTime& WTime::operator=(const WTime& timeSrc) { 
	if (&timeSrc != this) {

#ifdef _DEBUG
		if (m_tm)
			weak_assert(m_tm == timeSrc.m_tm);
#endif

		m_time = timeSrc.m_time;
		m_tm = timeSrc.m_tm;
	}
	return *this;
}


const WTime& WTime::SetTime(const WTime& timeSrc) {
	if (&timeSrc != this) {
		m_time = timeSrc.m_time;
		if ((timeSrc.m_tm) && (!m_tm))
			m_tm = timeSrc.m_tm;
	}
	return *this;
}


bool WTime::operator==(const WTime &time) const {

#ifdef _DEBUG
	weak_assert(m_time != (INTNM::uint64_t)(-1000000));
	if ((m_time != (INTNM::uint64_t)-1) && (m_time != (INTNM::uint64_t)0) &&
		(time.m_time != (INTNM::uint64_t)-1) && (time.m_time != (INTNM::uint64_t)0))
		weak_assert(time.m_tm == m_tm);
#endif

	return (m_time == time.m_time);
}


bool WTime::operator!=(const WTime &time) const {

#ifdef _DEBUG
	weak_assert(m_time != (INTNM::uint64_t)(-1000000));
	if ((m_time != (INTNM::uint64_t)-1) && (m_time != (INTNM::uint64_t)0) &&
		(time.m_time != (INTNM::uint64_t)-1) && (time.m_time != (INTNM::uint64_t)0))
		weak_assert(time.m_tm == m_tm);
#endif

	return (m_time != time.m_time);
}


WTime WTime::operator-(const WTimeSpan &timeSpan) const		{ if (m_time != (INTNM::uint64_t)(-1)) return WTime(m_time - timeSpan.GetTotalMicroSeconds(), m_tm, false); return *this; }
WTime WTime::operator+(const WTimeSpan &timeSpan) const		{ if (m_time != (INTNM::uint64_t)(-1)) return WTime(m_time + timeSpan.GetTotalMicroSeconds(), m_tm, false); return *this; }
const WTime& WTime::operator-=(const WTimeSpan &timeSpan)	{ if (m_time != (INTNM::uint64_t)(-1)) m_time -= timeSpan.GetTotalMicroSeconds(); return *this; }
const WTime& WTime::operator+=(const WTimeSpan &timeSpan)	{ if (m_time != (INTNM::uint64_t)(-1)) m_time += timeSpan.GetTotalMicroSeconds(); return *this; }
WTimeSpan WTime::operator-(const WTime& time) const			{ weak_assert(time.m_tm == m_tm); if (m_time != (INTNM::uint64_t)(-1)) return WTimeSpan(m_time - time.m_time, false); return WTimeSpan(-1, false); }
bool WTime::operator<(const WTime &time) const				{ weak_assert(time.m_tm == m_tm); if (m_time != (INTNM::uint64_t)(-1)) return (m_time < time.m_time); return false; }
bool WTime::operator>(const WTime &time) const				{ weak_assert(time.m_tm == m_tm); if (m_time != (INTNM::uint64_t)(-1)) return (m_time > time.m_time); return false; }
bool WTime::operator<=(const WTime &time) const				{ weak_assert(time.m_tm == m_tm); if (m_time != (INTNM::uint64_t)(-1)) return (m_time <= time.m_time); return false; }
bool WTime::operator>=(const WTime &time) const				{ weak_assert(time.m_tm == m_tm); if (m_time != (INTNM::uint64_t)(-1)) return (m_time >= time.m_time); return false; }

const char *WTimeManager::months_abbrev[12]		= { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
const char *WTimeManager::months[12]			= { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };
const char *WTimeManager::days_abbrev[7]		= { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
const char *WTimeManager::days[7]				= { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };


std::string WTime::ToString(INTNM::uint32_t flags) const {
	std::string str;
	char buff[128];
	std::string month_str, day_str;
	INTNM::int32_t year, month, day, hour, minute, second, usecs, day_of_week;
	bool need_leading_space;

	weak_assert(m_time != (INTNM::uint64_t)(-1000000));
	if (/*(!m_time) ||*/ (m_time == (INTNM::uint64_t)-1)) {
		str = "[Time Not Set]";
		return str;
	}

	year = GetYear(flags);
	month = GetMonth(flags);
	day = GetDay(flags);
	hour = GetHour(flags);
	minute = GetMinute(flags);
	second = GetSecond(flags);
	usecs = GetMicroSeconds(flags);

	day_of_week = GetDayOfWeek(flags);

	if (flags & WTIME_FORMAT_ABBREV) {		// decide if we should be abbreviating things or not
		month_str = WTimeManager::months_abbrev[month - 1];
		day_str = WTimeManager::days_abbrev[--day_of_week];
	} else {
		month_str = WTimeManager::months[month - 1];
		day_str = WTimeManager::days[--day_of_week];
	}

	if (flags & WTIME_FORMAT_DAY_OF_WEEK) {	// first, try to prepend the day-of-week to the string
		str = day_str;
		need_leading_space = true;
	} else	need_leading_space = false;

	if (flags & WTIME_FORMAT_DATE) {
		if (need_leading_space)	str += " ";
		else			need_leading_space = true;

		if ((flags & 0x000000ff) == WTIME_FORMAT_STRING_DD_MM_YYYY) {
			tm_snprintf(buff, 128, "%02d/%02d/%04d", day, month, year);
			str += buff;
		} else if ((flags & 0x000000ff) == WTIME_FORMAT_STRING_YYYY_MM_DD) {
			tm_snprintf(buff, 128, "%04d/%02d/%02d", year, month, day);
			str += buff;
		} else if ((flags & 0x000000ff) == WTIME_FORMAT_STRING_MM_DD_YYYY) {
			tm_snprintf(buff, 128, "%02d/%02d/%04d", month, day, year);
			str += buff;
		} else if ((flags & 0x000000ff) == WTIME_FORMAT_STRING_DDhMMhYYYY) {
			tm_snprintf(buff, 128, "%02d-%02d-%04d", day, month, year);
			str += buff;
		} else if (((flags & 0x000000ff) == WTIME_FORMAT_STRING_YYYYhMMhDD) || ((flags & 0x000000ff) == WTIME_FORMAT_STRING_YYYYhMMhDDT)) {
			tm_snprintf(buff, 128, "%04d-%02d-%02d", year, month, day);
			str += buff;
		} else if ((flags & 0x000000ff) == WTIME_FORMAT_STRING_MMhDDhYYYY) {
			tm_snprintf(buff, 128, "%02d-%02d-%04d", month, day, year);
			str += buff;
		} else if (((flags & 0x000000ff) == WTIME_FORMAT_STRING_YYYYMMDD) || ((flags & 0x000000ff) == WTIME_FORMAT_STRING_YYYYMMDDT)) {
			tm_snprintf(buff, 128, "%04d%02d%02d", year, month, day);
			str += buff;
		} else if ((flags & 0x000000ff) == WTIME_FORMAT_STRING_YYYYMMDDHH) {
			tm_snprintf(buff, 128, "%04d%02d%02d%02d", year, month, day, hour);
			str += buff;
		} else if (flags & WTIME_FORMAT_MONTH) {	// next, try to append date as month, day
			if (flags & WTIME_FORMAT_DAY)
			{
					tm_snprintf(buff, 128, "%s %2d", month_str.c_str(), day);
			}
			else 	tm_snprintf(buff, 128, "%s", month_str.c_str());
			str += buff;

			if (flags & WTIME_FORMAT_YEAR) {	// ...and conditionally add in the year
				tm_snprintf(buff, 128, ", %d", year);
				str += buff;
			}
		} else if (flags & WTIME_FORMAT_DAY) {
			tm_snprintf(buff, 128, "%2d", day);
			str += buff;
		}
	}

	if ((flags & WTIME_FORMAT_TIME) || ((flags & WTIME_FORMAT_CONDITIONAL_TIME) && ((usecs) || (second) || (minute) || (hour)))) {			// finally, try to append the time
		if ((((flags & 0x000000ff) == WTIME_FORMAT_STRING_YYYYhMMhDDT) || ((flags & 0x000000ff) == WTIME_FORMAT_STRING_YYYYMMDDT)) && (flags & WTIME_FORMAT_DATE))
			str += "T";
		else if (need_leading_space)
			str += " ";
		if (flags & WTIME_FORMAT_EXCLUDE_SECONDS) {
			if (second >= 30)		// perform any rounding
				minute++;
			tm_snprintf(buff, 128, "%02d:%02d", hour, minute);
		} else {
			if (flags & WTIME_FORMAT_INCLUDE_USECS)
				tm_snprintf(buff, 128, "%02d:%02d:%02d.%06d", hour, minute, second, usecs);
			else
				tm_snprintf(buff, 128, "%02d:%02d:%02d", hour, minute, second);
		}
		str += buff;
	}

	if (((flags & WTIME_FORMAT_STRING_TIMEZONE)) && (m_tm))
	{
		int64_t offset = m_tm->m_worldLocation.m_timezone().GetTotalMinutes();
		if (m_tm->m_worldLocation.m_startDST() != m_tm->m_worldLocation.m_endDST())
		{
			uint64_t intoYear = GetSecondsIntoYear(0);
			if (m_tm->m_worldLocation.m_startDST() < m_tm->m_worldLocation.m_endDST())
			{
				if (((uint64_t)m_tm->m_worldLocation.m_startDST().GetTotalSeconds() <= intoYear) &&
					(intoYear < (uint64_t)m_tm->m_worldLocation.m_endDST().GetTotalSeconds()))
					offset += m_tm->m_worldLocation.m_amtDST().GetTotalMinutes();
			}
			else
			{
				if (((uint64_t)m_tm->m_worldLocation.m_startDST().GetTotalSeconds() < intoYear) ||
					(intoYear <= (uint64_t)m_tm->m_worldLocation.m_endDST().GetTotalSeconds()))
					offset += m_tm->m_worldLocation.m_amtDST().GetTotalMinutes();
			}
		}
		if (offset == 0)
			str += "Z";
		else
		{
			if (offset < 0)
			{
				str += "-";
				offset = -offset;
			}
			else
				str += "+";
			int hourOffset = 0;
			while (offset >= 60)
			{
				hourOffset++;
				offset -= 60;
			}
			tm_snprintf(buff, 128, "%02" PRId32 ":%02" PRId64, hourOffset, offset);
			str += buff;
		}
	}
	return str;
}


#if defined(TIMES_WINDOWS) && !defined(_NO_MFC)
bool WTime::systemParseDateTime(const TCHAR *lpszDate, INTNM::uint32_t flags) {
	DWORD ole_flags = LOCALE_NOUSEROVERRIDE;
	if (!(flags & WTIME_FORMAT_TIME))
		ole_flags |= VAR_DATEVALUEONLY;
	COleDateTime ole_date(2000, 1, 1, 1, 1, 1);
	bool success = ole_date.ParseDateTime(lpszDate, ole_flags) ? true : false;
	if (success) {
		WTime new_time(ole_date, GetTimeManager(), flags);
		*this = new_time;
	}
	return success;
}

bool WTime::ParseDateTime(const CString &lpszDate, INTNM::uint32_t flags)
{
	CT2CA inter(lpszDate);
	return ParseDateTime(std::string(inter), flags);
}
#endif


bool str2int(INTNM::int32_t &i, char const *s, INTNM::int32_t base = 0)
{
	char *end;
	INTNM::int32_t l;
	l = strtol(s, &end, base);
	if (*s == '\0' || *end != '\0')
		return false;
	i = l;
	return true;
}


bool WTime::ParseDateTime(const std::wstring &lpszDate, INTNM::uint32_t flags)
{
	auto converted = std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(lpszDate);
	return ParseDateTime(converted, flags);
}


bool WTime::ParseDateTime(const std::string &lpszDate, INTNM::uint32_t flags, WorldLocation* location) {

#if defined(TIMES_WINDOWS) && !defined(_NO_MFC)
	if (flags & WTIME_FORMAT_PARSE_USING_SYSTEM)
	{
#if _UNICODE
		std::wstring wideDate(lpszDate.begin(), lpszDate.end());
		return systemParseDateTime(wideDate.c_str(), flags);
#else //_UNICODE
		return systemParseDateTime(lpszDate.c_str(), flags);
#endif //_UNICODE
	}
#endif

	static const char delimit[] = "./\\:;-, \t";
	static const char delimit2[] = "./\\:;-, \tT";
	INTNM::int32_t year, month, day, hour, min, sec;
	INTNM::int32_t v1, v2, v3;
	INTNM::int16_t time_scan = 0;
	std::optional<INTNM::int64_t> secondOffset = std::nullopt;
	bool timezoneExists = false;

	bool delimit_found = false;
	while (delimit[time_scan]) {
		if (lpszDate.find(delimit[time_scan]) != std::string::npos) {
			delimit_found = true;
			break;
		}
		time_scan++;
	}
			// TIME_FORMAT_DAY is implied

	if (!delimit_found) {
			// some groups are providing a screwy "YYYYMMDDHH" format
		if (((flags & 0x000000ff) != WTIME_FORMAT_STRING_YYYYMMDD) &&
			((flags & 0x000000ff) != WTIME_FORMAT_STRING_YYYYMMDDT) &&
			((flags & 0x000000ff) != WTIME_FORMAT_STRING_YYYYMMDDHH))
															return false;
		if ((flags & 0x000000ff) == WTIME_FORMAT_STRING_YYYYMMDD)
			if (lpszDate.size() != 8)						return false;
		if ((flags & 0x000000ff) == WTIME_FORMAT_STRING_YYYYMMDDHH)
			if (lpszDate.size() != 10)						return false;
		std::string yr, mn, dy, hr;
		yr = lpszDate.substr(0, 4);
		mn = lpszDate.substr(4, 2);
		dy = lpszDate.substr(6, 2);
		if ((flags & 0x000000ff) == WTIME_FORMAT_STRING_YYYYMMDDHH)
			hr = lpszDate.substr(8, 2);
		bool rd = str2int(year, yr.c_str()); if (!rd)		return false;
		rd = str2int(month, mn.c_str()); if (!rd)			return false;
		rd = str2int(day, dy.c_str()); if (!rd)				return false;

		if ((flags & 0x000000ff) == WTIME_FORMAT_STRING_YYYYMMDDHH) {
			rd = str2int(hour, hr.c_str());
			if (!rd)										return false;
		} else	hour = 0;
		min = sec = 0;
	} else {
		const char *tok;
		char* next = nullptr;
		char* dateBuf = __strdup(lpszDate.c_str());

		tok = __strtok(dateBuf, delimit, &next);
		if (!tok) {
			free(dateBuf);
			return false;
		}
		short guess_month = 0;
		if (!isdigit(tok[0])) {
			if (((flags & 0x000000ff) != WTIME_FORMAT_STRING_MM_DD_YYYY) &&
				((flags & 0x000000ff) != WTIME_FORMAT_STRING_MMhDDhYYYY) &&
				((flags & 0x000000ff) != 0)) {
				free(dateBuf);
				return false;
			}
			for (v1 = 0; v1 < 12; v1++)
				if ((!__stricmp(WTimeManager::months[v1], tok)) || (!__stricmp(WTimeManager::months_abbrev[v1], tok)))
					break;
			if (v1 == 12) {
				free(dateBuf);
				return false;
			}
			v1++;
			guess_month = 1;
		} else {
			INTNM::int32_t rd = sscanf(tok, "%d", &v1); 
			if (rd != 1) {
				free(dateBuf);
				return false;
			}
		}

		tok = __strtok(NULL, delimit, &next);
		if (!tok) {
			free(dateBuf);
			return false;
		}
		if (!isdigit(tok[0])) {
			if (((flags & 0x000000ff) != WTIME_FORMAT_STRING_DD_MM_YYYY) &&
				((flags & 0x000000ff) != WTIME_FORMAT_STRING_YYYY_MM_DD) &&
				((flags & 0x000000ff) != WTIME_FORMAT_STRING_DDhMMhYYYY) &&
				((flags & 0x000000ff) != WTIME_FORMAT_STRING_YYYYhMMhDD) &&
				((flags & 0x000000ff) != WTIME_FORMAT_STRING_YYYYhMMhDDT) &&
				((flags & 0x000000ff) != 0)) {
				free(dateBuf);
				return false;
			}
			for (v2 = 0; v2 < 12; v2++)
				if ((!__stricmp(WTimeManager::months[v2], tok)) || (!__stricmp(WTimeManager::months_abbrev[v2], tok)))
					break;
			if (v2 == 12) {
				free(dateBuf);
				return false;
			}
			v2++;
			guess_month = 2;
		} else {
			INTNM::int32_t rd = sscanf(tok, "%d", &v2);
			if (rd != 1) {
				free(dateBuf);
				return false;
			}
		}

		if ((flags & 0x000000ff) == WTIME_FORMAT_STRING_YYYYhMMhDDT)
			tok = __strtok(NULL, delimit2, &next);
		else
			tok = __strtok(NULL, delimit, &next);
		if (!tok) {
			free(dateBuf);
			return false;
		}
		if (!isdigit(tok[0])) {
			free(dateBuf);
			return false;
		} else {
			INTNM::int32_t rd = sscanf(tok, "%d", &v3);
			if (rd != 1) {
				free(dateBuf);
				return false;
			}
		}

		if (!(flags & 0x000000ff)) {
			if (guess_month == 1) {
				if (v3 >= 32)
					flags |= WTIME_FORMAT_STRING_MM_DD_YYYY;
			} else if (guess_month == 2) {
				if (v3 >= 32)
					flags |= WTIME_FORMAT_STRING_DD_MM_YYYY;
				else if (v1 >= 32)
					flags |= WTIME_FORMAT_STRING_YYYY_MM_DD;
			} else {
				if (v1 >= 32)
					flags |= WTIME_FORMAT_STRING_YYYY_MM_DD;
				else if (v3 >= 32 && v2 > 12)
					flags |= WTIME_FORMAT_STRING_MM_DD_YYYY;
			}
		}

		if (((flags & 0x000000ff) == WTIME_FORMAT_STRING_DD_MM_YYYY) ||
			((flags & 0x000000ff) == WTIME_FORMAT_STRING_DDhMMhYYYY) ||
			(!(flags & 0x000000ff))) {
			day = v1;
			month = v2;
			year = v3;
		} else if (((flags & 0x000000ff) == WTIME_FORMAT_STRING_YYYY_MM_DD) ||
				   ((flags & 0x000000ff) == WTIME_FORMAT_STRING_YYYYhMMhDD) ||
				   ((flags & 0x000000ff) == WTIME_FORMAT_STRING_YYYYhMMhDDT)) {
			day = v3;
			month = v2;
			year = v1;
		} else if (((flags & 0x000000ff) == WTIME_FORMAT_STRING_MM_DD_YYYY) ||
			       ((flags & 0x000000ff) == WTIME_FORMAT_STRING_MMhDDhYYYY)) {
			day = v2;
			month = v1;
			year = v3;
		}

		if (day >= 32) {
			free(dateBuf);
			return false;
		}
		if ((month < 1) || (month > 12)) {
			free(dateBuf);
			return false;
		}
		if (year < 39)							year += 2000;
		else if ((year < 100) && (year > 60))	year += 1900;

		if (flags & WTIME_FORMAT_TIME) {
			bool negative = false;
			char* save = next;
			tok = __strtok(NULL, "", &next);
			if (tok != nullptr)
			{
				if (strchr(tok, 'Z'))
				{
					secondOffset = std::optional<INTNM::int64_t>{ 0 };
					tok = __strtok(NULL, "-+Z", &save);
				}
				else
				{
					timezoneExists = strchr(tok, '+') != nullptr || strchr(tok, '-') != nullptr;
					if (timezoneExists)
					{
						negative = strchr(tok, '-') != nullptr;
						tok = __strtok(NULL, "-+Z", &save);
						next = save;
					}
				}
				WTimeSpan ts(std::string(tok), &time_scan);
				if (!time_scan)
					hour = min = sec = 0;
				else {
					hour = ts.GetHours();
					min = ts.GetMinutes();
					sec = ts.GetSeconds();
				}

				if (timezoneExists)
				{
					tok = __strtok(NULL, "", &next);
					if (tok)
					{
						WTimeSpan ts(std::string(tok), &time_scan);
						if (time_scan)
						{
							if (negative)
								secondOffset = std::optional<INTNM::int64_t>{ -ts.GetTotalSeconds() };
							else
								secondOffset = std::optional<INTNM::int64_t>{ ts.GetTotalSeconds() };
						}
					}
				}
			}
			else
				hour = min = sec = 0;
		}
		else	
			hour = min = sec = 0;

		free(dateBuf);
	}

	if ((year >= 1600) && (year < 2900)) {
		WTime t(year, month, day, hour, min, sec, m_tm);
		if (secondOffset.has_value())
		{
			int64_t offset = m_tm->m_worldLocation.m_timezone().GetTotalSeconds();
			uint64_t intoYear = t.GetSecondsIntoYear(0);
			if (location)
			{
				location->m_timezone(WTimeSpan(secondOffset.value()));
				location->m_amtDST(WTimeSpan(0));
			}
			if (m_tm->m_worldLocation.m_startDST() != m_tm->m_worldLocation.m_endDST())
			{
				if (m_tm->m_worldLocation.m_startDST() < m_tm->m_worldLocation.m_endDST())
				{
					if (((uint64_t)m_tm->m_worldLocation.m_startDST().GetTotalSeconds() <= intoYear) &&
						(intoYear < (uint64_t)m_tm->m_worldLocation.m_endDST().GetTotalSeconds()))
						offset += m_tm->m_worldLocation.m_amtDST().GetTotalSeconds();
				}
				else
				{
					if (((uint64_t)m_tm->m_worldLocation.m_startDST().GetTotalSeconds() < intoYear) ||
						(intoYear <= (uint64_t)m_tm->m_worldLocation.m_endDST().GetTotalSeconds()))
						offset += m_tm->m_worldLocation.m_amtDST().GetTotalSeconds();
				}
			}
			offset -= secondOffset.value();
			if (offset)
				t += WTimeSpan(offset);
		}
		else if (timezoneExists)
		{
			if (location)
			{
				location->m_timezone(WTimeSpan(0));
				location->m_amtDST(WTimeSpan(0));
			}
		}

		INTNM::uint64_t atm = adjusted_tm_math(flags);

		m_time = t.m_time - (atm - m_time);
	}
	return true;
}


#if defined(TIMES_WINDOWS) && !defined(_NO_MFC)
CArchive& AFXAPI HSS_Time::operator<<(CArchive& ar, const WTimeSpan timeSpan)	{
	INTNM::uint64_t milli_id = 0x7ffeeffccffaaffd;
	return ar << milli_id << timeSpan.m_timeSpan;
}


CArchive& AFXAPI HSS_Time::operator>>(CArchive& ar, WTimeSpan& rtimeSpan) {
	CArchive &r(ar >> rtimeSpan.m_timeSpan);
	if (rtimeSpan.m_timeSpan == 0x7ffeeffccffaaffd)
		return ar >> rtimeSpan.m_timeSpan;
	rtimeSpan.m_timeSpan *= 1000000;
	return r;
}


CArchive& AFXAPI HSS_Time::operator<<(CArchive& ar, const WTime &time)	{
	INTNM::uint64_t milli_id = 0x7ffeeddccbbaa009;
	return ar << milli_id << time.m_time;
}


CArchive& AFXAPI HSS_Time::operator>>(CArchive& ar, WTime &rtime) {
	CArchive &r(ar >> rtime.m_time);
	if (rtime.m_time == 0x7ffeeddccbbaa009)
		return ar >> rtime.m_time;
	if (rtime.m_time != (INTNM::uint64_t)-1LL) {
		rtime.m_time *= 1000000;
		rtime.m_time += WTIME_1900;		// new offset to year 1600 (no point in going back much further)
	}
	return r;
}
#endif


INTNM::uint64_t WTime::updateSerializedULONGLONG(INTNM::uint64_t toUpdate) {
	if (toUpdate != (INTNM::uint64_t)-1LL) {
		toUpdate *= 1000000LL;
		toUpdate += WTIME_1900;		// new offset to year 1600 (no point in going back much further)
	}
	return toUpdate;
}
