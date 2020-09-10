#include "Rgenerator.h"
#include <cmath>
#include <cstdlib>

CRgenerator::CRgenerator(void){}
CRgenerator::~CRgenerator(void){}

//"Polar" version without trigonometric calls
double CRgenerator::randn_notrig(double mu, double sigma){
	static bool deviateAvailable=false;	// flag
	static float storedDeviate;		// deviate from previous calculation
	double polar, rsquared, var1, var2;

	// If no deviate has been stored, the polar Box-Muller transformation is
	// performed, producing two independent normally-distributed random
	// deviates. One is stored for the next round, and one is returned.
	if (!deviateAvailable) {
		// Choose pairs of uniformly distributed deviates, discarding those
		// that don't fall within the unit circle
		do {
			var1=2.0*( double(rand())/double(RAND_MAX) ) / 1.0;
			var2=2.0*( double(rand())/double(RAND_MAX) ) / 1.0;
			rsquared=var1*var1+var2*var2;
		} while ( rsquared >= 1.0 || rsquared == 0.0);

		// Calculate polar transformation for each deviate
		polar=sqrt(-2.0*log(rsquared)/rsquared);

		// Store first deviate and set flag
		storedDeviate=var1*polar;
		deviateAvailable=true;

		// Return second deviate
		return var2*polar*sigma + mu;
	}

	// If a deviate is available from previous call to this function, it is
	// returned, and the flag is set to false.
	else {
		deviateAvailable=false;
		return storedDeviate*sigma + mu;
	}
}

#define PI 3.14159265358979323846

double CRgenerator::randn_trig(double mu, double sigma){
	static bool deviateAvailable=false;	// flag
	static float storedDeviate;		// deviate from previous calculation
	double dist, angle;

	// If no deviate has been stored, the standard Box-Muller transformation is
	// performed, producing two independent normally-distributed random
	// deviates. One is stored for the next round, and one is returned.
	if (!deviateAvailable) {
		// Choose a pair of uniformly distributed deviates, one for the
		// distance and one for the angle, and perform transformations
		dist=sqrt( -2.0 * log(double(rand()) / double(RAND_MAX)) );
		angle=2.0 * PI * (double(rand()) / double(RAND_MAX));

		// Calculate and store first deviate and set flag
		storedDeviate=dist*cos(angle);
		deviateAvailable=true;

		// Calculate return second deviate
		return dist * sin(angle) * sigma + mu;
	}

	// If a deviate is available from a previous call to this function, it is
	// returned, and the flag is set to false.
	else {
		deviateAvailable=false;
		return storedDeviate*sigma + mu;
	}
}
