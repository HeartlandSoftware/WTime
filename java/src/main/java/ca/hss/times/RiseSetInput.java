/**
 * RiseSetInput.java
 *
 * Copyright 2016-2019 Heartland Software Solutions Inc.
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
 * Holds information required to calculate
 * sunrise and sunset times.
 *
 */
class RiseSetInput {
	/**
	 * The latitude (west positive, in radians).
	 */
	public double latitude;
	/**
	 * The longitude (west positive, in radians).
	 */
	public double longitude;
	/**
	 * The year to calculate the sunrise/sunset for.
	 */
	public int year;
	/**
	 * The month to calculate the sunrise/sunset for (0 - January).
	 */
	public int month;
	/**
	 * The day of the month to calculate the sunrise/sunset for.
	 */
	public int day;
	/**
	 * The timezone's current offset from UTC (west is positive).
	 */
	public int timezone;
	/**
	 * If the timezone is currently in daylight savings time (adds and hour to the timezone offset).
	 */
	public boolean daytimeSaving;
}
