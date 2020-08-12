/*
 * report.c
 *
 *  Created on: Feb 20, 2020
 *      Author: mdaa
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "stdbool.h"

#include "tx_rx.h"

#define PKT_COUNT_BIN		500
#define PKT_SIZE_BIN		200

void pkt_size_stat (long long int size);
void pkt_count_stat (long long int packet);

int total_packet;
long int calcPacketSize;

long long int pkt_number_bin [TOTAL_USER / 50];
long long int pkt_size_bin [100];


FILE *traffic_write;
FILE *traffic_stat;
FILE *beta_pdf;
void file_prep ()
{
	traffic_write = fopen ("traffic_simulation.csv","w+");
	setbuf(traffic_write, NULL);
	if (traffic_write == NULL)
	{
		printf ("File not created okay, errno = %d\n", errno);
	}
	fprintf (traffic_write,"Time Scale; Number of Packets; Total data (b); \n");

	traffic_stat = fopen ("traffic_statistics.csv","w+");
	setbuf(traffic_stat, NULL);
	if (traffic_stat == NULL)
	{
		printf ("File not created okay, errno = %d\n", errno);
	}

//Enable this file writing for debugging purpose. Otherwise not needed
	beta_pdf = fopen ("beta_pdf_report.csv","w+");
	setbuf(beta_pdf, NULL);
	if (beta_pdf == NULL)
	{
		printf ("File not created okay, errno = %d\n", errno);
	}
}

void count_packet (long long int packet)
{
	total_packet += packet;
	pkt_count_stat(packet);
}

void total_data (long long int size)
{
	calcPacketSize += size;
	pkt_size_stat (size);
}


double time_count = 0;

void report_write ()
{
	time_count += DELTA_T;
	fprintf (traffic_write,"%f; %d; %ld; \n", time_count, total_packet, calcPacketSize);

	total_packet = 0;
	calcPacketSize = 0;
}



void pkt_size_stat (long long int size)
{
	int i;
	int temp1 = 0;
	int temp2;
	for (i=0; i<1500/PKT_SIZE_BIN; i++)
	{
		temp2 = temp1 + PKT_SIZE_BIN;
		if ((temp1 < size) && (size <= temp2))
		{
			pkt_size_bin[i]++;
			break;
		}
		temp1 = temp2;
	}
}

void pkt_count_stat (long long int packet)
{
	int i;
	int temp1 = 0;
	int temp2;
	for (i=0; i<TOTAL_USER/PKT_COUNT_BIN; i++)
	{
		temp2 = temp1 + PKT_COUNT_BIN;
		if ((temp1 < packet) && (packet <= temp2))
		{
			pkt_number_bin[i]++;
			return;
		}
		temp1 = temp2;
	}

	printf ("Wrong bin design for pkt count\n");
}

void write_stat_report ()
{
	fprintf (traffic_stat,"Packet Count; Number of Packets;\n");
	int i, count =0;
	for (i=0; i<(TOTAL_USER/PKT_COUNT_BIN); i++)
	{
		fprintf (traffic_stat, "%d - %d; %lld\n", count, count + PKT_COUNT_BIN, pkt_number_bin[i]);
		count += PKT_COUNT_BIN;
	}

	count = 0;

	fprintf (traffic_stat,"\n\n\n\n\n\n\n\nPacket Size; Number of Packets;\n");
	for (i=0; i<(1500/PKT_SIZE_BIN); i++)
	{
		fprintf (traffic_stat, "%d - %d; %lld\n", count, count + PKT_SIZE_BIN, pkt_size_bin[i]);
		count += PKT_SIZE_BIN;
	}


}

bool beta_file_init = true;
int beta_time_count = 1;
void beta_pdf_report (double pdf_val)
{
	if (beta_file_init == true)
	{
		fprintf (beta_pdf,"Time; PDF;\n");
		fprintf (beta_pdf, "%d; %f\n", beta_time_count, pdf_val);
		beta_file_init = false;
	} else
	{
		fprintf (beta_pdf, "%d; %f\n", beta_time_count, pdf_val);
	}
	beta_time_count++;
}
