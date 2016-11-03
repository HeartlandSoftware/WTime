/**
 * RiseSetOut.java
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

/**
 * Output from calculating the sunrise and sunset information.
 *
 */
class RiseSetOut {
	/**
	 * The year the sunrise took place in.
	 */
	public int YearRise;
	/**
	 * The month the sunrise took place in (0 - January).
	 */
	public int MonthRise;
	/**
	 * The day of the month the sunrise took place on.
	 */
	public int DayRise;
	/**
	 * The year the sunset took place in.
	 */
	public int YearSet;
	/**
	 * The month the sunset took place in (0 - January).
	 */
	public int MonthSet;
	/**
	 * The day of the month the sunset took place on.
	 */
	public int DaySet;
	/**
	 * The hour of the sunrise.
	 */
	public int HourRise;
	/**
	 * The minute of the sunrise.
	 */
	public int MinRise;
	/**
	 * The second of the sunrise.
	 */
	public int SecRise;
	/**
	 * The hour of the sunset.
	 */
	public int HourSet;
	/**
	 * The second of the sunset.
	 */
	public int MinSet;
	/**
	 * The second of the sunset.
	 */
	public int SecSet;
	/**
	 * The solar noon hour.
	 * 
	 * Solar noon is the time when the sun is the highest above the horizon.
	 */
	public int SolarNoonHour;
	/**
	 * The solar noon minute.
	 * 
	 * Solar noon is the time when the sun is the highest above the horizon.
	 */
	public int SolarNoonMin;
	/**
	 * The solar noon second.
	 * 
	 * Solar noon is the time when the sun is the highest above the horizon.
	 */
	public int SolarNoonSec;
	/**
	 * The equation of time for the given day.
	 */
	public double eqTime;
	/**
	 * The solar declination for the given day.
	 */
	public double solarDec;
}
