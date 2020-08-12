/*
 * traffic_agg_main.c
 *
 *  Created on: Feb 12, 2020
 *      Author: mdaa
 */

/*Consult with R2-114569 TR37.868 V0.8.1.doc for the definition of the traffic model
 * Model 1 generates uncoordinated traffic, model 2 generates coordinated traffic
 */

#include "main.h"
#include "unistd.h"
//#include "usage.h"
//#include "long_usage.h"
//#include <pthread.h>

#include "tx_rx.h"


#define REPORT_WRITE_PER_SECOND

/* Verbosity level: 0=normal, >0=speaker */
int verbose = 0;





//Set either for packet size or user arrival the distribution is generated
int packetSizeDistrib;
int elapseTime;

POneSourceOnOff initializeOneSourceOnOff(POneSourceOnOff nextOne)
{

	POneSourceOnOff newsource;

	//Don represents the packet size
	newsource.receiver 				= true;
	newsource.ipv4 					= true;

	newsource.Don.type				= normal;
//	newsource.Don.type				= pareto;
	newsource.Don.max 				= 1500; /* in bytes per timeunit */
	newsource.Don.min 				= 50; /* in bytes per timeunit */
	newsource.Don.lambda 			= 4;
//	newsource.Don.lambda 			= 0.00027;
	newsource.Don.k 				= 1.0;
	newsource.Don.avg 				= (double) newsource.Don.max * 0.5;
	newsource.Don.sigma 			= (double) newsource.Don.max * 0.1;
	newsource.Don.alpha 			= 1.1;
	newsource.Don.xm 				= 0.81;//4;


	//Doff represents the number of MTC devices
	if (TRAFFIC_MODEL == 1)
	{
		newsource.Doff.type				= uniform;
	} else
	{
		newsource.Doff.type				= poisson;
//		newsource.Doff.type				= beta;
	}
//	newsource.Doff.type				= uniform;
	newsource.Doff.max				= TOTAL_USER;
	newsource.Doff.min				= TOTAL_USER * 0.6;
//	newsource.Doff.min				= 10;
//	newsource.Doff.min				= TOTAL_USER * 0.6;
//	newsource.Doff.lambda 			= 0.28;
	newsource.Doff.lambda 			= 1.0;
	newsource.Doff.k 				= 0.74;
	newsource.Doff.avg 				= (double) newsource.Doff.max * 0.5;
	newsource.Doff.sigma 			= (double) newsource.Doff.max * 0.1;
	newsource.Doff.alpha 			= 3.0;
	newsource.Doff.xm 				= 1.0;
	newsource.Doff.beta				= 4.0;

	newsource.udp_delay_precision 	= DEFAULT_DELAY_PRECISION;
	newsource.internal_buffer_size 	= DEFAULT_INTERNAL_BUFFER_SIZE;
//	newsource.port_number 			= DEFAULT_PORT_NUMBER;

/*	if (TRAFFIC_MODEL == 1)
	{
		newsource.turns = 60;
	} else if (TRAFFIC_MODEL == 2)
	{
		newsource.turns = 10;
	} else
	{
		printf ("Wrong traffic model selected \n");
		exit (0);
	}*/
	newsource.turns 		= DISTRIBUTION_PERIOD;

//	newsource.next 		= (void *) nextOne;
	return newsource;
}


void goChild(TOneSourceOnOff src, int index)
{
	int dry_run = 1;

	/* Here dataon represents the distribution of the packet size. dataoff represents the number of MTC devices
	 */

	Pdistribdata dataOn[TOTAL_USER], dataOff ;

	if(src.rand_seed!=0)
		srand(src.rand_seed);
	else
		srand((unsigned int) rand() +src.number_of_source);

	packetSizeDistrib = 0;
//	int sim_loop = ((SIMULATION_RUN_TIME_MINUTE * 60)/ src.turns) / DELTA_T;

	int noSession = (SIMULATION_RUN_TIME_MINUTE * 60) / DISTRIBUTION_PERIOD;
	int k;

//	if (TRAFFIC_MODEL == 1)
	if(true)
	{
		for (k = 0; k < noSession; k++)
		{
			//Doff represents the number of MTC devices
			dataOff = getDistribution(src.turns, src.Doff);

			int i;
	/*		for (i = 0; i < src.turns; i++)
			{
				printf ("Total user %lld\n", dataOff->data[i]);
			}*/

			//Don represents the packet size
			packetSizeDistrib = 1;
			for (i = 0; i<src.turns; i++)
			{
				count_packet (dataOff->data[i]);
				dataOn[i]  = getDistribution(src.turns, src.Don);
				elapseTime++;

				report_write();


			}

			elapseTime = 0;
			packetSizeDistrib = 0;
			sleep (1);
		}
	} else
	{
		exit (0);
	}



//	free_distribdata(dataOn[index]);
//	free_distribdata(dataOff[index]);
}


void startAllSources()
{
	signal(SIGCHLD, SIG_IGN );
//	POneSourceOnOff src[TOTAL_USER];
	POneSourceOnOff src;
	src = initializeOneSourceOnOff(completeSetOfSourcesToUse);

	goChild(src, 1);
//	createNewSourcesSet(i);

}

int main(int argc, char*argv[])
{
	setbuf(stdout, NULL);
	srand((unsigned int) time(NULL));

	file_prep ();

//	initialize_stat_report ();
//	parseCmdLine(argc, argv);
	printf("\nSources ON/OFF generator\n");
//	checkConsistencyOfAllSourcesOnOff();
	startAllSources();
	write_stat_report ();
	return EXIT_SUCCESS;
}
