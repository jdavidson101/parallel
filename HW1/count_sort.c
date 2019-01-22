#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <omp.h>

void Serial_count_sort(int a[], int n);
void Parallel_count_sort(int a[], int n);
int compareArrays(int a[], int b[], int c[], int n);
int cmpfunc (const void * a, const void * b);

int main(int argc, char * argv[]) {
    if (argc != 3) {
        printf("Usage: ./a.out <array size (int)> <# of threads to request (int)>-- ex. : ./a.out 10000 6)");
        return -1;
    }
    
    int n = atoi(argv[1]);
    omp_set_num_threads(atoi(argv[2]));
    
    int count = 0;
    int *a = (int *)malloc(sizeof(int)*n);
    int *b = (int *)malloc(sizeof(int)*n);
    int *c = (int *)malloc(sizeof(int)*n);
    for (int i=n; i > 0; i--) {
        a[i-1] = count;
        b[i-1] = count;
        c[i-1] = count;
        if((i % 8) == 0) {
            count++;
        }
    } 
    double start = omp_get_wtime();
    Serial_count_sort(a, n);
    double end_serial = omp_get_wtime() - start;
    start = omp_get_wtime();
    Parallel_count_sort(b, n);
    double end_parallel = omp_get_wtime() - start;
    start = omp_get_wtime();
    qsort(c, n, sizeof(int), cmpfunc);
    double end_qsort = omp_get_wtime() - start;
    bool equal = compareArrays(a, b, c, n);
    printf("Serial: %lf\nParallel: %lf\nQSort: %f\nEqual Answers?: %s\n", end_serial, end_parallel, end_qsort, equal ? "true":"false");

    return 0;
}

void Serial_count_sort(int a[], int n) {
   int i, j, count;
   int *temp = (int *)malloc(sizeof(int)*n);
   for (i = 0; i < n; i++) { 
      count = 0;
      for (j = 0; j < n; j++) { 
         if (a[j] < a[i])
            count++;
         else if (a[j] == a[i] && j < i)
            count++; 
      }
      temp[count] = a[i];
   } 
   memcpy(a, temp, sizeof(int)*n);
   free(temp);
}

void Parallel_count_sort(int a[], int n) {
   int i, j, count, threads;
   int *temp = (int *)malloc(sizeof(int)*n);
   #pragma omp parallel for private(count, j) shared(i, temp, a)
   for (i = 0; i < n; i++) {
      count = 0;
      for (j = 0; j < n; j++) { 
         if (a[j] < a[i])
            count++;
         else if (a[j] == a[i] && j < i)
            count++; 
      }
      #pragma omp critical
      temp[count] = a[i];
   } 
   memcpy(a, temp, sizeof(int)*n);
   free(temp);
}

int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

int compareArrays(int a[], int b[], int c[], int n) {
  int i;
  for(i = 0; i < n; i++) {
    if (a[i] != b[i] || a[i] != c[i]) return 0;
  }
  return 1;
}
