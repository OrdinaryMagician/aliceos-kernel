/*
	math.c : Math fuctions.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <math.h>
#include <berp.h>
/* undefined functions will result in a berp (panic) or whatever when called */
double sin( double ang )
{
	extern double fpu_sin(double);
	return fpu_sin(ang);
}
double cos( double ang )
{
	extern double fpu_cos(double);
	return fpu_cos(ang);
}
double tan( double ang )
{
	extern double fpu_tan(double);
	return fpu_tan(ang);
}
double asin( double ang )
{
	extern double fpu_asin(double);
	return fpu_asin(ang);
}
double acos( double ang )
{
	extern double fpu_acos(double);
	return fpu_acos(ang);
}
double atan( double ang )
{
	extern double fpu_atan(double);
	return fpu_atan(ang);
}
double log( double val )
{
	extern double fpu_log(double);
	return fpu_log(val);
}
double sqrt( double val )
{
	extern double fpu_sqrt(double);
	return fpu_sqrt(val);
}
double square( double val )
{
	return val*val;
}
double exp( double val )
{
	return pow(2,val);
}
double root( double val, double rdx )
{
	BERP("not implemented");
	return 0.0;
}
double pow( double val, double exp )
{
	double baseval = val;
	if ( exp < 0.0 )
		return 1.0/pow(val,exp);
	if ( exp == 0.0 )
		return 1.0;
	if ( decp(exp) != 0.0 )
		BERP("not implemented");
	while ( exp > 1.0 )
	{
		val *= baseval;
		exp -= 1.0;
	}
	return val;
}
double floor( double val )
{
	extern double fpu_floor(double);
	return fpu_floor(val);
}
double trunc( double val )
{
	extern double fpu_trunc(double);
	return fpu_trunc(val);
}
double ceil( double val )
{
	extern double fpu_ceil(double);
	return fpu_ceil(val);
}
double decp( double val )
{
	return val-floor(val);
}
