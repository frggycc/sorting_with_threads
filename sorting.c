/*******************************
* CPSC351: Group Assignment #2
* Members: 
* 	Charlie Cardenas
* 	Chaitanya Talluri
* 	Ansh Tomar
* 	Hunter Tran
********************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define ARRAY_SIZE 11

// Array to sort and results of sorted array
int list[ARRAY_SIZE] = {7, 12, 19, 3, 18, 4, 2, -5, 6, 15, 8};
int sorted[ARRAY_SIZE];  

// Structure to send data to sorting threads
typedef struct{
    int* subArray;
    unsigned int size;
}SortingThreadParameters;

// Structure to keep track of the two sides of the array
typedef struct{
    SortingThreadParameters left;
    SortingThreadParameters right;
}MergingThreadParameters;

// Function to compare two integers for qsort
int compare(const void *a, const void *b) {
    int va = *(const int*) a;
    int vb = *(const int*) b;
    return (va > vb) - (va < vb);
}

// Thread function for sorting each half of the array using qsort
void *sortingThread(void *args) {
    SortingThreadParameters *oneSide = (SortingThreadParameters *)args;
    int *start = oneSide->subArray;
    int size = oneSide->size;
    qsort(start, size, sizeof(int), compare);
    pthread_exit(NULL);
}

// Thread function for merging the two sorted halves
void *mergingThread(void *args) {
    MergingThreadParameters *mergeParams = (MergingThreadParameters *)args;

    // Define variable to help merge
    int i = 0, j = 0, k = 0;
    int n1 = mergeParams->left.size;
    int n2 = mergeParams->right.size;

    // Start comparing values of both sides (left and right)
    while(i < n1 && j < n2){
        if(mergeParams->left.subArray[i] < mergeParams->right.subArray[j]){
            sorted[k] = mergeParams->left.subArray[i];
            k++;
            i++;
        }
        else{
            sorted[k] = mergeParams->right.subArray[j];
            k++;
            j++;
        }
    }

    // Runs when values remains on the left side but the
    // the right side is "empty"
    while(i < n1){
        sorted[k] = mergeParams->left.subArray[i];
        k++;
        i++;
    }

    // Runs when values remains on the right side but the
    // the left side is "empty"
    while(j < n1){
        sorted[k] = mergeParams->right.subArray[j];
        k++;
        j++;
    }

    pthread_exit(NULL);
}

int main() {
    // Printing out the original array
    printf("Original Array: { ");
    for (int i = 0; i < ARRAY_SIZE; i++) {
        printf("%d ", list[i]);
    }
    printf("}\n");

    // Define thread snd struct variables
    pthread_t thread1, thread2, merge_thread;
    SortingThreadParameters subLeft, subRight;
    MergingThreadParameters mergeLR;

    int mid = ARRAY_SIZE / 2;

    // Left side starts at array[0]; size = half of array size
    subLeft.subArray = list;
    subLeft.size = mid;
    
    // Right side starts at array[mid]; size = array size - mid
    subRight.subArray = &(list[mid]);
    subRight.size = ARRAY_SIZE - mid;

    // Test running through sub array 1
    printf("Left Side: ");
    for (int i = 0; i < subLeft.size; i++) {
        printf("%d ", subLeft.subArray[i]);
    }
    printf("\n");

    printf("Right Side: ");
    for (int i = 0; i < subRight.size; i++) {
        printf("%d ", subRight.subArray[i]);
    }
    printf("\n");

    // Sort both halves using two threads
    pthread_create(&thread1, NULL, sortingThread, (void *)&subLeft);
    pthread_create(&thread2, NULL, sortingThread, (void *)&subRight);

    // Wait for both sorting threads to complete
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // Print the partially sorted array
    printf("\n");
    printf("Partially Sorted Array\n");
    printf("Left Side: ");
    for (int i = 0; i < ARRAY_SIZE; i++) {
        if(i == mid){ 
            printf("\nRight Side: ");
        }
        printf("%d ", list[i]);
    }
    printf("\n");

    // From MergingThreadParameters mergeLR; variable
    mergeLR.left = subLeft;
    mergeLR.right = subRight;

    // Merge left and right together
    pthread_create(&merge_thread, NULL, mergingThread, (void *)&mergeLR);

    // Wait for the merge thread to complete
    pthread_join(merge_thread, NULL);

    // Print the fully sorted array
    printf("\n");
    printf("Fully Sorted Array: { ");
    for (int i = 0; i < ARRAY_SIZE; i++) {
        printf("%d ", sorted[i]);
    }
    printf("}\n");

    return 0;
}
