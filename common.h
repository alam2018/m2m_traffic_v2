/*
 * common.h
 *
 *  Created on: 9 oct. 2012
 *      Author: avaret
 */

#ifndef COMMON_H_
#define COMMON_H_


#define bool int
#define true 1
#define false 0

/* Pour préciser les fonctions de sortie */
#define EXIT_POINT  __attribute__((noreturn))

/* type MyInteger: (-1 .. 1E15) [unit=ns].
 * -1 generally means "error", 0 can mean INFINITE or just 0 (nul) */
#define myInteger long long

/* PercentMyInt is the symbol for the printf&co functions */
#define PercentMyInt "%lld"

#define NS_IN_SECONDS 1000000000

#define MS_IN_NS 1000000

/* MAX_DATA_IN_DISTRIBDATA limits the number of random data in distributions.
 *  When more data are required, read the data cyclicly... */
#define MAX_DATA_IN_DISTRIBDATA 1000000

/* It defines the value of internal_buffer_size */
#define DEFAULT_INTERNAL_BUFFER_SIZE (1470)

#define DEFAULT_DELAY_PRECISION (50 * MS_IN_NS)

typedef struct {
	unsigned int size;
	myInteger * data;
} Tdistribdata;

typedef Tdistribdata * Pdistribdata;

typedef enum __Edistrib {
	erroneous, constant, uniform, exponential, poisson, normal, weibull, pareto, beta
} Edistrib;

typedef struct {
	Edistrib type;
	myInteger min, max;
	double lambda, k, avg, sigma, alpha, xm, beta;
	char * datafilename;
} Tdistrib;

typedef struct {
	bool defined_by_user;
	unsigned int  number_of_source;

	bool tcp;
	bool receiver;
	char * destination;
	unsigned int port_number;
	unsigned int internal_buffer_size;

	bool ipv4;
	Tdistrib Don, Doff;

	unsigned int turns;
	myInteger delay_before_start, delay_stop_after;

	myInteger udp_delay_precision;
	int tcp_sock_opt;

	unsigned int tcp_max_conn_ign, tcp_max_conn_exit;
	unsigned int udp_max_bitr_ign, udp_max_bitr_exit;

	unsigned int rand_seed;

	void * next; /* chained list */
} TOneSourceOnOff;

typedef TOneSourceOnOff POneSourceOnOff;

extern int verbose;

#define debug if(verbose>0) printf
#define debug_verbose if(verbose>1) printf
#define debug_very_verbose if(verbose>2) printf

/* When the user wants the program to run indefinitely */
#define INFINITE_TURN_NUMBER MAX_DATA_IN_DISTRIBDATA

/* And the associed condition macro */
#define is_infinite_turn_number(x) (x.turns == INFINITE_TURN_NUMBER)

#endif /* COMMON_H_ */
