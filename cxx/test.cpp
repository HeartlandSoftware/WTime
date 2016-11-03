#include <WTime.h>

#include <stdio.h>

using namespace HSS_Time;

int main(int argc, char* argv[])
{
	WTime t(2016, 02, 10, 6, 0, 0, new WTimeManager(WorldLocation()));

	printf("Date %d %d %d\n", t.GetYear(WTIME_FORMAT_AS_LOCAL), t.GetMonth(WTIME_FORMAT_AS_LOCAL), t.GetDay(WTIME_FORMAT_AS_LOCAL));
	printf("Time %s\n", t.ToString(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_DATE | WTIME_FORMAT_TIME | WTIME_FORMAT_YEAR | WTIME_FORMAT_EXCLUDE_SECONDS | WTIME_FORMAT_ABBREV).c_str());

	WTime t2(t);
	if (!t2.ParseDateTime("2016021022", WTIME_FORMAT_STRING_YYYYMMDDHH))
		printf("Error parsing date\n");
	else
		printf("Time %s\n", t2.ToString(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_DATE | WTIME_FORMAT_TIME | WTIME_FORMAT_YEAR | WTIME_FORMAT_EXCLUDE_SECONDS | WTIME_FORMAT_ABBREV).c_str());
	return 0;
}
