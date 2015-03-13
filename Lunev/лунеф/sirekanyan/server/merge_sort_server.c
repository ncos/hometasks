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
#include <signal.h>
#include <errno.h>
#include <limits.h>

#include "libserver.h"

int main( int argc, char **argv )
{
	int sockfd;
	char **end = malloc( sizeof(char) );
	
	if( !end )
	{
		printf( "Can\'t allocate memory.\n" );
		return -1;
	}
	
	if( argc < 2 )
	{
		printf( "Too few arguments.\n" );
		printf( "Usage:\t./merge_server [number_of_threads]\n" );
		free( end );
		return -1;
	}
	
	if( argc > 2 )
	{
		printf( "Too many arguments.\n" );
		printf( "Usage:\t./merge_server [number_of_threads]\n" );
		free( end );
		return -1;
	}
	
	long int max_thread_long = strtol( argv[1], end, 10 );
	
	if( **end != '\0' )
	{
		printf( "Error in the argument: it must be a positive integer.\n" );
		free( end );
		return -1;
	}
	
	free( end );
		
	if( max_thread_long == LONG_MAX || max_thread_long == LONG_MIN )
	{
		printf( "Error in the argument: it must be a positive integer.\n" );
		return -1;
	}
	
	if( max_thread_long >= INT_MAX || max_thread_long <= 0 )
	{
		printf( "Error in the argument: it must be a positive integer.\n" );
		return -1;
	}
	
	int max_thread = ( int ) max_thread_long;
	
	while(1)
	{
		if( broadcast() < 0 )							/* Broadcasting */
			return -1;
			
		if( ( sockfd = wait_for_the_client() ) < 0 )				/* Waiting for the client */
			return -1;
		
		if( receive_and_make_task( sockfd, max_thread ) < 0 )			/* Making the task */
			return -1;
		
		if( close( sockfd ) < 0 )
		{
			printf( "Sorry can\'t close the TCP socket.\n" );
			return -1;
		}
	}
	
	signal( SIGINT, SIG_DFL );
	
	return 0;
}
