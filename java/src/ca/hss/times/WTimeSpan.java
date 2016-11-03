/**
 * WTimeSpan.java
 *
 * Copyright 2016 Heartland Software Solutions Inc.
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


package ca.hss.times;

import java.io.Serializable;
import java.util.*;

import ca.hss.general.OutVariable;

import ca.hss.text.StringExtensions;
import sun.reflect.generics.reflectiveObjects.NotImplementedException;

/**
 * A class to represent a span of time.
 *
 */
public class WTimeSpan implements Comparable<WTimeSpan>, Serializable {
	public static final long serialVersionUID = 1;

	/**
	 * A time span of a day.
	 */
	public static final WTimeSpan Day = new WTimeSpan(1, 0, 0, 0);
	/**
	 * A time span of an hour.
	 */
	public static final WTimeSpan Hour = new WTimeSpan(0, 1, 0, 0);
	/**
	 * A time span of a minute.
	 */
	public static final WTimeSpan Minute = new WTimeSpan(0, 0, 1, 0);
	/**
	 * A time span of a second.
	 */
	public static final WTimeSpan Second = new WTimeSpan(0, 0, 0, 1);
	/**
	 * A time span of a microsecond.
	 */
	public static final WTimeSpan MicroSecond = new WTimeSpan(0, 0, 0, 0, 1);

	private long m_timeSpan;

	/**
	 * Construct a time span of 0 seconds.
	 */
	public WTimeSpan() {
		this(0, true);
	}

	/**
	 * Construct a time span of a given number of seconds.
	 * @param time
	 */
	public WTimeSpan(long time) {
		this(time, true);
	}

	/**
	 * Construct a time span with a given number of either seconds or microseconds.
	 * @param time
	 * @param units_are_seconds true if the span is in seconds, false if microseconds
	 */
	public WTimeSpan(long time, boolean units_are_seconds) {
		if (units_are_seconds)
			m_timeSpan = time * 1000000L;
		else
			m_timeSpan = time;
	}

	/**
	 * Construct a time span.
	 * @param lDays the number of days in the time span
	 * @param nHours the number of hours in the time span
	 * @param nMins the number of minutes in the time span
	 * @param nSecs the number of seconds in the time span
	 */
	public WTimeSpan(long lDays, long nHours, long nMins, long nSecs) {
		m_timeSpan = (nSecs + 60 * (nMins + 60 * (nHours + 24 * lDays))) * 1000000L;
	}

	/**
	 * Construct a time span.
	 * @param lDays the number of days in the time span
	 * @param nHours the number of hours in the time span
	 * @param nMins the number of minutes in the time span
	 * @param nSecs the number of seconds in the time span
	 * @param uSecs the number of microseconds in the time span
	 */
	public WTimeSpan(long lDays, long nHours, long nMins, long nSecs, long uSecs) {
	    m_timeSpan = (long)uSecs + ((long)(nSecs + 60 * (nMins + 60 * (nHours + 24 * lDays)))) * 1000000L;
	}

	/**
	 * Construct a time span from a string in the format HH:MM:ss.
	 * @param timeSrc
	 * @param cnt
	 */
	public WTimeSpan(String timeSrc, OutVariable<Short> cnt) {
		int hour, minute, read;
		double second;

		for (int i = 0; i < timeSrc.length(); i++) {
			char c = timeSrc.charAt(i);
			if (!Character.isDigit(c) && (c != ':') && (c != ' ') && (c != '-')) {
				if (cnt != null)
					cnt.value = 0;
				return;
			}
		}

		List<OutVariable<Object>> lst = new ArrayList<OutVariable<Object>>();
		lst.add(new OutVariable<Object>());
		lst.add(new OutVariable<Object>());
		lst.add(new OutVariable<Object>());
		read = StringExtensions._stscanf_s(timeSrc, "%d:%d:%lf", lst);
		hour = (Integer)lst.get(0).value;
		minute = (Integer)lst.get(1).value;
		second = (Double)lst.get(2).value;
		if (cnt != null)
			cnt.value = (short)read;
		switch (read) {
		case -1: if (cnt != null) cnt.value = 0;
		case 0: return;
		case 1: minute = 0;
		case 2: second = 0.0;
		}
		if (hour < 0) {
			minute = 0 - minute;
			second = 0 - second;
		}
		m_timeSpan = ((long)(60 * (minute + 60 * hour))) * 1000000L + (long)(second * 1000000.0);
	}

