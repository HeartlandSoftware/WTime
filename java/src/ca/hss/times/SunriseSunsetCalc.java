/**
 * SunriseSunsetCalc.java
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

import ca.hss.general.OutVariable;

import static ca.hss.math.general.*;

/**
 * Calculates sunrise and sunset times.
 *
 */
public class SunriseSunsetCalc {

	/**
	 * Flag to indicate that there was no sunrise on
	 * the requested day.
	 */
	public static final short NO_SUNRISE = 0x0001;
	/**
	 * Flag to indicate that there was no sunset on
	 * the requested day.
	 */
	public static final short NO_SUNSET = 0x0002;
	
	SunriseSunsetCalc() { }
	
	/**
	 * Calculate the sunrise, sunset, solar noon, equation of time, and solar declination
	 * for a given day.
	 * 
	 * @param latLongForm A set of input parameters for calculating the solar values.
	 * @param riseSetForm A set of output values containing the solar values.
	 * @return Returns a bit mask possibly containing SunriseSunsetCalc.NO_SUNRISE and SunriseSunsetCalc.NO_SUNSET to indicate
	 * that there is not sunrise or not sunset for the given day.
	 */
	public static short calcSun(RiseSetInput latLongForm, RiseSetOut riseSetForm) {
		short retval = 0;
		OutVariable<Integer> yearOut = new OutVariable<Integer>();
		OutVariable<Integer> monthOut = new OutVariable<Integer>();
		OutVariable<Integer> dayOut = new OutVariable<Integer>();
		OutVariable<Integer> hourOut = new OutVariable<Integer>();
		OutVariable<Integer> minOut = new OutVariable<Integer>();
		OutVariable<Integer> secOut = new OutVariable<Integer>();
		
		double latitude = latLongForm.Latitude;
		double longitude = latLongForm.Longitude;


		if (true)
		{
			if((latitude >= -90) && (latitude < -89.8))
			{
				latLongForm.Latitude = -89.8;
				latitude = -89.8;
			}
			if ((latitude <= 90) && (latitude > 89.8))
			{
				latLongForm.Latitude= 89.8;
				latitude = 89.8;
			}
			
			//*****	Calculate the time of sunrise			

			double JD = calcJD(latLongForm.year, latLongForm.month, latLongForm.day);

			double doy = calcDayOfYear(latLongForm.month, latLongForm.day, isLeapYear(latLongForm.year));
			double T = calcTimeJulianCent(JD);
			calcSunRtAscension(T);
			double theta = calcSunDeclination(T);
			double Etime = calcEquationOfTime(T);

			double eqTime = Etime;
			double solarDec = theta;

			riseSetForm.eqTime= (Math.floor(100*eqTime))/100;
			riseSetForm.solarDec = (Math.floor(100*(solarDec)))/100;

			// Calculate sunrise for this date
			// if no sunrise is found, set flag nosunrise

			boolean nosunrise = false;

			double riseTimeGMT = calcSunriseUTC(JD, latitude, longitude);
			if (riseTimeGMT==-9999)
			{
				nosunrise = true;
			}

			// Calculate sunset for this date
			// if no sunset is found, set flag nosunset

			boolean nosunset = false;
			double setTimeGMT = calcSunsetUTC(JD, latitude, longitude);
			if (setTimeGMT==-9999)
			{
				nosunset = true;
			}


			double daySavings=latLongForm.DaytimeSaving?60:0;
			int zone = latLongForm.timezone;
			if(zone > 12 || zone < -12.5)
			{

				zone = 0;
				latLongForm.timezone = zone;
			}

			if (!nosunrise)		// Sunrise was found
			{
				double riseTimeLST = riseTimeGMT - (60 * zone) + daySavings;	
					//	in minutes
				timeStringShortAMPM(riseTimeLST, JD, yearOut, monthOut, dayOut, hourOut, minOut, secOut);
				riseSetForm.YearRise=(Integer)yearOut.value;
				riseSetForm.MonthRise=(Integer)monthOut.value;
				riseSetForm.DayRise=(Integer)dayOut.value;
				riseSetForm.HourRise=(Integer)hourOut.value;
				riseSetForm.MinRise=(Integer)minOut.value;
				riseSetForm.SecRise=(Integer)secOut.value;

				timeStringDate(riseTimeGMT, JD, yearOut, monthOut, dayOut, hourOut, minOut, secOut);
			}

			if (!nosunset)		// Sunset was found
			{
				double setTimeLST = setTimeGMT - (60 * zone) + daySavings;
				timeStringShortAMPM(setTimeLST, JD, yearOut, monthOut, dayOut, hourOut, minOut, secOut);
				riseSetForm.YearSet=(Integer)yearOut.value;
				riseSetForm.MonthSet=(Integer)monthOut.value;
				riseSetForm.DaySet=(Integer)dayOut.value;
				riseSetForm.HourSet=(Integer)hourOut.value;
				riseSetForm.MinSet=(Integer)minOut.value;
				riseSetForm.SecSet=(Integer)secOut.value;
				timeStringDate(setTimeGMT, JD, yearOut, monthOut, dayOut, hourOut, minOut, secOut);

			}

			// Calculate solar noon for this date

			double solNoonGMT = calcSolNoonUTC(T, longitude);
			double solNoonLST = solNoonGMT - (60 * zone) + daySavings;

			timeString(solNoonLST, hourOut, minOut, secOut);
			riseSetForm.SolarNoonHour=(Integer)hourOut.value;
			riseSetForm.SolarNoonMin=(Integer)minOut.value;
			riseSetForm.SolarNoonSec=(Integer)secOut.value;

			timeString(solNoonGMT, hourOut, minOut, secOut);

			// report special cases of no sunrise

			if(nosunrise)
			{ 
				// if Northern hemisphere and spring or summer, OR  
				// if Southern hemisphere and fall or winter, use 
				// previous sunrise and next sunset

				if ( ((latitude > 66.4) && (doy > 79) && (doy < 267)) ||
				   ((latitude < -66.4) && ((doy < 83) || (doy > 263))) )
				{
					double newjd = findRecentSunrise(JD, latitude, longitude);
					double tempnewTime=calcSunriseUTC(newjd, latitude, longitude);
					double newtime = tempnewTime - (60 * zone) + daySavings;
					if (newtime > 1440)
					{
						newtime -= 1440;
						newjd += 1.0;
					}
					if (newtime < 0)
					{
						newtime += 1440;
						newjd -= 1.0;
					}

					timeStringAMPMDate(newtime, newjd, yearOut, monthOut, dayOut, hourOut, minOut, secOut);
					riseSetForm.YearRise=(Integer)yearOut.value;
					riseSetForm.MonthRise=(Integer)monthOut.value;
					riseSetForm.DayRise=(Integer)dayOut.value;
					riseSetForm.HourRise=(Integer)hourOut.value;
					riseSetForm.MinRise=(Integer)minOut.value;
					riseSetForm.SecRise=(Integer)secOut.value;

				}

				// if Northern hemisphere and fall or winter, OR 
				// if Southern hemisphere and spring or summer, use 
				// next sunrise and previous sunset

				else if ( ((latitude > 66.4) && ((doy < 83) || (doy > 263))) ||
					((latitude < -66.4) && (doy > 79) && (doy < 267)) )
				{
					double newjd = findNextSunrise(JD, latitude, longitude);
					double newtime = calcSunriseUTC(newjd, latitude, longitude)
						- (60 * zone) + daySavings;
					if (newtime > 1440)
					{
						newtime -= 1440;
						newjd += 1.0;
					}
					if (newtime < 0)
					{
						newtime += 1440;
						newjd -= 1.0;
					}

					timeStringAMPMDate(newtime, newjd, yearOut, monthOut, dayOut, hourOut, minOut, secOut);
					riseSetForm.YearRise=(Integer)yearOut.value;
					riseSetForm.MonthRise=(Integer)monthOut.value;
					riseSetForm.DayRise=(Integer)dayOut.value;
					riseSetForm.HourRise=(Integer)hourOut.value;
					riseSetForm.MinRise=(Integer)minOut.value;
					riseSetForm.SecRise=(Integer)secOut.value;

				}
				else 
				{
					retval |= NO_SUNRISE;
				}

			}

			if(nosunset)
			{ 

				if ( ((latitude > 66.4) && (doy > 79) && (doy < 267)) ||
				   ((latitude < -66.4) && ((doy < 83) || (doy > 263))) )
				{
					double newjd = findNextSunset(JD, latitude, longitude);
					double newtime = calcSunsetUTC(newjd, latitude, longitude)
						- (60 * zone) + daySavings;
					if (newtime > 1440)
					{
						newtime -= 1440;
						newjd += 1.0;
					}
					if (newtime < 0)
					{
						newtime += 1440;
						newjd -= 1.0;
					}

					timeStringAMPMDate(newtime, newjd, yearOut, monthOut, dayOut, hourOut, minOut, secOut);
					riseSetForm.YearSet=(Integer)yearOut.value;
					riseSetForm.MonthSet=(Integer)monthOut.value;
					riseSetForm.DaySet=(Integer)dayOut.value;
					riseSetForm.HourSet=(Integer)hourOut.value;
					riseSetForm.MinSet=(Integer)minOut.value;
					riseSetForm.SecSet=(Integer)secOut.value;

				}

				// if Northern hemisphere and fall or winter, OR
				// if Southern hemisphere and spring or summer, use 
				// next sunrise and last sunset

				else if ( ((latitude > 66.4) && ((doy < 83) || (doy > 263))) ||
					((latitude < -66.4) && (doy > 79) && (doy < 267)) )
				{
					double newjd = findRecentSunset(JD, latitude, longitude);
					double newtime = calcSunsetUTC(newjd, latitude, longitude)
						- (60 * zone) + daySavings;
					if (newtime > 1440)
					{
						newtime -= 1440;
						newjd += 1.0;
					}
					if (newtime < 0)
					{
						newtime += 1440;
						newjd -= 1.0;
					}
 
					timeStringAMPMDate(newtime, newjd, yearOut, monthOut, dayOut, hourOut, minOut, secOut);
					riseSetForm.YearSet=(Integer)yearOut.value;
					riseSetForm.MonthSet=(Integer)monthOut.value;
					riseSetForm.DaySet=(Integer)dayOut.value;
					riseSetForm.HourSet=(Integer)hourOut.value;
					riseSetForm.MinSet=(Integer)minOut.value;
					riseSetForm.SecSet=(Integer)secOut.value;

				}

				else 
				{
					retval |= NO_SUNSET;
				}
			}
		}
		return retval;
	}
	
