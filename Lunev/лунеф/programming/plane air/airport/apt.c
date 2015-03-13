#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<unistd.h>
#include <signal.h>
#include<pthread.h>
#include<fcntl.h>
#include <sys/time.h>



#define MINUTES 1

struct data{
int* planes;
int* k;
int* probesockfd;
int* sockfd;
};

pthread_mutex_t thread_mutex[2] = {PTHREAD_MUTEX_INITIALIZER,PTHREAD_MUTEX_INITIALIZER};
pthread_mutex_t data_mutex = PTHREAD_MUTEX_INITIALIZER;

void handler(int i){

   printf("killed\n");
   exit(0);
}


struct thread_struct{
	int number;
	int time0;
	int status;
	int sockfd;
	pthread_mutex_t* thread_mutex;
};




	
int recv_broadcast(int waiting_time){

   char message[] = "hello";
   char rcvline[1024];
   struct sockaddr_in servaddr,  cli_addr;
   struct timeval timeout;
   int sk_udp;
   socklen_t len;
   sk_udp = socket(AF_INET, SOCK_DGRAM, 0);

   
   bzero(&cli_addr, sizeof(cli_addr));
   bzero(&servaddr, sizeof(servaddr));

   //fill server address
   servaddr.sin_family = AF_INET;
   servaddr.sin_port = htons(52001);
   servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
   
   

   if((bind(sk_udp, (struct sockaddr*) &servaddr, sizeof(servaddr))) < 0){
      
      close(sk_udp);
      return -1;
   }
   
   len = sizeof(cli_addr);
   
	timeout.tv_sec = waiting_time;
	timeout.tv_usec=0;
	if((setsockopt(sk_udp,SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout))) < 0){
		printf("error setsockoptime \n");
		close(sk_udp);
		exit(EXIT_FAILURE);
	}
   
   
label:
	
	

   if((recvfrom(sk_udp, rcvline, 1024, 0, (struct sockaddr*) &cli_addr, &len )) < 0){
      close(sk_udp);
      return -1;
   }

  // printf("%s\n", rcvline);
  
   if((sendto(sk_udp, message, strlen(message) + 1, 0, (struct sockaddr*) &cli_addr, sizeof(cli_addr))) < 0){
      goto label;
     }
    
   close(sk_udp);
   return 1;
}


void* control(void* data1){
	int planes;
	int planes_all = 0;
//	int time0;
//	char line[20];
	int flag=0;
	long int p;
	
	struct data* data = (struct data*)data1;
while_label:
		flag = 0;
		planes = 0;
		
		if(*data->k == 0){
		
			if((recv_broadcast(10)) < 0){
				printf("can't recieve broadcast\n");
				flag=1;
				exit(1);
			}
			flag = 1;
			*data->k += 1;
		}
		else{
		
			if((recv_broadcast(10)) < 0){
				goto while_label;
			}
			else{
				printf("broadcast received\n");
				flag = 1;
				*data->k += 1;
				}
		}
		
		if((data->probesockfd[*data->k-1] = accept(*data->sockfd, NULL, NULL))<0){
			perror(NULL);
			exit(-1);
		}

		planes = read(data->probesockfd[*data->k-1],&p,sizeof(p));
		if(planes!=sizeof(int)){
			printf("error in reading amount of planes\n");
			exit(0);
		}
		planes = (int)ntohl(p);

		planes_all+=planes;
		printf("planes_all %d\n", planes_all);
		
		if(planes_all>5){
			printf("too much planes\n");
//			close(data->probesockfd[*data->k-1]);
			exit(0);
			}
		
		if(flag == 1){
		pthread_mutex_lock(&data_mutex);

		*data->planes = planes;
		
		pthread_mutex_unlock(&data_mutex);
		}

	goto while_label;		
}

void* run(void* args1){
	int n = 0;
	int k, hours, minutes;
	char line[3];
	int way;
	int t, time0;
	struct thread_struct* args = (struct thread_struct*)args1;
	time0 = args->time0;
	way = args->number;
	bzero(line,3*sizeof(char));
	printf("args is %p\n", args);
	printf("&thread_mutex is %p\n", (args->thread_mutex));
	while(1){
		
		pthread_mutex_lock((args->thread_mutex));
		k = 2;
		while(n<2){
			n+=read(args->sockfd,line,k);
			k-=n;
//		printf("%d",n);
			if(n == 0){
				printf("plane leaved\n");
				exit(0);
				}
			if(n>2){
				printf("extra information in the socket\n");
				exit(-1);
				}
			}
		n = 0;
			
//		printf("read %d\n", n);
		line[2] = '\0';
		if(way == 1){
			if(line[0] == 'a'){
				t = time(NULL);
				hours = ((t-time0)/20)%24;
				minutes = ((t - time0)%20)*3;
				printf("%02d:%02d 1 way --%c-- comes  < -- < -- <\n",hours, minutes, line[1]); // 30 symbols
				}
			if(line[0] == 'd'){
				t = time(NULL);
				hours = ((t-time0)/20)%24;
				minutes = ((t - time0)%20)*3;
				printf("%02d:%02d 1 way --%c-- leaves > -- > -- >\n",hours, minutes, line[1]); // 30 symbols
				}
			}
		if(way == 2){
			if(line[0] == 'a'){
				t = time(NULL);
				hours = ((t-time0)/20)%24;
				minutes = ((t - time0)%20)*3;
				printf("%02d:%02d                                      2 way --%c-- comes  < -- < -- <\n",hours, minutes, line[1]); // 30 symbols
				}
			if(line[0] == 'd'){
				t = time(NULL);
				hours = ((t-time0)/20)%24;
				minutes = ((t - time0)%20)*3;
				printf("%02d:%02d                                      2 way --%c-- leaves > -- > -- >\n",hours, minutes, line[1]); // 30 symbols
				}
			}
		
		sleep(1);
		args->status = 0;
	}	
}

