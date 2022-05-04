
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
int	    NowNumDeer;		// number of deer in the current population
int     NowNumHunters;              //number of humans in current population

const float GRAIN_GROWS_PER_MONTH =		9.0;
const float ONE_DEER_EATS_PER_MONTH =		1.0;
const float ONE_HUNTER_EATS_PER_MONTH = 1.0;

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

float
SQR( float x )
{
        return x*x;
}

float
Ranf( unsigned int *seedp,  float low, float high )
{
        float r = (float) rand_r( seedp );              // 0 - RAND_MAX

        return(   low  +  r * ( high - low ) / (float)RAND_MAX   );
}


int
Ranf( unsigned int *seedp, int ilow, int ihigh )
{
        float low = (float)ilow;
        float high = (float)ihigh + 0.9999f;

        return (int)(  Ranf(seedp, low,high) );
}

void Deer(){
	while(NowYear<2028){
	    int nextNumDeer = NowNumDeer;
		int carryingCapacity = (int)( NowHeight );
		if( nextNumDeer < carryingCapacity )
        		nextNumDeer++;
		else if( nextNumDeer > carryingCapacity )
        		nextNumDeer--;

    	nextNumDeer = nextNumDeer - (int)(NowNumHunters * ONE_HUNTER_EATS_PER_MONTH);

		if( nextNumDeer < 0 )
        		nextNumDeer = 0;
		#pragma omp barrier

		NowNumDeer = nextNumDeer;
		#pragma omp barrier

		#pragma omp barrier
	}
}

void Grain(){
	float nextHeight = 0;
	while(NowYear<2028){
		float tempFactor = exp(-SQR((NowTemp-MIDTEMP)/10.));
		float precipFactor = exp(-SQR((NowPrecip-MIDPRECIP)/10.));
		nextHeight = NowHeight;
 		nextHeight += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
 		nextHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;
		if( nextHeight < 0. )
			nextHeight = 0.;
		#pragma omp barrier

		NowHeight = nextHeight;
		#pragma omp barrier

		#pragma omp barrier

	}

}

void Watcher(){
	while(NowYear<2028){
		#pragma omp barrier

		#pragma omp barrier
        //printf("Month:%d,Year:%d\n",NowMonth,NowYear);
		//printf("%d,%f,%f,%d,%d,%d\n",NowMonth,NowTemp,NowPrecip,NowNumDeer,NowHeight,NowNumHunters);
		//printf("Temp:%f,Precipitation:%f,Number of Deer:%d,Height of Grain:%f,Number of Hunters%d\n",NowTemp,NowPrecip,NowNumDeer,NowHeight,NowNumHunters);
		printf("%d,%f,%f,%d,%f,%d\n",NowMonth,NowTemp,NowPrecip,NowNumDeer,NowHeight,NowNumHunters);
        //printf("%d\n",NowNumHunters);
		float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );

		float temp = AVG_TEMP - AMP_TEMP * cos( ang );
		NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );

		float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
		NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );
		if( NowPrecip < 0. )
			NowPrecip = 0.;
		NowMonth++;
		if(NowMonth >= 12){
			NowYear++;
			NowMonth = 0;
		}
		#pragma omp barrier
	}
}

void Hunter(){
	while(NowYear<2028){
		int nextNumHunters = NowNumHunters;
		if( NowNumDeer > 2 && nextNumHunters < NowNumDeer)
        		nextNumHunters++;
		else
        		nextNumHunters--;

		if( nextNumHunters < 0 )
        		nextNumHunters = 0;
	    #pragma omp barrier
		NowNumHunters = nextNumHunters;
        //printf("%d\n",NowNumHunters);
		#pragma omp barrier

		#pragma omp barrier
	}
}


int main(int argc, char *argv[ ]){

omp_set_num_threads( 4 );	// same as # of sections
	NowMonth =    0;
	NowYear  = 2022;

	// starting state (feel free to change this if you want):
	NowNumDeer = 8;
	NowHeight =  10.;
	NowNumHunters = 1;
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
			Hunter( );	// your own
		}
	}       // implied barrier -- all functions must return in order
		// to allow any of them to get past here
		// compute a temporary next-value for this quantity
		// based on the current state of the simulation:
        return 0;
}
