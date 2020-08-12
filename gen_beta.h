
#ifndef GENBETAH
#define GENBETAH 1


typedef struct _gen_beta_param
    {   double a,b;		/* the parameters of the beta distribution */
        /* The remaining fields are precomputed values that can
	 * save some computation when many numbers are drawn from the
	 * same distribution.
	 */
	double min_ab, max_ab;  /* min(a,b) and max(a,b) */
    	double sum_ab;		/* a+b */
	double param[3];	/* various precomputed parameters,
				 * different for each generation method
				 */
    } gen_beta_param;

/* Initialize the variables of the already allocated generator in gen .
 * This must be called before gen can be used.
 */
void gen_beta_initialize(gen_beta_param *gen, double a, double b);


/* Generate one number from the distribution specified by gen */
double gen_beta(const gen_beta_param *gen);


#endif