int main(){
	int sockfd, planesockfd[10];
	int probesockfd[10];
//	int clilen;
//	int n;
	fd_set planeset;
	fd_set writeset;
//	fd_set set;
	int planes =0;
	int planes_all = 0;
	int yes = 1;
	int probelen;
	int i;
	pthread_t thread;
//	char line[20];
	int planelen, maxfd = 0, k;
	fd_set set;
	int time0;
	int apttime1;
	int in_num;
	int stack[15];
	int pt;
//	int d,a;
	
	
	struct thread_struct args[2];
	struct sockaddr_in aptaddr;
	struct sockaddr_in planeaddr[10];
	struct sockaddr_in probeaddr;
	struct data data;
	
	struct sigaction sa;
	sa.sa_handler = handler;
	sigfillset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTSTP, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
	sigaction(SIGPIPE, &sa, NULL);
//	struct sockaddr_in dplaneaddr[5];
	
//	printf("\n--sizeof(int) is %d--\n", sizeof(int));
	
	if((sockfd = socket(AF_INET, SOCK_STREAM,0))<0){
		perror(NULL);
		exit(-1);
	}
	
	bzero(&aptaddr, sizeof(aptaddr));
	aptaddr.sin_family = AF_INET;
	aptaddr.sin_port = htons(52001);
	aptaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if((setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*) &yes,sizeof(yes))) < 0){	
	      printf("error setsockopt\n");
	      close(sockfd);
	      exit(-1);
	}
	
	if(bind(sockfd,(struct sockaddr*)(&aptaddr),sizeof(aptaddr))<0){
		perror(NULL);
		close(sockfd);
		exit(-1);
	}
	
	if(listen(sockfd,25)<0){
		perror(NULL);
		close(sockfd);
		exit(-1);
	}
	
	k=0;
	probelen = sizeof(probeaddr);
	
	pthread_mutex_lock(&(thread_mutex[0]));
	pthread_mutex_lock(&(thread_mutex[1]));
	
//	printf("l\n");
	
	time0 = time(NULL);
	for (i=0;i<2;i++){
		(args[i]).number = i+1;
		(args[i]).status = 0;
		(args[i]).time0 = time0;
		(args[i]).sockfd = 0;
		(args[i]).thread_mutex = &thread_mutex[i];
		pthread_create(&thread, NULL, run, &(args[i]));
	}
	data.k = &k;
	data.planes = &planes;
	data.probesockfd = probesockfd;
	data.sockfd = &sockfd;
	pthread_create(&thread, NULL, control, &data);
label_while:
	pthread_mutex_lock(&data_mutex);
	
	if(planes>0){
		printf("amount of new planes is %d\n", planes);
		
		in_num = planes_all+1;
		planes_all+=planes;
		in_num = htonl(in_num);
		write(probesockfd[k-1],&in_num,sizeof(int)); //we must write initial number for plane process (number of current planes + 1)
		
		
		apttime1 = htonl((int)(time(NULL)-time0));
		write(probesockfd[k-1],&apttime1,sizeof(int));
		
		close(probesockfd[k-1]);
		planelen = sizeof(planeaddr);
	
	
	for(i=planes_all-planes;i<planes_all;i++){
		if((planesockfd[i] = accept(sockfd, NULL, NULL))<0){
			printf("accept failure\n");
			perror(NULL);
			close(sockfd);
			exit(-1);
			}
//		printf("socket %d\n", planesockfd[i]);
		}

	for(i = 0; i<planes_all; i++){
		if(maxfd < planesockfd[i])
			maxfd = planesockfd[i];
		}
		planes = 0;
		
		pthread_mutex_unlock(&data_mutex);
		
	}
	else
		pthread_mutex_unlock(&data_mutex);
	
	while(1){
		FD_ZERO(&planeset);
		FD_ZERO(&writeset);
		FD_ZERO(&set);
		for(i=0;i<planes_all;i++){
			FD_SET(planesockfd[i], &planeset);
			}
		if(planes_all>0)
		select(maxfd+1, &planeset, &writeset, &set, NULL);
		
		pt = 0;
		for(i=0;i<planes_all;i++){
			if(FD_ISSET(planesockfd[i],&planeset)){
				stack[pt] = planesockfd[i];
				pt+=1;
				}
			}

		while(pt>0){
			if(args[0].status == 0){
				args[0].sockfd = stack[pt-1];
				pt-=1;
				args[0].status = 1;
				pthread_mutex_unlock((args[0].thread_mutex));
				}	
			else{
				if(args[1].status == 0){
					args[1].sockfd = stack[pt-1];
					pt-=1;
					args[1].status = 1;
//					printf("2 %d\n", pt);
					pthread_mutex_unlock((args[1].thread_mutex));
					}
				}
			}
		if(planes_all<10)
			
			goto label_while;
		}
		exit(0);
	}