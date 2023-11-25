#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define SIZE 100

int array[SIZE];

void fillArrayWithRandomNumbers(int arr[SIZE]) {
    for(int i = 0; i<SIZE; i++) array[i] = rand()%100;
}

void printArray(int arr[SIZE]) {
    for(int i = 0; i<SIZE; i++) printf("%5d", arr[i]);
    printf("\n");
}

typedef struct StartEndIndexes {
    int start;
    int end;
} StartEndIndexes;

// Runs mergesort on the array segment described in the
// argument. Spawns two threads to mergesort each half
// of the array segment, and then merges the results.
void merge(int arr[SIZE], int start, int mid, int end) {
    int arr_half1 = mid - start + 1;
    int arr_half2 = end - mid;

    int left[arr_half1], right[arr_half2];

    int i, j;
    for (i = 0; i < arr_half1; i++){
        left[i] = arr[start + 1];
    }
    for (j = 0; j < arr_half2; j++){
        right[j] = arr[mid + 1 + j];
    }

    i = 0;
    j = 0;
    int k = start;

    while (i < arr_half1 && j < arr_half2){
        if (left[i] <= right[j]){
            arr[k] = left[i];
            i++;
        }
        else{
            arr[k] = right[j];
            j++;
        }
        k++;
    }

    while (i < arr_half1){
        arr[k] = left[i];
        i++;
        k++;
    }

    while(j < arr_half2){
        arr[k] = right[j];
        j++;
        k++;
    }
}

void* mergeSort(void* args){
    StartEndIndexes* sei = (StartEndIndexes*)args;
    int start = sei -> start;
    int end = sei -> end;

    if (start < end){
        int mid = start + (end - start) / 2;

        //create 2 threads to sort each half
        pthread_t l_thread, r_thread;

        StartEndIndexes l_sei, r_sei;
        l_sei.start = start;
        l_sei.end = mid;

        r_sei.start = mid + 1;
        r_sei.end = end;

        pthread_create(&l_thread, NULL, mergeSort, &l_sei);
        pthread_create(&r_thread, NULL, mergeSort, &r_sei);

        //wait for threads to finish
        pthread_join(l_thread, NULL);
        pthread_join(r_thread, NULL);

        //merge the sorted halves
        merge(array, start, mid, end);

    }

    return NULL;
}

int main() {
    srand(time(0));
    StartEndIndexes sei;
    sei.start = 0;
    sei.end = SIZE - 1;
    
    // 1. Fill array with random numbers.
    fillArrayWithRandomNumbers(array);
    
    // 2. Print the array.
    printf("Unsorted array: ");
    printArray(array);
    
    // 3. Create a 2 threads for merge sort.
    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, mergeSort, &sei);
    pthread_join(thread1, NULL);
    
    // 4. Wait for mergesort to finish.
    pthread_create(&thread2, NULL, mergeSort, &sei);
    pthread_join(thread2, NULL);
    
    // 5. Print the sorted array.
    printf("Sorted array:   ");
    printArray(array);
}
