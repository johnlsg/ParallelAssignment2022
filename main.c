#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#define THREAD_COUNT 8

int Find_bin(float elem, float bin_maxes[], int bin_count, float min_meas);

//OpenMP 4.5 is used
int main(int argc, char* argv[]) {
    //inputs
   int data_count =20;
   float data[] = {1.3, 2.9, 0.4, 0.3, 1.3, 4.4, 1.7, 0.4, 3.2, 0.3, 4.9, 2.4, 3.1, 4.4, 3.9, 0.4, 4.2, 4.5, 4.9, 0.9};
   float min_meas = 0;
   float max_meas = 5;
   int bin_count = 5;

   // outputs
   float bin_maxes[bin_count];
   int bin_counts[bin_count];

   //array for per thread bin_counts
   int loc_bin_cts[THREAD_COUNT][bin_count];
   memset(loc_bin_cts, 0, sizeof(loc_bin_cts[0][0]) * THREAD_COUNT * bin_count);

   float bin_width = (max_meas-min_meas)/bin_count;


   // initialize bin_counts to zeros
   for (int i=0; i <bin_count; i++){
        bin_counts[i]= 0;
    }

    #pragma omp parallel
    {
        // compute bin_maxes
        # pragma omp for
        for (int i=0; i <bin_count; i++){
            bin_maxes[i] = min_meas + bin_width*(i+1);
        }


        int threadID = omp_get_thread_num();

        #pragma omp for
        for (int i=0; i< data_count;i++){
            int bin = Find_bin(data[i] , bin_maxes, bin_count, min_meas);
            loc_bin_cts[threadID][bin]++;

        }


        #pragma omp for reduction(+:bin_counts[:bin_count])
        for (int i=0; i<THREAD_COUNT; i++){
            for(int j=0; j< bin_count; j++){
                bin_counts[j] += loc_bin_cts[i][j];
            }
        }
    }
    for (int i=0; i <bin_count; i++){
        if(i==0){
            printf("bin_maxes\tbin_counts\n");
        }
        printf("%.4f\t\t%d \n", bin_maxes[i], bin_counts[i]);
    }
    return 0;
}

int Find_bin(float elem, float bin_maxes[], int bin_count, float min_meas){
    for (int i =0; i< bin_count;i++){
        if(i==0){
            if(elem>= min_meas && elem < bin_maxes[i]){
                return 0;
            }else{
                continue;
            }
        }
        if(elem>= bin_maxes[i-1] && elem < bin_maxes[i]){
                return i;
            }else{
                continue;
            }
    }
    printf("%.4f \n", elem);
    return -1;
}
