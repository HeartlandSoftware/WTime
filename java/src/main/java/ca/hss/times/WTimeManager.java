/**
 * WTimeManager.java
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


package ca.hss.times;

import ca.hss.general.OutVariable;

/**
 * A class to manage WTimes.
 *
 */
public class WTimeManager {
	private WorldLocation m_worldLocation;
	
	/**
	 * Get the world location for this manager.
	 * @return
	 */
	public WorldLocation getWorldLocation() { return m_worldLocation; }

	/**
	 * Get a WTimeSpan of 1 second from TimeForIndex.
	 */
	public static final short ITERATION_1SEC		= 0;
	/**
	 * Get a WTimeSpan of 1 minute from TimeForIndex.
	 */
	public static final short ITERATION_1MIN		= 1;
	/**
	 * Get a WTimeSpan of 5 minutes from TimeForIndex.
	 */
	public static final short ITERATION_5MIN		= 2;
	/**
	 * Get a WTimeSpan of 15 minutes from TimeForIndex.
	 */
	public static final short ITERATION_15MIN		= 3;
	/**
	 * Get a WTimeSpan of 30 minutes from TimeForIndex.
	 */
	public static final short ITERATION_30MIN		= 4;
	/**
	 * Get a WTimeSpan of 1 hour from TimeForIndex.
	 */
	public static final short ITERATION_1HOUR		= 5;
	/**
	 * Get a WTimeSpan of 2 hours from TimeForIndex.
	 */
	public static final short ITERATION_2HOUR		= 6;
	/**
	 * Get a WTimeSpan of 1 day from TimeForIndex.
	 */
	public static final short ITERATION_1DAY		= 7;
	/**
	 * Get a WTimeSpan of 1 week from TimeForIndex.
	 */
	public static final short ITERATION_1WEEK		= 8;

	/**
	 * Construct a new time manager for the given location in the world.
	 * @param worldLocation
	 */
	public WTimeManager(WorldLocation worldLocation) {
		m_worldLocation = worldLocation;
	}

	/**
	 * Get a time span for a specific length of time.
	 * @param index one of the ITERATION_ constants
	 * @return
	 */
	public WTimeSpan timeForIndex(int index) {
		switch (index) {
		case ITERATION_1SEC:
			return new WTimeSpan(1);
		case ITERATION_1MIN:
			return new WTimeSpan(0, 0, 1, 0);
		case ITERATION_5MIN:
			return new WTimeSpan(0, 0, 5, 0);
		case ITERATION_15MIN:
			return new WTimeSpan(0, 0, 15, 0);
		case ITERATION_30MIN:
			return new WTimeSpan(0, 0, 30, 0);
		case ITERATION_1HOUR:
			return new WTimeSpan(0, 1, 0, 0);
		case ITERATION_2HOUR:
			return new WTimeSpan(0, 2, 0, 0);
		case ITERATION_1DAY:
			return new WTimeSpan(1, 0, 0, 0);
		}

		return new WTimeSpan(7, 0, 0, 0);
	}

	/**
	 * Convert a time span to one of the ITERAION_ constants.
	 * @param time
	 * @return
	 */
	public int iterationIndex(WTimeSpan time) {
		long iteration_amt[] = {
				1 * 1000000L,
				60 * 1000000L,
				60 * 5 * 1000000L,
				60 * 15 * 1000000L,
				60 * 30 * 1000000L,
				60 * 60 * 1000000L,
				60 * 60 * 2 * 1000000L,
				60 * 60 * 24 * 1000000L,
				60 * 60 * 24 * 7 * 1000000L };

			int i;
			for (i = 0; i < ITERATION_1WEEK; i++)
				if (iteration_amt[i] >= time.getTotalSeconds())
					break;
			return i;
	}

	/**
	 * Is the given year a leap year.
	 * @param year the year to check
	 * @return
	 */
	static public boolean isLeapYear(short year) {
		return SunriseSunsetCalc.isLeapYear(year);
	};

	/**
	 * Get the number of days in a given month for a given year.
	 * @param month the month to look up (0 - January)
	 * @param year the year to look up
	 * @return
	 */
	static public short daysInMonth(short month, short year) {
		short days_in_months[] = {
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

	/**
	 * Get the number of days in a given year.
	 * @param year the year to check
	 * @return 366 if the year is a leap year, 365 otherwise
	 */
	static public short getJulianCount(short year) {
		if (isLeapYear(year)) return 366; return 365;
	}

	static void normalize(OutVariable<Short> year, OutVariable<Short> julian) {
		short i;

		if (julian.value > 0) {
			i = getJulianCount(year.value);
			while (julian.value > i) {
				year.value++;
				julian.value = (short)(julian.value - i);
				i = getJulianCount(year.value);
			}
		}
		else {
			while (julian.value < 0) {
				year.value--;
				julian.value = (short)(julian.value + getJulianCount(year.value));
			}
		}
	}

	/**
	 * Get the julian day count for a given year, month, and day.
	 * @param year
	 * @param month
	 * @param day
	 * @return
	 */
	static public short toJulian(short year, short month, short day) {
		short i, calculated = 0;
		if (year < 70)
			year += 2000;
		else if (year < 100)
			year += 1900;
		while (year < 1600)
			year += 100;                                    // make sure our start time is appropriate for leap year calculations
		if (month > 0) {                                        // get months into range, adjust year accordingly to deal with leap years
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
		OutVariable<Short> y = new OutVariable<Short>();
		y.value = year;
		OutVariable<Short> d = new OutVariable<Short>();
		d.value = day;
		normalize(y, d);                                 // take care of obvious range checking for the days too
		year = y.value;
		day = d.value;
		for (i = 1; i < month; i++)
			day += daysInMonth(month, year);
		y.value = year;
		d.value = day;
		normalize(y, d);
		year = y.value;
		day = d.value;
		calculated = day;
																// NOTE if day or month is way out of range, we could have a slight error in calculation
		return calculated;                                      // due to getting the years correct for leap year calculation...oh well...
	}

	// converts month+day to julian (origin 0) for the provided year
	static public void fromJulian(OutVariable<Short> julian, OutVariable<Short> year, OutVariable<Short> month, OutVariable<Short> day) {
		if (year.value < 70)
			year.value = (short)(year.value + 2000);
		else if (year.value < 100)
			year.value = (short)(year.value + 1900);
		while (year.value < 1600)
			year.value = (short)(year.value + 100);
		normalize(year, julian);
		month.value = 1;
		while (julian.value >= daysInMonth(month.value, year.value))
			julian.value = (short)(julian.value - daysInMonth((short)((month.value)++), year.value));
		day.value = (short)(julian.value + 1);
	}
}
