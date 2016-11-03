AUTOMAKE_OPTIONS = foreign
lib_LTLIBRARIES = libtimes.la
libtimes_la_SOURCES = src/SunriseSunsetCalc.cpp \
					 src/Times.cpp \
					 src/TimezoneMapper.cpp \
					 src/worldlocation.cpp
libtimes_la_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/include -I$(top_srcdir)/include/internal
include_HEADERS = include/WTime.h
				  include/internal/Times.h \
				  include/internal/SunriseSunsetCalc.h \
				  include/internal/times_internal.h \
				  include/internal/worldlocation.h
bin_PROGRAMS = timetest
timetest_SOURCES = test.cpp
timetest_LDADD = -L. -ltimes
timetest_CPPFLAGS = -I$(top_srcdir)/include
dist_data_DATA = Times.vcxproj config.h

