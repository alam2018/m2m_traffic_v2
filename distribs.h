
/*
 * distribs.h
 *
 *  Created on: 5 oct. 2012
 *      Author: avaret
 */

#ifndef DISTRIBS_H_
#define DISTRIBS_H_

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "gen_beta.h"

/* Number of ranges to compute the distribution printing */
#define PRINT_DISTRIB_COUNT_NB 100

/* getDistribution generates "numberTurns" random numbers, conforming to "distrib",
 * and store the result into "result".
 */
Pdistribdata getDistribution(unsigned int numberTurns, Tdistrib distrib);

/* printDistributions prints the distrib "result" and its parameter "distrib" on the screen */
void printDistributions(Tdistrib distrib, Pdistribdata result, int don, int printrawdata, int indexNo);

/* Allocate a struct for DistribData, with "number" items inside */
Pdistribdata malloc_distribdata(unsigned int number);

/* Free the memory associated with a distribution */
void free_distribdata(Pdistribdata d);

/* Convert a string into an unsigned integer (32 bits).
    string may be concluded with a suffixe like "k" and "M" to multiply by 1000 (kilo)... */
unsigned int stringToUInt(const char * s);

/* Convert a string into an myInteger (64 bits).
    string may be concluded with a suffixe like "k" and "M" to multiply by 1000 (kilo)... */
myInteger stringTomyInteger(const char * s, bool isUnit_Byte);


/*** internal ***/

/* Returns x if (min < x < max), else return min or max (nearest of x) */
myInteger bound_min_max(long double x, myInteger min, myInteger max);

/* Random generator of double numbers in range [0;1] */
long double drand(void);

/* Initialize the uniform random number generator with the new maximum */
void initialize_irand(myInteger max);

/* Random generator of integer, uniform distrib between [0..__max], see  initialize_irand() */
myInteger irand(void);

/* Polar form of the Box-Muller transformation */
long double get_random_gaussian(void);

/* Knuth 's algorithm, do NOT use for large lambda values */
int get_random_poisson(void);

long double get_random_weibull(void);
long double get_random_exp(void);
long double get_random_pareto(void);

/* Return a string like "Don" or "Doff", depending on the param */
char * DonOffStr(int don);

/* getDistributionFromFile reads the file given in parameter and returns the numbers into the resulting distribution. If numberTurns > number of lines of the file, the distribution will be completed with the value "0". If numberTurns < number of lines of the file, the file content will be truncated. If there is an error during the file reading, then all data will be 0. */
Pdistribdata getDistributionFromFile(unsigned int numberTurns, char * filename);

#endif /* DISTRIBS_H_ */
