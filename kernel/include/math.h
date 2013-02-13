/*
	math.h : Math fuctions.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef MATH_H
#define MATH_H
#include <sys/types.h>

#define M_PI 3.141592653589793
#define M_E 2.718281828459045
#define M_NAN (0.f/0.f)
#define M_INF 1e40f

double sin( double ang );
double cos( double ang );
double tan( double ang );
double asin( double ang );
double acos( double ang );
double atan( double ang );
double log( double val );
double sqrt( double val );
double square( double val );
double exp( double val );
double root( double val, double rdx );
double pow( double val, double exp );
double fact( double val );
double floor( double val );
double trunc( double val );
double ceil( double val );
double decp( double val );

#endif
