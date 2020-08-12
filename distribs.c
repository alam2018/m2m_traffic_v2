/*
 * distribs.c
 *
 *  Created on: Feb 12, 2020
 *      Author: mdaa
 */

#include "unistd.h"
#include "distribs.h"
#include "tx_rx.h"
#include "math.h"

extern int packetSizeDistrib;
extern int elapseTime;

/* FIXME Poisson's distrib is badly implemented */
#define Coeff_Poisson_adjust 10.0
#define BOUND_RESEARCH_TRIES 1000

long double __lambda, __k, __alpha, __xm, __beta; /* local copies to speed calculus */

/* Returns x if (min < x < max), else return min or max (nearest of x) */
myInteger bound_min_max(long double x, myInteger min, myInteger max)
{
	myInteger y = (myInteger) (x);
	if(y<min)
		return min;
	else if (y>max)
		return max;
	else
		return y;
}

/* Generalized uniform random generator of double numbers in range [0;1] */
long double drand(void)
{
	srand((unsigned int) time(NULL));
	return (long double) drand48();
}


/* Polar form of the Box-Muller transformation */
long double get_random_gaussian(void)
{
	long double x1, x2, w;
	do {
		x1 = 2.0 * drand() - 1.0;
		x2 = 2.0 * drand() - 1.0;
		w = x1 * x1 + x2 * x2;
	} while( w > 1.0);
	return x1 * sqrtl( -2.0 * logl(w) / w );
}


/* Knuth 's algorithm, do NOT use for large lambda values */
int get_random_poisson(void)
{
	int i = 0;
	long double curr = 1;
	long double lim = expl(0.0 - __lambda);
	srand ( time(NULL) );
	do {
		i++;
//		curr *= drand();
		long double temp;
		if (lim <= 1.0)
		{
			temp = (rand() % 10001) / 10000.0;
		} else
		{
			temp = rand() % (int)lim;
		}
//		long double temp = rand() % (int)lim;
		curr *= temp;
	} while((i<BOUND_RESEARCH_TRIES)&&(curr>=lim));
	return i - 1;
}

long double get_random_weibull(void)
{
	return (__k) * expl( (1.0 / __lambda) * logl( 0.0 - logl( drand() ) ) );
}

long double get_random_exp(void)
{
/* FIXME exp may not work. Use get_random_weibull with adequate parameters instead ? */
	return logl( __lambda / drand() ) / __lambda;
}

long double get_random_pareto()
{
	return __xm / powl( (1.0 - drand()) , 1.0 / __alpha );
}

long double scale_beta(float val, long long int t_max, long long int t_min)
{
	double r_min = 0;
	double r_max = 1;

	long double result = ((t_max - t_min) * (val - r_min) / (r_max - r_min)) + t_min;

	return result;
}

/* Allocate a struct for DistribData, with "number" items inside */
Pdistribdata malloc_distribdata(unsigned int number)
{
	Pdistribdata result = malloc(sizeof(Tdistribdata));
	result->data = malloc(number * sizeof(myInteger));
	result->size = number;
	memset(result->data, 0, number * sizeof(myInteger));
	return result;
}


/* Free the memory associated with a distribution */
void free_distribdata(Pdistribdata d)
{
	if(d)
	{
		free(d->data);
		free(d);
	}
}

/* See initialize_irand() for explanations */
myInteger mask_max, __max;

/* Initialize the uniform random number generator with the new maximum */
void initialize_irand(myInteger max)
{
	/* Mask_max is the equivalent of max with all LSB set to 1 until the first MSB at 1.
	 * For example:
	 *  init_irand(00001) => mask_max = 00001
	 *  init_irand(00010) => mask_max = 00011
	 *  init_irand(00011) => mask_max = 00011
	 *  init_irand(00101) => mask_max = 00111
	 *  init_irand(00010010101011) => mask_max = 00011111111111
	 */
	double next_integer_after_log2_max_as_double = ceil(log2((double)max));
	int next_integer_after_log2_max_as_int = (int) next_integer_after_log2_max_as_double;
	mask_max = (1 << next_integer_after_log2_max_as_int) - 1;
	__max = max;
}

/* Random generator of integer, uniform distrib between [0..__max], see  initialize_irand() */
myInteger irand(void)
{
	myInteger try;
	do {
		try = random() & mask_max;
	} while (try >= __max);
	return try;
}

/* getDistribution generates "numberTurns" random numbers, conforming to "distrib",
 * and store the result into "result".
 */
