#include "Canada.cpp"

bool insideCanadaDetail(Canada **canada, const double latitude, const double longitude) {
	if (!(*canada))
		*canada = new Canada();
	return (*canada)->Inside(latitude, longitude);
}


void insideCanadaCleanup(Canada *canada) {
	if (canada)
		delete canada;
}