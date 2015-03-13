#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#include <sys/times.h>

#include "libclient.h"

#define MAX_SERVERS 30

int main( int argc, char **argv )
{
	int *sd;						/* TCP sockets for each server */
	struct sockaddr_in *servaddr;				/* Servers addresses */

	static clock_t calc_time;
	static struct tms st_cpu, en_cpu;

	int *InputArray, *OutputArray, *temp;
	int *Output[MAX_SERVERS];
	
	int size, task_size, task_position;
	
	int pairs, i, j;
	
	int number_of_servers;
	
	FILE *in, *out;
	
	servaddr = calloc( MAX_SERVERS, sizeof( struct sockaddr_in ) );
	
	if( !servaddr )
	{
		printf( "Can\'t allocate memory.\n" );
		return -1;
	}

	if( argc < 3 )
	{
		printf( "Too few arguments.\n" );
		printf( "Usage:\t./merge_client [input_file] [output_file]\n" );
		return -1;
	}
	
	if( argc > 3 )
	{
		printf( "Too many arguments.\n" );
		printf( "Usage:\t./merge_server [input_file] [output_file]\n" );
		return -1;
	}
	
	if( ( number_of_servers = broadcast( servaddr ) ) < 0 )					/* Broadcast start */
		return -1;

	if( !( sd = connect_to_the_server( servaddr, number_of_servers ) ) )			/* Connecting to the servers */
		return -1;
	
	in = fopen( argv[1], "r" );								/* Open file for reading */
	fscanf( in, "%d", &size );								/* Reading array size */
	
	InputArray = calloc( size, sizeof(int) );
	OutputArray = calloc( size, sizeof(int) );
	
	if( !InputArray || !OutputArray )
	{
		/* Не удалось выделить память */
		printf( "Can\'t allocate memory.\n" );
		return -1;
	}
	
	printf( "Reading the file... " );
	fflush( stdout );
	
	for( i = 0; i < size; i ++ ){								/* Reading the file */
		fscanf( in, "%d", InputArray + i );
	}
	
	fclose( in );
	printf("OK!\n");
	
	pairs = ceil( ((double)size) / 2 );
	task_size = ceil( ((double)pairs) / number_of_servers ) * 2;
	
	for( i = 0; i < number_of_servers; i ++ )
	{
		Output[i] = calloc( task_size, sizeof(int) );
		
		if( !Output[i] )
		{
			printf( "Can\'t allocate memory.\n" );
			return -1;
		}
	}
	
	calc_time = times(&st_cpu);
	
	for( task_position = 0, i = 0; task_position < size; task_position += task_size, i ++ )
	{
		if( send_task( sd[i], InputArray, task_position, task_size, size, i ) < 0 )
			return -1;
	}
	
	calc_time = times(&en_cpu) - calc_time;
	
	temp = OutputArray;
	OutputArray = InputArray;
	InputArray = temp;
	
	for( i = 0; i < number_of_servers; i ++ )
	{
		Output[i] = get_result( sd[i], task_size );
	}
	
	printf( "Writing result in a file... " );
	fflush( stdout );
	
	out = fopen( argv[2], "w+" );
	
	OutputArray = lasttask( Output, number_of_servers, task_size, size );
	
	for( i = 0; i < size; i ++ )
		{
			fprintf( out, "%d ", *( OutputArray + i ) );
		}
			
	fclose( out );
	printf( "OK!\n" );
		
	printf("Calculation time: %.2f seconds.\n", (float) calc_time / 60 );

 	free( InputArray );
 	free( OutputArray );
	
	for( j = 0; j < number_of_servers; j ++ )
		close( sd[j] );
	
	return 0;
}
