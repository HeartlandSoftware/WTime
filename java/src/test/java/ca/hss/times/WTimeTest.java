package ca.hss.times;

import static org.junit.jupiter.api.Assertions.*;

import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.TestInstance;
import org.junit.jupiter.api.TestInstance.Lifecycle;

@TestInstance(Lifecycle.PER_CLASS)
class WTimeTest {

	private WTimeManager winnipeg;
	private WTimeManager calgary;
	private WTimeManager utc;
	private WTimeManager india;
	
	@BeforeAll
	void initialize() {
		WorldLocation location = new WorldLocation();
		location.setLatitude(0.8708337756);
		location.setLongitude(-1.69538491);
		location.setTimezoneOffset(new WTimeSpan(0, -6, 0, 0));
		winnipeg = new WTimeManager(location);

		location = new WorldLocation();
		location.setLatitude(0.8909661485);
		location.setLongitude(-1.9909110404);
		location.setTimezoneOffset(new WTimeSpan(0, -7, 0, 0));
		calgary = new WTimeManager(location);

		location = new WorldLocation();
		location.setLatitude(0.2135986298);
		location.setLongitude(0.0272550616);
		location.setTimezoneOffset(new WTimeSpan(0, 0, 0, 0));
		utc = new WTimeManager(location);

		location = new WorldLocation();
		location.setLatitude(0.3594278702);
		location.setLongitude(1.378162592);
		location.setTimezoneOffset(new WTimeSpan(0, 5, 30, 0));
		india = new WTimeManager(location);
	}
	
	@Test
	void testToString1() {
		WTime time = new WTime(2018, 1, 20, 18, 31, 00, winnipeg);
		String str = time.toString(WTime.FORMAT_STRING_ISO8601);
		assertEquals("2018-01-20T12:31:00-06:00", str);
	}
	
	@Test
	void testToString2() {
		WTime time = new WTime(2018, 1, 20, 18, 31, 00, utc);
		String str = time.toString(WTime.FORMAT_STRING_ISO8601);
		assertEquals("2018-01-20T18:31:00Z", str);
	}

	@Test
	void testParseDate1() {
		WTime time = new WTime(winnipeg);
		time.parseDateTime("2018-01-20T12:31:00-06:00", WTime.FORMAT_STRING_ISO8601);
		String str = time.toString(WTime.FORMAT_STRING_ISO8601);
		assertEquals("2018-01-20T12:31:00-06:00", str);
	}

	@Test
	void testParseDate2() {
		WTime time = new WTime(calgary);
		time.parseDateTime("2018-01-20T12:31:00-06:00", WTime.FORMAT_STRING_ISO8601);
		String str = time.toString(WTime.FORMAT_STRING_ISO8601);
		assertEquals("2018-01-20T11:31:00-07:00", str);
	}

	@Test
	void testParseDate3() {
		WTime time = new WTime(utc);
		time.parseDateTime("2018-01-20T12:31:00-06:00", WTime.FORMAT_STRING_ISO8601);
		String str = time.toString(WTime.FORMAT_STRING_ISO8601);
		assertEquals("2018-01-20T18:31:00Z", str);
	}

	@Test
	void testParseDate4() {
		WTime time = new WTime(india);
		time.parseDateTime("2018-01-20T12:31:00-06:00", WTime.FORMAT_STRING_ISO8601);
		String str = time.toString(WTime.FORMAT_STRING_ISO8601);
		assertEquals("2018-01-21T00:01:00+05:30", str);
	}

	@Test
	void testParseDate5() {
		WTime time = new WTime(india);
		time.parseDateTime("2018-01-20T12:31:00Z", WTime.FORMAT_STRING_ISO8601);
		String str = time.toString(WTime.FORMAT_STRING_ISO8601);
		assertEquals("2018-01-20T18:01:00+05:30", str);
	}

	@Test
	void testParseDate6() {
		WTime time = new WTime(india);
		time.parseDateTime("2018-01-20T12:31:00", WTime.FORMAT_STRING_ISO8601);
		String str = time.toString(WTime.FORMAT_STRING_ISO8601);
		assertEquals("2018-01-20T12:31:00+05:30", str);
	}
	
	@Test
	void testParse5() {
		WTime time = new WTime(winnipeg);
		time.parseDateTime("2018-10-22", WTime.FORMAT_STRING_ISO8601);
		assertEquals(2018, time.getYear(WTime.FORMAT_AS_LOCAL));
		assertEquals(10, time.getMonth(WTime.FORMAT_AS_LOCAL));
		assertEquals(22, time.getDay(WTime.FORMAT_AS_LOCAL));
		assertEquals(0, time.getHour(WTime.FORMAT_AS_LOCAL));
		assertEquals(0, time.getMinute(WTime.FORMAT_AS_LOCAL));
	}
	
	@Test
	void testExport1() {
	    TimeZoneInfo info = WorldLocation.getTimeZoneFromId(131084);
        WorldLocation location = new WorldLocation();
        location.setLatitude(0.8708337756);
        location.setLongitude(-1.69538491);
        location.setTimezoneOffset(info);
        WTimeManager manager = new WTimeManager(location);
        WTime time = new WTime(manager);
        time.parseDateTime("2004-01-01T00:00:00.2319662-06:00", WTime.FORMAT_STRING_ISO8601);
        
        assertEquals(2004, time.getYear(WTime.FORMAT_AS_LOCAL | WTime.FORMAT_WITHDST));
        assertEquals(1, time.getMonth(WTime.FORMAT_AS_LOCAL | WTime.FORMAT_WITHDST));
        assertEquals(1, time.getDay(WTime.FORMAT_AS_LOCAL | WTime.FORMAT_WITHDST));
        assertEquals(0, time.getHour(WTime.FORMAT_AS_LOCAL | WTime.FORMAT_WITHDST));
        assertEquals(0, time.getMinute(WTime.FORMAT_AS_LOCAL | WTime.FORMAT_WITHDST));
	}
}
