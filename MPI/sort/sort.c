#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <math.h>


#define TREEMERGE

void merge(int *sorted_array, long total_size, int **chunks, long chunk_size, long n_of_chunks);
void merge2(int *output, int *arr1, int* arr2, int len1, int len2);

int ComparisonFunc(const void * a, const void * b) {
    return ( * (int *)a - * (int *)b );
}

bool check_sorted(int *array, long size) {
    if (size <= 1) return true;
    for (long i = 1; i < size; ++i) {
        if (array[i - 1] > array[i]) {
            return false;
        }
    }
    return true;
};


int main(int argc, char** argv) {
	int n = atoi(argv[1]);

    int world_rank;
	int world_size;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);	

    int *original_array = NULL;
    int *received_array = NULL;
    int *sorted_array   = NULL;

    int size = ceil(double(n) / double(world_size));
	int *sub_array = (int *)malloc(size * sizeof(int));

    if (world_rank == 0) {
	    original_array = (int *)malloc(n * sizeof(int));
        sorted_array   = (int *)malloc(n * sizeof(int));
        received_array = (int *)malloc(n * sizeof(int));
        srand(time(NULL));
        for(int i = 0; i < n; ++i) {	
            original_array[i] = rand();
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double dt = MPI_Wtime();


	MPI_Scatter(original_array, size, MPI_INT, sub_array, size, MPI_INT, 0, MPI_COMM_WORLD);
	qsort(sub_array, size, sizeof(int), ComparisonFunc);

#ifndef TREEMERGE
    MPI_Gather(sub_array, size, MPI_INT, received_array, size, MPI_INT, 0, MPI_COMM_WORLD);
	if(world_rank == 0) {
        int **parts = (int **)malloc(world_size * sizeof(int *));
        for (int i = 0; i < world_size; ++i) {
            parts[i] = (int *)((&received_array[0]) + i*size);
        }

        merge(sorted_array, n, parts, size, world_size);
        printf("Sort successful: %d\n", check_sorted(sorted_array, n));
        free(parts);
    }
    free(sub_array);
#else
    long long int current_size = size;
    long long int recv_size = size; 
    int current_world_size = world_size;
    int id = 0;
 
    while (current_world_size != 1) {
        int middle = current_world_size / 2 - 1;
        for (int i = 0; i <= middle; i++) {
            int left_id = current_world_size - i - 1;
            int rght_id = i;
            if (world_rank == left_id) {
                MPI_Send(&current_size, 1, MPI_INT, rght_id, id, MPI_COMM_WORLD);
            }
            if (world_rank == rght_id) {
                MPI_Recv(&recv_size, 1, MPI_INT, left_id, id, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }
 
        int *recv_array = NULL;
        if (world_rank <= middle)
            recv_array = (int*) calloc(recv_size, sizeof(int));
 
        for (int i = 0; i <= middle; i++) {
             int left_id = current_world_size - i - 1;
            int rght_id = i;
            if (world_rank == left_id) {
                MPI_Send(sub_array, current_size, MPI_INT, rght_id, id, MPI_COMM_WORLD);
            }
            if (world_rank == rght_id) {
                MPI_Recv(recv_array, recv_size, MPI_INT, left_id, id, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }
 
        if (world_rank <= middle) {
	        int *new_sub_array = (int *)malloc((current_size + recv_size) * sizeof(int));
            merge2 (new_sub_array, sub_array, recv_array, current_size , recv_size);
 
            free (recv_array);
            free (sub_array);
 
            current_size += recv_size;
            recv_size = 0;
 
            sub_array = new_sub_array;
        }
 
        current_world_size = ceil((double )current_world_size / 2);
        id ++;
    }

	if(world_rank == 0) {
        printf("Sort successful: %d\n", check_sorted(sub_array, n));
    }
#endif

	if(world_rank == 0) {
        free(original_array);
        free(received_array);
        free(sorted_array);
    }


    MPI_Barrier(MPI_COMM_WORLD);
    
    if (world_rank == 0) {
        printf("Time = %f\n", MPI_Wtime() - dt);;
    }

    MPI_Finalize();    
}

void print_arr(int *sorted_array, long total_size) {
    for(int i = 0; i < total_size; ++i){
        printf("%d\n", sorted_array[i]);
    };
};

void merge(int *sorted_array, long total_size, int **chunks, long chunk_size, long n_of_chunks) {
    /*
    for (int i = 0; i < chunk_size; ++i) {
        for (int j = 0; j < n_of_chunks; ++j)
            printf("%d\t", chunks[j][i]);
        printf("\n");
    };
    */

    long *cnt = (long *)calloc(n_of_chunks, sizeof(long));
    long sorted_i = 0;


    int max = chunks[0][chunk_size - 1];
    for (int i = 1; i < n_of_chunks; ++i) {
        if (max < chunks[i][chunk_size - 1]) max = chunks[i][chunk_size - 1];
    }

    int lowest_j = 0;
    while (sorted_i < total_size) {
        //print_arr(sorted_array, total_size);

        int min = max;
        for (int j = 0; j < n_of_chunks; ++j) {
            if (cnt[j] >= chunk_size) continue;
            if (chunks[j][cnt[j]] < min) {
                lowest_j = j;
                min = chunks[j][cnt[j]];
            }
        };
        cnt[lowest_j] ++; 
        sorted_array[sorted_i] = min;
        sorted_i ++;
    }
        
    
    free(cnt);
};


void merge2(int *output, int *arr1, int* arr2, int len1, int len2){
    long sorted_i = 0; 
    int cnt1 = 0, cnt2 = 0;
    while (sorted_i < len1+len2) {
        //print_arr(sorted_array, total_size);
        if (arr1[cnt1] < arr2[cnt2]) {
            output[sorted_i] = arr1[cnt1];
            cnt1 ++;            
        }
        else {
            output[sorted_i] = arr2[cnt2];
            cnt2 ++;
        }
        sorted_i ++;
    }
};

