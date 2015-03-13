#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <limits.h>
#include <malloc.h>
#include <string.h>
#include "header.h"
#include <math.h>
#include <resolv.h>
#include <signal.h>

void handler(int i){

	printf("\n");
	exit(0);
}

int recv_broadcast(){

	char message[] = "hello";
	char rcvline[1000];
	struct sockaddr_in servaddr,  cli_addr;
	int sk_udp, len;
	sk_udp = socket(AF_INET, SOCK_DGRAM, 0);

   
	bzero(&cli_addr, sizeof(cli_addr));
	bzero(&servaddr, sizeof(servaddr));

	//fill server address
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(52000);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if((bind(sk_udp, (struct sockaddr*) &servaddr, sizeof(servaddr))) < 0){
      
		close(sk_udp);
		return -1;
	}
   
	len = sizeof(cli_addr);
   
	label:

	if((recvfrom(sk_udp, rcvline, 999, 0, (struct sockaddr*) &cli_addr, &len )) < 0){
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

double detrim(double** a, int n){
	double det = 1, temp;
	int i, j, k, f, t;

	//for (f=0; f<n;f++)
	for (i=0; i<n; ++i) {
		k = i;
		//find the maximum element in i column and replace it to A[i][i]
		for (j=i+1; j<n; ++j){
			if ((a[j][i] - a[k][i]) > 0)
			k = j;
		}
		//replace the maximum element to A[i][i]
		if (i != k){
			for(t = 0; t < n; t++){
				temp = a[i][t];
				a[i][t] = a[k][t];
				a[k][t] = temp;
			}
		}  
		//if we changed lines, multiply det on -1
		if (i != k){
			det = -det;
		}

		det *= a[i][i];
		//zero all column exept A[i][i]
		for (j = i+1; j < n; ++j){
			a[i][j] /= a[i][i];
		}
		for (j = 0; j < n; ++j){
			if (j != i && a[j][i] > 0){
				for (k = i+1;  k < n; ++k){
					a[j][k] -= a[i][k] * a[j][i];
				}  
			}      
		}
	}

	return det;
}

void delete_matrix(double** A, int n){
	int i;

	for(i = 0; i < n; i++){
		free(A[i]);
	}
	free(A);

	return;
}   

int check_number(int N,char* end, char* p){//verifying  input number 
   
	if((errno == ERANGE && (N == LONG_MAX || N == LONG_MIN)) || (errno != 0 && N == 0) ){ //overflow of int
		return 1;
	}
  
	if(end == p){    //no digits at all
		return 2;
	}
        
	if(*end != '\0'){ //some values aren't digits
		return 3;
	}
 
	return 0;
}

void* func_thread (void* arg){
	param* p = (param*)arg;
	int i, j, task;
	int n = p->n;
	double **a1;
	double det_a, result;// b[n]; 

	//allocate memory for  the matrix a1
	if((a1 = (double**)malloc(sizeof(double*)*n)) == NULL){
		pthread_mutex_lock(&mutextask); 
		ntask = -1; 
		pthread_mutex_unlock(&mutextask);
		return NULL;
	}
 
	for(i = 0; i < n; i++){
		if((a1[i] = (double*)malloc(sizeof(double) * n)) == NULL){           
			delete_matrix(a1, i);
			pthread_mutex_lock(&mutextask); 
			ntask = -1; 
			pthread_mutex_unlock(&mutextask);
			return NULL;
		}
	}
      
	for(i = 0;i < n; i++){
		for(j = 0; j < n ; j++){
			a1[i][j] = p->a[i][j];
		}
	}
    

	det_a=detrim(a1, n);


	while(1){ 
      

		pthread_mutex_lock(&mutextask); 
		//check the ntask (do we need to calculate?)
		if(ntask >= max_numb || ntask < 0){
			pthread_mutex_unlock(&mutextask);
			delete_matrix(a1, n);
			return NULL;
		} 

		task = ntask;
		pthread_mutex_unlock(&mutextask);

		//copy matrix A   
		for(i = 0;i < n; i++){
		for(j = 0; j < n ; j++)
			a1[i][j] = p->a[i][j];
		}
		//change task column in matrix A1
		for(i = 0; i < n; i++){
			a1[i][task] = p->b[i];
		}

		//calculate result
		result = (detrim(a1, n))/det_a;
     
		//check result
		pthread_mutex_lock(&mutextask); 
		if(result == HUGE_VAL){ 
			ntask = -1;
		}
		else{
			ntask++;
			p->result[task] = result;
		}
		pthread_mutex_unlock(&mutextask);
	}
	delete_matrix(a1, p->n);
	return NULL;
}

