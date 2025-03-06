//
// Created by Simone Mussini on 05/03/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define BUFF_SIZE 100
#define INT_BIT_SIZE 32

typedef enum {false, true} Boolean;

// Function that given an array of chars returns an array of the integers it contains
Boolean lineSplitter(int **arrPtr, int *arrSize, const char *line, const char delimiter, int int_size) {
  char currentInt[int_size];
  int i =0, j = 0;
  while (i < BUFF_SIZE - 1 && line[i] != '\0') {
    if ((line[i] >= '0' && line[i] <= '9') || line[i] == '-') {
      // If you are reading a part of an integer
      currentInt[j] = line[i];
      j++;
    } else {
      // If you have finished reading an integer
      currentInt[j] = '\0';
      (*arrSize)++;
      *arrPtr = (int*) realloc(*arrPtr, (*arrSize) * sizeof(int));
      (*arrPtr)[*arrSize - 1] = atoi(currentInt);
      if (line[i] == '\n' || line[i] == delimiter || line[i] == '\0') j = 0;
      else return false;
    }
    i++;
  }
  return true;
}

int compare(const void *a, const void *b) {
  return (*(int*)a - *(int*)b);
}

int main(int argc, char *argv[]) {
  int max_chars_for_int = pow(2, (int) log2(INT_BIT_SIZE) + 1); // +1 because of the terminator

  if (argc != 3) {
    printf("Wrong number of arguments\n");
    return 1;
  }

  printf("\nArguments are correct!");

  const char *input_filename = argv[1];
  const char *output_filename = argv[2];

  printf("\nInput file: %s", input_filename);
  printf("\nOutput file: %s", output_filename);

  int n = 0;
  int *arrPtr = NULL;

  // Read each line from the file
  printf("\nReading integers from file!");
  FILE *input_file = fopen(input_filename, "r");
  if (input_file == NULL) {
    printf("\nError opening input file!");
    return 1;
  }

  char line[BUFF_SIZE];
  while (fgets(line, BUFF_SIZE, input_file) != NULL) {
    if (!lineSplitter(&arrPtr, &n, line, ' ', max_chars_for_int)) {
      printf("\nError parsing line: %s\n", line);
      break;
    }
  }
  fclose(input_file);


  // Order array of integers
  printf("\nOrdering integers!");
  qsort(arrPtr, n, sizeof(int), compare);

  // Write the array on the output file
  printf("\nWriting integers to file!");
  FILE *output_file = fopen(output_filename, "w");
  if (output_file == NULL) {
    printf("\nError opening output file!");
    free(arrPtr);
    return 1;
  }

  for (int i = 0; i < n; i++) {
    fprintf(output_file, "%d", arrPtr[i]);
    fprintf(output_file, "\n");
  }
  fclose(output_file);

  free(arrPtr);

  return 0;
}