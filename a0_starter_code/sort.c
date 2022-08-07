// add headers as appropriate
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

// // following used merge sort algorithm online
// void merge(int arr[], int l, int m, int r)
// {
//     int i, j, k;
//     int n1 = m - l + 1;
//     int n2 = r - m;
  
//     /* create temp arrays */
//     int L[n1], R[n2];
  
//     /* Copy data to temp arrays L[] and R[] */
//     for (i = 0; i < n1; i++)
//         L[i] = arr[l + i];
//     for (j = 0; j < n2; j++)
//         R[j] = arr[m + 1 + j];
  
//     /* Merge the temp arrays back into arr[l..r]*/
//     i = 0; // Initial index of first subarray
//     j = 0; // Initial index of second subarray
//     k = l; // Initial index of merged subarray
//     while (i < n1 && j < n2) {
//         if (L[i] <= R[j]) {
//             arr[k] = L[i];
//             i++;
//         }
//         else {
//             arr[k] = R[j];
//             j++;
//         }
//         k++;
//     }
  
//     /* Copy the remaining elements of L[], if there
//     are any */
//     while (i < n1) {
//         arr[k] = L[i];
//         i++;
//         k++;
//     }
  
//     /* Copy the remaining elements of R[], if there
//     are any */
//     while (j < n2) {
//         arr[k] = R[j];
//         j++;
//         k++;
//     }
// }
  
// /* l is for left index and r is right index of the
// sub-array of arr to be sorted */
// void mergeSort(int arr[], int l, int r)
// {
//     if (l < r) {
//         // Same as (l+r)/2, but avoids overflow for
//         // large l and h
//         int m = l + (r - l) / 2;
  
//         // Sort first and second halves
//         mergeSort(arr, l, m);
//         mergeSort(arr, m + 1, r);
  
//         merge(arr, l, m, r);
//     }
// }

void swap(int *a, int *b) {
  int t = *a;
  *a = *b;
  *b = t;
}

// function to find the partition position
int partition(int array[], int low, int high) {
  
  // select the rightmost element as pivot
  int pivot = array[high];
  
  // pointer for greater element
  int i = (low - 1);

  // traverse each element of the array
  // compare them with the pivot
  for (int j = low; j < high; j++) {
    if (array[j] <= pivot) {
        
      // if element smaller than pivot is found
      // swap it with the greater element pointed by i
      i++;
      
      // swap element at i with element at j
      swap(&array[i], &array[j]);
    }
  }

  // swap the pivot element with the greater element at i
  swap(&array[i + 1], &array[high]);
  
  // return the partition point
  return (i + 1);
}

void quickSort(int array[], int low, int high) {
  if (low < high) {
    
    // find the pivot element such that
    // elements smaller than pivot are on left of pivot
    // elements greater than pivot are on right of pivot
    int pi = partition(array, low, high);
    
    // recursive call on the left of pivot
    quickSort(array, low, pi - 1);
    
    // recursive call on the right of pivot
    quickSort(array, pi + 1, high);
  }
}





int main(int argc, char *argv[])
{


    FILE *fprd = fopen("log.txt", "r");
    if(fprd == NULL){
        printf("log.txt is not a readable file");
        return 1; 
    }
    FILE *fpsd = fopen("sorted.txt", "w+");
    if (fpsd == NULL)
    {
        printf("Error! cannot open and write to file");
    }
    int numlines = 0;
    fscanf (fprd, "%d", &numlines);
    fprintf(fpsd, "%d \n", numlines);
    int readIn [numlines];
    int ct = 0;
    int anumber;
    while(fscanf (fprd, "%d", &anumber) == 1){
        
        readIn[ct] = anumber;
        // printf("%d time read in: %d\n", ct, anumber);
        ct++;
        // if(feof (fprd)){printf("incorrect number of line indicated in log.txt");break;}
    }
    if(numlines != ct) { printf("incorrect number of line indicated in log.txt"); return 1;}
    fclose (fprd);  

    // find out time 
    struct timeval tv;
    struct timezone tz;

    gettimeofday(&tv,&tz);
    int dur = tv.tv_usec;
    quickSort(readIn, 0, ct - 1); 
    gettimeofday(&tv,&tz);
    dur = tv.tv_usec - dur;
    for(int i = 0; i < ct; ++i){
        fprintf(fpsd, "%d \n", readIn[i]);
    }
    fclose (fpsd);

    printf("%d\n", dur);
    

    return 0;   
}
