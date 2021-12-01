package ca.hss.times;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.TestInstance;
import org.junit.jupiter.api.TestInstance.Lifecycle;

@TestInstance(Lifecycle.PER_CLASS)
public class WTimeSpanTest {

	@Test
	void testParse1() {
		WTimeSpan span = new WTimeSpan("0:0:10.5");
		
		assertEquals(0, span.getDays());
		assertEquals(0, span.getHours());
		assertEquals(0, span.getMinutes());
		assertEquals(10, span.getSeconds());
		assertEquals(500000, span.getMicroSeconds());
	}
	
	@Test
	void testParse2() {
		WTimeSpan span = new WTimeSpan("3 days 2:45:0");
		
		assertEquals(3, span.getDays());
		assertEquals(2, span.getHours());
		assertEquals(45, span.getMinutes());
		assertEquals(0, span.getSeconds());
	}
	
	@Test
	void testParse3() {
		WTimeSpan span = new WTimeSpan("P3DT1H5M10.5S");
		
		assertEquals(3, span.getDays());
		assertEquals(1, span.getHours());
		assertEquals(5, span.getMinutes());
		assertEquals(10, span.getSeconds());
		assertEquals(500, span.getMilliSeconds());
	}
}
