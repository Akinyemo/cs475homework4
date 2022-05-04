
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include<fstream>

unsigned int seed = 0;

int	NowYear;		// 2022 - 2027
int	NowMonth;		// 0 - 11

float	NowPrecip;		// inches of rain per month
float	NowTemp;		// temperature this month
float	NowHeight;		// grain height in inches
int	NowNumDeer;		// number of deer in the current population
const float GRAIN_GROWS_PER_MONTH =		9.0;
const float ONE_DEER_EATS_PER_MONTH =		1.0;

const float AVG_PRECIP_PER_MONTH =		7.0;	// average
const float AMP_PRECIP_PER_MONTH =		6.0;	// plus or minus
const float RANDOM_PRECIP =			2.0;	// plus or minus noise

const float AVG_TEMP =				60.0;	// average
const float AMP_TEMP =				20.0;	// plus or minus
const float RANDOM_TEMP =			10.0;	// plus or minus noise

const float MIDTEMP =				40.0;
const float MIDPRECIP =				10.0;
//Units of grain growth are inches.
//Units of temperature are degrees Fahrenheit (°F).
//Units of precipitation are inches.


int main(int argc, char *argv[ ]){

omp_set_num_threads( 4 );	// same as # of sections
#pragma omp parallel sections
{
	#pragma omp section
	{
		Deer( );
	}

	#pragma omp section
	{
		Grain( );
	}

	#pragma omp section
	{
		Watcher( );
	}

	#pragma omp section
	{
		Human( );	// your own
	}
}       // implied barrier -- all functions must return in order
	// to allow any of them to get past here

}