Pdistribdata getDistribution(unsigned int numberTurns, Tdistrib distrib)
{

	unsigned int step;
	Pdistribdata  result;

	/* Bound numberTurns with MAX_DATA_IN_DISTRIBDATA */
	if(numberTurns>MAX_DATA_IN_DISTRIBDATA)
		numberTurns = MAX_DATA_IN_DISTRIBDATA;
//IF packetSizeDistrib = 0, the current function works for packet number/user distribution, IF packetSizeDistrib = 1, the current function works for packet size distribution
	if (packetSizeDistrib == 1)
	{
		numberTurns = user_arrival[elapseTime];
	}

/*	printf("Computation of %d number with a distrib of type=%d...        ",
			numberTurns, distrib.type);*/

	/* Prepare the uniform random generation */
	initialize_irand(distrib.max - distrib.min);

	/* Prepare the result struct */
	result = malloc_distribdata(numberTurns);

	/* Prepare to compute the numbers */
	__lambda 	= distrib.lambda;
	__k 		= distrib.k;
	__xm 		= distrib.xm;
	__alpha 	= distrib.alpha;
	__beta		= distrib.beta;

	//Below belongs to the set the beta distribution. Source : https://compbio.soe.ucsc.edu/gen_sequence/

	double beta_val;
    gen_beta_param g;
	if (packetSizeDistrib == 0)
	{
	    gen_beta_initialize(&g, (double) __alpha, (double) __beta);
	}

	long long int max = distrib.max;
	long long int min = distrib.min;


/*	step = numberTurns / 10;
	if(step<1)
		step = 1;*/

	/* Make all generation */
	int i;
	for(i=0;i<numberTurns;i++)
	{
/*		if(i%step==0)
			printf("\b\b\b\b\b\b\b\b%d0%% ... ", i/step);*/

		switch(distrib.type) {
		case constant:
			result->data[i] = distrib.max;
			break;
		case uniform:
			result->data[i] = distrib.min + irand();
			break;
		case normal:
			result->data[i] = bound_min_max(distrib.avg + distrib.sigma*get_random_gaussian(),
					distrib.min, distrib.max);
			break;
		case poisson:
		{
			if (packetSizeDistrib == 0)
			{
//Source: beta function calculation 3GPP TR 37.868. Beta(α, β) is calculated using the gama function B(α, β) = Γ(α)Γ(β)/Γ(α+β)
//Source: Beta Distribution, Paul Johnson <pauljohn@ku.edu> and Matt Beverlin <mbeverlin@ku.edu> June 10, 2013
//https://pj.freefaculty.org/guides/stat/Distributions/DistributionWriteups/Beta/Beta.pdf

				double temp1 = (double)(pow (i, 2) * pow ((numberTurns - i), 3));
				double temp2 = (double) (pow (numberTurns, 6) * 0.0166);
				double pdf = temp1 /temp2;
//				double pdf = (double)((pow (i, 2) * pow ((numberTurns - i), 3)) / (pow (numberTurns, 6) * 0.0166));
				__lambda = pdf * (DELTA_T/10.0) * TOTAL_USER;
//				printf ("lambda %Lf\n", __lambda);
//				printf ("beta pdf %Lf\n", __lambda);
				beta_pdf_report(pdf);
			}
//			sleep (1);
			int poiss = get_random_poisson();
			result->data[i] = bound_min_max(( (long double) distrib.min +
					(long double) (distrib.max - distrib.min)
					* (1.0 * poiss / Coeff_Poisson_adjust)),
					distrib.min, distrib.max);
			break;
		}
		case weibull:
			result->data[i] = bound_min_max(get_random_weibull(), distrib.min, distrib.max );
			break;
		case exponential:
			result->data[i] = bound_min_max(get_random_exp(), distrib.min, distrib.max );
			break;
		case pareto:
			result->data[i] = bound_min_max(get_random_pareto(), distrib.min, distrib.max );

			break;
		case beta:
			//Source : https://compbio.soe.ucsc.edu/gen_sequence/
			beta_val = gen_beta(&g);

			printf("%f\n", beta_val);

			result->data[i] = bound_min_max(scale_beta(beta_val, max, min), distrib.min, distrib.max );
			break;

		default:
			exit(-1);
		}

		if (packetSizeDistrib == 0)
		{
			user_arrival[i] = result->data[i];
		} else if (packetSizeDistrib == 1)
		{
			packet_size [elapseTime][i] = result->data[i];
			total_data(result->data[i]);
			printf ("Packet size; %d\n", result->data[i]);
		}



//		printf ("Total user origin %d\n", result->data[i]);
	}
//	printf("\b\b\b\b\b\b\b\b done.   \n");
	return result;
}
