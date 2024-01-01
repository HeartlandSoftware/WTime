/**
 * WTimeSpanTest.java
 *
 * Copyright 2016-2024 Heartland Software Solutions Inc.
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
	
	@Test
	void testParse4() {
	    WTimeSpan span = new WTimeSpan("120:00:00");
	    
	    assertEquals(5, span.getDays());
	    assertEquals(0, span.getHours());
	    assertEquals(0, span.getMinutes());
	    assertEquals(0, span.getSeconds());
	    assertEquals(0, span.getMilliSeconds());
	}
    
    @Test
    void testFormat1() {
        WTimeSpan span = new WTimeSpan("120:00:00");
        
        assertEquals("5 days 00:00:00:00", span.toString(WTime.FORMAT_YEAR | WTime.FORMAT_DAY | WTime.FORMAT_INCLUDE_USECS));
    }
}
