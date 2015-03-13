#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <signal.h>
#include<pthread.h>

#define HOUR 5

struct plane_struct{
	int sockfd;
	char* line;
	int number;
	int time0;
	int delta;
};

void handler(int i){

   printf("dying\n");
   exit(0);
}

//1 second = 3 minutes, 20 seconds = hour
void* plane(void* args1){
	//(int) (10.0*random()/(RAND_MAX+1.0))
	int n = 0;
	int t, time0;
	int delta, hours, minutes;
	char sending_line[3];
	int departing_time, arriving_time, arriving_range;
	struct plane_struct* args = (struct plane_struct*)args1;
	delta = args -> delta;
	srandom(time(NULL)+args->number);
	time0 = args->time0;
	while(1){
		
		departing_time = 2+(int)(18*((random()/(RAND_MAX+1.0))));
		arriving_range = departing_time;
		arriving_time = (int)(arriving_range*(random()/(RAND_MAX+1.0)));
//		printf("dt %d ar %d at %d\n", departing_time, arriving_range, arriving_time);
		sleep (arriving_time);
		t = time(NULL);
		hours = (t-time0+delta)/20%24;
		minutes = (t - time0 + delta)%20*3;
		printf("%02d:%02d    plane --%d-- requests way to come   \n" ,hours , minutes, args->number);
		
		sprintf(sending_line,"a%d",args->number); // message that --number-- arrives
//		printf("after sprintf %d\n", (int)sending_line[2]);
//		sending_line[2] = '\0';
		n = write(args->sockfd, sending_line, strlen(sending_line));
		
		sleep(departing_time-arriving_time);
		
		t = time(NULL);
		hours = (t-time0+delta)/20%24;
		minutes = (t - time0 + delta)%20*3;
		printf("%02d:%02d    plane --%d-- requests way to leave  \n", hours , minutes, args->number );
		
		
		sprintf(sending_line,"d%d",args->number);
//		sending_line[2] = '\0';
		n = write(args->sockfd, sending_line, strlen(sending_line));
		sleep(20 - departing_time);
		}
}
		
int main(int argc, char* argv[]){
	int sockfd[10];
	int time0;
	int probesockfd;
	int my_init_number = 0;
	socklen_t len;
	int yes = 1;
	int i,res;
	int apttime1, delta;
	pthread_t* threads;
	int sk_udp;
	int amount = 0;
	char message[] = "is there any server?\n";
//	char dep_line[20] = "departing plane";
//	char arr_line[20]="arriving  plane";
//	char sendline[20];
	char rcvline[20];
	struct timeval timeout;
	struct plane_struct args[10];
	
	int planes;
	struct sockaddr_in aptaddr, myaddr, saddr_dest;
	if(argc!=2){
		printf("usage: plane.out [number of planes, total<=5]\n");
		exit(-1);
		}
	
	struct sigaction sa;
	sa.sa_handler = handler;
	sigfillset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTSTP, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
	sigaction(SIGPIPE, &sa, NULL);
	
	planes = strtol(argv[1],NULL,10);
	if(planes<=0){
		printf("bad argument\n");
		exit(0);
		}
//	arr = strtol(argv[2],NULL,10);
/*	if(dep<0 || arr< 0){
		printf("wrong arguments\n");
		exit(-1);
		}
*/		
//broadcast	
//	printf("\n--sizeof(int) is %d--\n", sizeof(int));
	threads = malloc(planes*sizeof(pthread_t));
	if(threads == NULL){
		printf("can'a allocate memory for threads\n");
		exit(-1);
		}
	printf("threads %p\n", threads);
	sk_udp = socket(AF_INET, SOCK_DGRAM, 0);

   
	bzero(&myaddr, sizeof(myaddr));
	bzero(&saddr_dest, sizeof(saddr_dest));

   
	myaddr.sin_family = AF_INET;
	myaddr.sin_port = 0;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);

   
	if((bind(sk_udp, (struct sockaddr*) &myaddr, sizeof(myaddr))) < 0){
		printf("error bind \n");
		close(sk_udp);
		exit(EXIT_FAILURE);
	}

   	if((setsockopt(sk_udp, SOL_SOCKET, SO_BROADCAST, (char*) &yes,sizeof(yes))) < 0){
		printf("error setsockopt\n");
		close(sk_udp);
		exit(EXIT_FAILURE);
	}

   
	saddr_dest.sin_port = htons(52001);
	saddr_dest.sin_family = AF_INET;
	saddr_dest.sin_addr.s_addr = INADDR_BROADCAST;

	len = sizeof(aptaddr);

   // set limit time for answer
	timeout.tv_sec=15;
	timeout.tv_usec=0;

	if((setsockopt(sk_udp,SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout))) < 0){
		printf("error setsockoptime \n");
		close(sk_udp);
		exit(EXIT_FAILURE);
	}

    
	if((sendto(sk_udp, message, strlen(message) + 1, 0, (struct sockaddr *) &saddr_dest, sizeof(saddr_dest))) < 0){
		printf("error sendto\n");
		close(sk_udp);
		exit(EXIT_FAILURE);
	}

	
       // wait answer
		bzero(rcvline,20);
		res=recvfrom(sk_udp, rcvline, 999, 0, (struct sockaddr *) &aptaddr, &len );
		if(res==-1){
	    // if time is out
		//	if(errno==EAGAIN) break;
	   //  error
			printf("error recv\n");
			close(sk_udp);
			exit(EXIT_FAILURE);
		}
 //         printf("%s\n", rcvline);
	
	  close(sk_udp);
