/*
	math.c : Math fuctions.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <math.h>
#include <berp.h>

/*
   at the moment, pretty much everything is undefined and will result in
   a berp (panic) or whatever when called
*/

double sin( double ang )
{
	BERP("not implemented");
	return 0.0;
}

double cos( double ang )
{
	BERP("not implemented");
	return 0.0;
}

double tan( double ang )
{
	BERP("not implemented");
	return 0.0;
}

double asin( double ang )
{
	BERP("not implemented");
	return 0.0;
}

double acos( double ang )
{
	BERP("not implemented");
	return 0.0;
}

double atan( double ang )
{
	BERP("not implemented");
	return 0.0;
}

double log( double val )
{
	BERP("not implemented");
	return 0.0;
}

double sqrt( double val )
{
	BERP("not implemented");
	return 0.0;
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

double fact( double val )
{
	if ( val <= 1.0 )
		return 1.0;
	/* just trim decimals, I don't think anyone would mind */
	val = floor(val);
	return val*fact(val-1.0);
}

double floor( double val )
{
	/* really silly guesswork-based method */
	double test = val;
	while ( test >= 1.0 )
		test -= 1.0;
	while ( test < 0.0 )
		test += 1.0;
	return val-test;
}

double trunc( double val )
{
	return (decp(val)<0.5)?floor(val):ceil(val);
}

double ceil( double val )
{
	/* really silly guesswork-based method */
	double test = val;
	while ( test > 1.0 )
		test -= 1.0;
	while ( test <= 0.0 )
		test += 1.0;
	return val-test;
}

double decp( double val )
{
	return val-floor(val);
}
