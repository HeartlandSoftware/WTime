/**
 * RiseSetOut.java
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


package ca.hss.times;

/**
 * Output from calculating the sunrise and sunset information.
 *
 */
class RiseSetOut {
	/**
	 * The year the sunrise took place in.
	 */
	public int yearRise;
	/**
	 * The month the sunrise took place in (0 - January).
	 */
	public int monthRise;
	/**
	 * The day of the month the sunrise took place on.
	 */
	public int dayRise;
	/**
	 * The year the sunset took place in.
	 */
	public int yearSet;
	/**
	 * The month the sunset took place in (0 - January).
	 */
	public int monthSet;
	/**
	 * The day of the month the sunset took place on.
	 */
	public int daySet;
	/**
	 * The hour of the sunrise.
	 */
	public int hourRise;
	/**
	 * The minute of the sunrise.
	 */
	public int minRise;
	/**
	 * The second of the sunrise.
	 */
	public int secRise;
	/**
	 * The hour of the sunset.
	 */
	public int hourSet;
	/**
	 * The second of the sunset.
	 */
	public int minSet;
	/**
	 * The second of the sunset.
	 */
	public int secSet;
	/**
	 * The solar noon hour.
	 * 
	 * Solar noon is the time when the sun is the highest above the horizon.
	 */
	public int solarNoonHour;
	/**
	 * The solar noon minute.
	 * 
	 * Solar noon is the time when the sun is the highest above the horizon.
	 */
	public int solarNoonMin;
	/**
	 * The solar noon second.
	 * 
	 * Solar noon is the time when the sun is the highest above the horizon.
	 */
	public int solarNoonSec;
	/**
	 * The equation of time for the given day.
	 */
	public double eqTime;
	/**
	 * The solar declination for the given day.
	 */
	public double solarDec;
}