	/**
	 * Copy constructor.
	 * @param timeSrc the WTimeSpan to copy
	 */
	public WTimeSpan(WTimeSpan timeSrc) {
		m_timeSpan = timeSrc.m_timeSpan;
	}

	/**
	 * Not currently implemented.
	 * @param flags
	 * @return
	 */
	public String toString(short flags) {
		throw new NotImplementedException();
	}

	/**
	 * Update the time span in microseconds.
	 * @param timeSpan the new amount of microseconds
	 */
	public void setTimeSpan(long timeSpan) {
		this.m_timeSpan = timeSpan;
	}

	/**
	 * Get the number of microseconds in the time span.
	 * @return
	 */
	long getTimeSpan() {
		return m_timeSpan;
	}

	/**
	 * Get the number of years in the time span.
	 * @return
	 */
	public long GetYears() {
		return (long)((double)m_timeSpan / 1000000.0 / 24.0 / 60.0 / 60.0 / 365.25 + 0.75);
	}

	/**
	 * Get the number of days in the time span.
	 * @return
	 */
	public long GetDays() {
		return m_timeSpan / (24L * 60L * 60L * 1000000L);
	}

	/**
	 * Get the total number of hours in the time span.
	 * @return
	 */
	public long GetTotalHours() {
		return m_timeSpan / (60L * 60L * 1000000L);
	}

	/**
	 * Get the number of hours into the last day of the time span.
	 * @return
	 */
	public long GetHours() {
		return (long)(GetTotalHours() - GetDays() * 24);
	}

	/**
	 * Get the total number of minutes in the time span.
	 * @return
	 */
	public long GetTotalMinutes() {
		return m_timeSpan / (60L * 1000000L);
	}

	/**
	 * Get the number of minutes into the last hour of the time span.
	 * @return
	 */
	public long GetMinutes() {
		return (long)(GetTotalMinutes() - GetTotalHours() * 60);
	}

	/**
	 * Get the total number of seconds stored in the time span.
	 * @return
	 */
	public long GetTotalSeconds() {
		return m_timeSpan / 1000000L;
	}

	/**
	 * Get the number of seconds into the last minute of the time span.
	 * @return
	 */
	public long GetSeconds() {
		return (long)(GetTotalSeconds() - GetTotalMinutes() * 60);
	}

	/**
	 * Get the total number of microseconds in the time span.
	 * @return
	 */
	long GetTotalMicroSeconds() {
		return m_timeSpan;
	}

	/**
	 * Get the number of microseconds in the last second of the time span
	 * @return
	 */
	long GetMicroSeconds() {
		return m_timeSpan % 1000000L;
	}

	/**
	 * Get the fraction of days of a year are stored in this time span.
	 * @return
	 */
	double GetDaysFraction() {
		return ((double)m_timeSpan) / (24.0 * 60.0 * 60.0 * 1000000.0);
	}

	/**
	 * Get the fraction of a day stored in this time span.
	 * @return
	 */
	double GetFractionOfDay() {
		return ((double)(m_timeSpan % (24L * 60L * 60L * 1000000L))) / (24.0 * 60.0 * 60.0 * 1000000.0);
	}

	/**
	 * Purge all time steps smaller than seconds.
	 */
	void PurgeToSecond() {
		m_timeSpan = m_timeSpan - (m_timeSpan % (1000000L));
	}