//	exit(0);
 //      }

//now we know apt adress
	printf("aptaddr port %d ip %s\n", ntohs(aptaddr.sin_port), inet_ntoa((aptaddr.sin_addr)));
	
	if((probesockfd = socket(AF_INET, SOCK_STREAM, 0))<0){
			printf("couldn't create socket\n");
			exit(-1);
			}
		
//	sleep(1);
	
	if(connect(probesockfd, (struct sockaddr*)&aptaddr, sizeof(aptaddr))<0){
			printf("connection failed\n");
			perror("connect probe");
			close(probesockfd);
			exit(-1);
			}
	amount = planes;
	amount = (int)htonl(amount);
	write(probesockfd, &amount, sizeof(int));

	i= read(probesockfd,&my_init_number,sizeof(int));
//	printf("%d\n", i);
	my_init_number = (int)ntohl(my_init_number);
	
	printf("init num %d\n", my_init_number);
	
	read(probesockfd, &apttime1, sizeof(int));
	apttime1 = (int)ntohl(apttime1);
	time0 = time(NULL);
	delta = apttime1 - (time(NULL) - time0);
	close(probesockfd);
//	bzero(sendline, 20);
//	srandom(time(NULL));
	for(i=0;i<planes;i++){
		if((sockfd[i] = socket(AF_INET, SOCK_STREAM,0))<0){
			printf("couldn't create socket\n");
			exit(-1);
			}
		}
	
	
//	srandom(time(NULL));
//we already have aptaddr

	
	for(i=0;i<planes;i++){
		if(connect(sockfd[i], (struct sockaddr*)&aptaddr,sizeof(aptaddr))<0){
			printf("connection failed\n");
			perror("connect in planes");
			close(sockfd[i]);//must close all!!!
			exit(-1);
		}
		(args[i]).sockfd = sockfd[i];
		(args[i]).delta = delta;
		(args[i]).number = my_init_number+i;
		(args[i]).time0 = time0;
		pthread_create(&(threads[i]),NULL,plane,&(args[i]));
	}
	for(i = 0; i<planes; i++){
		printf("thread %p\n", threads[i]);
	}
		pthread_join(threads[i-1],NULL);
		
	return 0;	
}
	