	/**
	 * Is the given year a leap year.
	 * 
	 * @param year The year to compute for.
	 * @return true if the given year is a leap year.
	 */
	static public boolean isLeapYear(int year) {
		return ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0);
	}
	
	private static double radToDeg(double angleRad) {
		return RADIAN_TO_DEGREE(angleRad);
	}
	
	private static double degToRad(double angleDeg) {
		return DEGREE_TO_RADIAN(angleDeg);
	}
	
	private static int calcDayOfYear(int mn, int dy, boolean lpyr) {
		double k = (lpyr ? 1 : 2);
		double doy = Math.floor((275.0 * mn)/9.0) - k * Math.floor((mn + 9.0)/12.0) + dy -30.0;
		return (int)doy;
	}
	
	@SuppressWarnings("unused")
	private static String calcDayOfWeek(double juld, OutVariable<Short> day) {
		int A = ((int)(juld + 1.5)) % 7;
		String DOW = (A==0)?"Sunday":(A==1)?"Monday":(A==2)?"Tuesday":(A==3)?"Wednesday":(A==4)?"Thursday":(A==5)?"Friday":"Saturday";
		day.value = (short)A;
		return DOW;
	}
	
	private static double calcJD(int year, int month, int day) {
		if (month <= 2) 
		{
			year -= 1;
			month += 12;
		}
		double A = Math.floor(year/100.0);
		double B = 2 - A + Math.floor(A/4.0);

		double JD = Math.floor(365.25*(year + 4716)) + Math.floor(30.6001*(month+1)) + day + B - 1524.5;
		return JD;
	}

	private static void calcDayFromJD(double jd, OutVariable<Integer> yearOut, OutVariable<Integer> monthOut, OutVariable<Integer> dayOut) {
		double z = Math.floor(jd + 0.5);
		double f = (jd + 0.5) - z;

		double A,B,C,D,E;

		double alpha;

		if (z < 2299161) 
		{
			A = z;
		} 
		else 
		{
			alpha = Math.floor((z - 1867216.25)/36524.25);
			A = z + 1 + alpha - Math.floor(alpha/4.0);
		}

		B = A + 1524;
		C = Math.floor((B - 122.1)/365.25);
		D = Math.floor(365.25 * C);
		E = Math.floor((B - D)/30.6001);

		int day = (int)(B - D - Math.floor(30.6001 * E) + f);
		int month =(int)( (E < 14) ? E - 1 : E - 13 );
		int year = (int)( (month > 2) ? C - 4716 : C - 4715);

		yearOut.value =year;
		monthOut.value =month;
		dayOut.value =day;
	}
	
	private static double calcTimeJulianCent(double jd) {
		double T = (jd - 2451545.0)/36525.0;
		return T;
	}
	
	private static double calcJDFromJulianCent(double t) {
		double JD = t * 36525.0 + 2451545.0;
		return JD;
	}
	
	private static double calcGeomMeanLongSun(double t) {
		double L0 = 280.46646 + t * (36000.76983 + 0.0003032 * t);
		while(L0 > 360.0)
		{
			L0 -= 360.0;
		}
		while(L0 < 0.0)
		{
			L0 += 360.0;
		}
		return L0;		// in degrees
	}
	
	private static double calcGeomMeanAnomalySun(double t) {
		double M = 357.52911 + t * (35999.05029 - 0.0001537 * t);
		return M;		// in degrees
	}
	
	private static double calcEccentricityEarthOrbit(double t) {
		double e = 0.016708634 - t * (0.000042037 + 0.0000001267 * t);
		return e;		// unitless
	}
	
	private static double calcSunEqOfCenter(double t) {
		double m = calcGeomMeanAnomalySun(t);

		double mrad = degToRad(m);
		double sinm = Math.sin(mrad);
		double sin2m = Math.sin(mrad+mrad);
		double sin3m = Math.sin(mrad+mrad+mrad);

		double C = sinm * (1.914602 - t * (0.004817 + 0.000014 * t)) + sin2m * (0.019993 - 0.000101 * t) + sin3m * 0.000289;
		return C;		// in degrees
	}
	
	private static double calcSunTrueLong(double t) {
		double l0 = calcGeomMeanLongSun(t);
		double c = calcSunEqOfCenter(t);

		double O = l0 + c;
		return O;		// in degrees
	}
	
	private static double calcSunTrueAnomaly(double t) {
		double m = calcGeomMeanAnomalySun(t);
		double c = calcSunEqOfCenter(t);

		double v = m + c;
		return v;		// in degrees
	}
	
	@SuppressWarnings("unused")
	private static double calcSunRadVector(double t) {
		double v = calcSunTrueAnomaly(t);
		double e = calcEccentricityEarthOrbit(t);

		double R = (1.000001018 * (1 - e * e)) / (1 + e * Math.cos(degToRad(v)));
		return R;		// in AUs
	}
	
	private static double calcSunApparentLong(double t) {
		double o = calcSunTrueLong(t);

		double omega = 125.04 - 1934.136 * t;
		double lambda = o - 0.00569 - 0.00478 * Math.sin(degToRad(omega));
		return lambda;		// in degrees
	}
	
	private static double calcMeanObliquityOfEcliptic(double t) {
		double seconds = 21.448 - t*(46.8150 + t*(0.00059 - t*(0.001813)));
		double e0 = 23.0 + (26.0 + (seconds/60.0))/60.0;
		return e0;		// in degrees
	}
	
	private static double calcObliquityCorrection(double t) {
		double e0 = calcMeanObliquityOfEcliptic(t);

		double omega = 125.04 - 1934.136 * t;
		double e = e0 + 0.00256 * Math.cos(degToRad(omega));
		return e;		// in degrees
	}
	
	private static double calcSunRtAscension(double t) {
		double e = calcObliquityCorrection(t);
		double lambda = calcSunApparentLong(t);

		double tananum = (Math.cos(degToRad(e)) * Math.sin(degToRad(lambda)));
		double tanadenom = (Math.cos(degToRad(lambda)));
		double alpha = radToDeg(Math.atan2(tananum, tanadenom));
		return alpha;		// in degrees
	}
	
	private static double calcSunDeclination(double t) {
		double e = calcObliquityCorrection(t);
		double lambda = calcSunApparentLong(t);

		double sint = Math.sin(degToRad(e)) * Math.sin(degToRad(lambda));
		double theta = radToDeg(Math.asin(sint));
		return theta;		// in degrees
	}
	
	private static double calcEquationOfTime(double t) {
		double epsilon = calcObliquityCorrection(t);
		double l0 = calcGeomMeanLongSun(t);
		double e = calcEccentricityEarthOrbit(t);
		double m = calcGeomMeanAnomalySun(t);

		if(epsilon==180)
			return -9999;
		double y = Math.tan(degToRad(epsilon)/2.0);
		y *= y;

		double sin2l0 = Math.sin(2.0 * degToRad(l0));
		double sinm   = Math.sin(degToRad(m));
		double cos2l0 = Math.cos(2.0 * degToRad(l0));
		double sin4l0 = Math.sin(4.0 * degToRad(l0));
		double sin2m  = Math.sin(2.0 * degToRad(m));

		double Etime = y * sin2l0 - 2.0 * e * sinm + 4.0 * e * y * sinm * cos2l0
				- 0.5 * y * y * sin4l0 - 1.25 * e * e * sin2m;

		return radToDeg(Etime)*4.0;	// in minutes of time
	}
	
	private static double calcHourAngleSunrise(double lat, double solarDec) {
		double latRad = degToRad(lat);
		double sdRad  = degToRad(solarDec);

		double HAarg = (Math.cos(degToRad(90.833))/(Math.cos(latRad)*Math.cos(sdRad))-Math.tan(latRad) * Math.tan(sdRad));

		if(Math.abs(HAarg)>1)
			return -9999;
		double divider;
		divider=(Math.cos(latRad)*Math.cos(sdRad));
		if(Math.abs(divider)<0.0000001)
			return -9999;
		divider=Math.cos(degToRad(90.833))/(Math.cos(latRad)*Math.cos(sdRad))-Math.tan(latRad) * Math.tan(sdRad);
		if(Math.abs(divider)>1)
			return -9999;
		double HA = (Math.acos(divider));

		return HA;		// in radians
	}
	
	private static double calcHourAngleSunset(double lat, double solarDec) {
		double latRad = degToRad(lat);
		double sdRad  = degToRad(solarDec);

		double HAarg = (Math.cos(degToRad(90.833))/(Math.cos(latRad)*Math.cos(sdRad))-Math.tan(latRad) * Math.tan(sdRad));

		if(Math.abs(HAarg)>1)
			return -9999;
		double divider=Math.cos(degToRad(90.833))/(Math.cos(latRad)*Math.cos(sdRad))-Math.tan(latRad) * Math.tan(sdRad);
		if(Math.abs(divider)>1)
			return -9999;
		double HA = (Math.acos(divider));

		return -HA;		// in radians
	}
	
	private static double calcSunriseUTC(double JD, double latitude, double longitude) {
		double t = calcTimeJulianCent(JD);

		// *** First pass to approximate sunrise

		double eqTime = calcEquationOfTime(t);
		if(eqTime==-9999)
			return -9999;
		double solarDec = calcSunDeclination(t);
		double hourAngle = calcHourAngleSunrise(latitude, solarDec);
		if(hourAngle==-9999)
			return -9999;

		double delta = longitude - radToDeg(hourAngle);
		double timeDiff = 4 * delta;	// in minutes of time
		double timeUTC = 720 + timeDiff - eqTime;	// in minutes

		// alert("eqTime = " + eqTime + "\nsolarDec = " + solarDec + "\ntimeUTC = " + timeUTC);

		// *** Second pass includes fractional jday in gamma calc

		double newt = calcTimeJulianCent(calcJDFromJulianCent(t) + timeUTC/1440.0); 
		eqTime = calcEquationOfTime(newt);
		if(eqTime==-9999)
			return -9999;
		solarDec = calcSunDeclination(newt);
		hourAngle = calcHourAngleSunrise(latitude, solarDec);
		if(hourAngle==-9999)
			return -9999;
		delta = longitude - radToDeg(hourAngle);
		timeDiff = 4 * delta;
		timeUTC = 720 + timeDiff - eqTime; // in minutes

		// alert("eqTime = " + eqTime + "\nsolarDec = " + solarDec + "\ntimeUTC = " + timeUTC);

		return timeUTC;
	}
	
	private static double calcSolNoonUTC(double t, double longitude) {
		/*double newt =*/ calcTimeJulianCent(calcJDFromJulianCent(t) + 0.5 + longitude/360.0); 

		double eqTime = calcEquationOfTime(t);
		/*double solarNoonDec =*/ calcSunDeclination(t);
		double solNoonUTC = 720 + (longitude * 4) - eqTime; // min
		
		return solNoonUTC;
	}
	
	private static double calcSunsetUTC(double JD, double latitude, double longitude) {
		double t = calcTimeJulianCent(JD);

		// First calculates sunrise and approx length of day

		double eqTime = calcEquationOfTime(t);
		if(eqTime==-9999)
			return -9999;
		double solarDec = calcSunDeclination(t);
		double hourAngle = calcHourAngleSunset(latitude, solarDec);

		if(hourAngle==-9999)
			return -9999;
		double delta = longitude - radToDeg(hourAngle);
		double timeDiff = 4 * delta;
		double timeUTC = 720 + timeDiff - eqTime;

		// first pass used to include fractional day in gamma calc

		double newt = calcTimeJulianCent(calcJDFromJulianCent(t) + timeUTC/1440.0); 
		eqTime = calcEquationOfTime(newt);
		if(eqTime==-9999)
			return -9999;
		solarDec = calcSunDeclination(newt);
		hourAngle = calcHourAngleSunset(latitude, solarDec);
		if(hourAngle==-9999)
			return -9999;

		delta = longitude - radToDeg(hourAngle);
		timeDiff = 4 * delta;
		timeUTC = 720 + timeDiff - eqTime; // in minutes

		return timeUTC;
	}
	
	private static double findRecentSunrise(double jd, double latitude, double longitude) {
		double julianday = jd;

		double time = calcSunriseUTC(julianday, latitude, longitude);
		while(time==-9999)//!isNumber(time))
		{
			julianday -= 1.0;
			time = calcSunriseUTC(julianday, latitude, longitude);
		}

		return julianday;
	}
	
	private static double findRecentSunset(double jd, double latitude, double longitude) {
		double julianday = jd;

		double time = calcSunsetUTC(julianday, latitude, longitude);
		while(time==-9999)//!isNumber(time))
		{
			julianday -= 1.0;
			time = calcSunsetUTC(julianday, latitude, longitude);
		}

		return julianday;
	}
	
	private static double findNextSunrise(double jd, double latitude, double longitude) {
		double julianday = jd;

		double time = calcSunriseUTC(julianday, latitude, longitude);
		while(time==-9999)//!isNumber(time))
		{
			julianday += 1.0;
			time = calcSunriseUTC(julianday, latitude, longitude);
		}

		return julianday;
	}
	
	private static double findNextSunset(double jd, double latitude, double longitude) {
		double julianday = jd;

		double time = calcSunsetUTC(julianday, latitude, longitude);
		while(time==-9999)//!isNumber(time))
		{
			julianday += 1.0;
			time = calcSunsetUTC(julianday, latitude, longitude);
		}

		return julianday;
	}
	
	private static void timeString(double minutes, OutVariable<Integer> hourOut, OutVariable<Integer> minOut, OutVariable<Integer> secOut) {
		double floatHour = minutes / 60;
		double hour = Math.floor(floatHour);
		double floatMinute = 60 * (floatHour - Math.floor(floatHour));
		double minute = Math.floor(floatMinute);
		double floatSec = 60 * (floatMinute - Math.floor(floatMinute));
		double second = Math.floor(floatSec);

		hourOut.value = (int)hour;
		minOut.value = (int)minute;
		secOut.value = (int)second;
	}
	
	private static void timeStringShortAMPM(double minutes, double JD, OutVariable<Integer> yearOut, OutVariable<Integer> monthOut, OutVariable<Integer> dayOut, OutVariable<Integer> hourOut, OutVariable<Integer> minOut, OutVariable<Integer> secOut) {
		double julianday = JD;
		double floatHour = minutes / 60;
		double hour = Math.floor(floatHour);
		double floatMinute = 60 * (floatHour - Math.floor(floatHour));
		double minute = Math.floor(floatMinute);
		double floatSec = 60 * (floatMinute - Math.floor(floatMinute));
		double second = Math.floor(floatSec);
		minute += (second >= 30)? 1 : 0;

		if (minute >= 60) 
		{
			minute -= 60;
			hour ++;
		}

		if (hour > 23) 
		{
			hour -= 24;
			julianday += 1.0;
		}

		if (hour < 0)
		{
			hour += 24;
			julianday -= 1.0;
		}

		hourOut.value = (int)hour;
		minOut.value = (int)minute;
		secOut.value = (int)second;

		if (hour > 11)
		{
			hour -= 12;
		}

		if (hour == 0)
		{
			hour = 12;
		}

		calcDayFromJD(julianday,yearOut,monthOut,dayOut);
	}
	
	private static void timeStringAMPMDate(double minutes, double JD, OutVariable<Integer> yearOut, OutVariable<Integer> monthOut, OutVariable<Integer> dayOut, OutVariable<Integer> hourOut, OutVariable<Integer> minOut, OutVariable<Integer> secOut) {
		double julianday = JD;
		double floatHour = minutes / 60;
		double hour = Math.floor(floatHour);
		double floatMinute = 60 * (floatHour - Math.floor(floatHour));
		double minute = Math.floor(floatMinute);
		double floatSec = 60 * (floatMinute - Math.floor(floatMinute));
		double second = Math.floor(floatSec);

		minute += (second >= 30)? 1 : 0;

		if (minute >= 60) 
		{
			minute -= 60;
			hour ++;
		}

		if (hour > 23) 
		{
			hour -= 24;
			julianday += 1.0;
		}

		if (hour < 0)
		{
			hour += 24;
			julianday -= 1.0;
		}

		hourOut.value = (int)hour;
		minOut.value = (int)minute;
		secOut.value = (int)second;

		if (hour > 11)
		{
			hour -= 12;
		}

		if (hour == 0)
		{
			hour = 12;
		}

		calcDayFromJD(julianday,yearOut,monthOut,dayOut);

	}
	
	private static void timeStringDate(double minutes, double JD, OutVariable<Integer> yearOut, OutVariable<Integer> monthOut, OutVariable<Integer> dayOut, OutVariable<Integer> hourOut, OutVariable<Integer> minOut, OutVariable<Integer> secOut) {
		double julianday = JD;
		double floatHour = minutes / 60;
		double hour = Math.floor(floatHour);
		double floatMinute = 60 * (floatHour - Math.floor(floatHour));
		double minute = Math.floor(floatMinute);
		double floatSec = 60 * (floatMinute - Math.floor(floatMinute));
		double second = Math.floor(floatSec);

		minute += (second >= 30)? 1 : 0;

		if (minute >= 60) 
		{
			minute -= 60;
			hour ++;
		}

		if (hour > 23) 
		{
			hour -= 24;
			julianday += 1.0;
		}

		if (hour < 0)
		{
			hour += 24;
			julianday -= 1.0;
		}

		hourOut.value = (int)hour;
		minOut.value = (int)minute;
		secOut.value = (int)second;

		calcDayFromJD(julianday,yearOut,monthOut,dayOut);
	}
}