	/**
	 * Purge all time steps smaller than minutes.
	 */
	public void PurgeToMinute() {
		m_timeSpan = m_timeSpan - (m_timeSpan % (60L * 1000000L));
	}

	/**
	 * Purge all time steps smaller than hours.
	 */
	public void PurgeToHour() {
		m_timeSpan = m_timeSpan - (m_timeSpan % (60L * 60L * 1000000L));
	}

	/**
	 * Purge all time steps smaller that days.
	 */
	public void PurgeToDay() {
		m_timeSpan = m_timeSpan - (m_timeSpan % (60L * 60L * 24L * 1000000L));
	}

	/**
	 * Compare this time span to another.
	 */
	@Override
	public int compareTo(WTimeSpan arg0) {
		if (m_timeSpan > arg0.m_timeSpan)
			return -1;
		else if (m_timeSpan == arg0.m_timeSpan)
			return 0;
		else
			return 1;
	}

	/**
	 * Convert the time span to a java.util.Date.
	 * @return
	 */
	public Date AsDateTimeSpan() {
		Date resultTimeSpan = new Date();
		resultTimeSpan.setTime(m_timeSpan);
		return resultTimeSpan;
	}

	/**
	 * Are two time spans equivalent.
	 * @param s1
	 * @param s2
	 * @return
	 */
	public static boolean Equal(WTimeSpan s1, WTimeSpan s2) {
		return s1.m_timeSpan == s2.m_timeSpan;
	}

	/**
	 * Are two time spans different.
	 * @param s1
	 * @param s2
	 * @return
	 */
	public static boolean NotEqual(WTimeSpan s1, WTimeSpan s2) {
		return s1.m_timeSpan != s2.m_timeSpan;
	}

	/**
	 * Is time span s1 greater than s2.
	 * @param s1
	 * @param s2
	 * @return
	 */
	public static boolean GreaterThan(WTimeSpan s1, WTimeSpan s2) {
		return s1.m_timeSpan > s2.m_timeSpan;
	}

	/**
	 * Is time span s1 greater than or equal to s2.
	 * @param s1
	 * @param s2
	 * @return
	 */
	public static boolean GreaterThanEqualTo(WTimeSpan s1, WTimeSpan s2) {
		return s1.m_timeSpan >= s2.m_timeSpan;
	}

	/**
	 * Is time span s1 less than s2.
	 * @param s1
	 * @param s2
	 * @return
	 */
	public static boolean LessThan(WTimeSpan s1, WTimeSpan s2) {
		return s1.m_timeSpan < s2.m_timeSpan;
	}

	/**
	 * Is time span s1 less than or equal to s2.
	 * @param s1
	 * @param s2
	 * @return
	 */
	public static boolean LessThanEqualTo(WTimeSpan s1, WTimeSpan s2) {
		return s1.m_timeSpan <= s2.m_timeSpan;
	}

	/**
	 * Multiply a time span by a value.
	 * @param span
	 * @param value
	 * @return
	 */
	public static WTimeSpan Multiply(WTimeSpan span, long value) {
		WTimeSpan ret = new WTimeSpan(span);
		ret.Multiply(value);
		return ret;
	}

	/**
	 * Divide a time span by a value.
	 * @param value
	 */
	public void Divide(long value) {
		m_timeSpan /= value;
	}

	/**
	 * Multiply this time span by a value.
	 * @param value
	 */
	public void Multiply(long value) {
		m_timeSpan *= value;
	}

	/**
	 * Subtract a time span from this one.
	 * @param span
	 */
	public void Subtract(WTimeSpan span) {
		m_timeSpan -= span.m_timeSpan;
	}
	
	@Override
	public String toString() {
		long h = GetHours();
		long m = GetMinutes();
		long s = GetSeconds();
		String retval = String.valueOf(h);
		if (m > 0 || s > 0) {
			retval += ":";
			retval += String.valueOf(m);
			if (s > 0) {
				retval += ":";
				retval += String.valueOf(s);
			}
		}
		return retval;
	}
}
