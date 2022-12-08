#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "engine.h"

#define  MAX_MATRIX_SIZE 500

int main(void) {

  //////////////////////////////////////////////////
  // Extract Connectivity Matrix from web.txt
  int matrix_size = 0;
  char* line_buffer = (char*) malloc(MAX_MATRIX_SIZE*sizeof(char));
  FILE* file_pointer = NULL;
  fopen_s(&file_pointer, "web.txt", "r");
  //file_pointer = fopen_s(&file_pointer, "web.txt", "r");
  if (file_pointer == NULL) {
    printf("error opening web.txt");
    return 1;
  }

  // count number of lines in web.txt = matrix size
  fseek(file_pointer, 0, SEEK_SET); // Move file_pointer to the beginning of the file
  while (fgets(line_buffer, MAX_MATRIX_SIZE, file_pointer) != NULL) {
    matrix_size++;
  }

  // store matrix from web.txt
  int** matrix = (int**) malloc (matrix_size*sizeof(int*));
  int row = 0;                      // matrix row
  int col = 0;                      // matrix col
  int k = 0;                        // line_buffer_index
  // char temp[MAX_MATRIX_SIZE] = "";   // temp string containing integers read from line_buffer
  // int temp_index = 0;
  fseek(file_pointer, 0, SEEK_SET); // Move file_pointer to the beginning of the file
  while (fgets(line_buffer, MAX_MATRIX_SIZE, file_pointer) != NULL) {
    // allocate matrix row
    matrix[row] = (int*) malloc(matrix_size*sizeof(int));
    // // reset temp
    // temp_index = 0;
    // strcpy_s(temp, sizeof(temp), "");
    //// skip any leadings spaces in line_buffer
    //while (line_buffer[k] == " ") {
    //  k++;
    //}
    printf("k = %d\n", k);
    while (line_buffer[k] != '\n' && k < MAX_MATRIX_SIZE) {
      if (line_buffer[k] == '1' || line_buffer[k] == '0') {
        // convert to int and store in matrix
        matrix[row][col] = (line_buffer[k] == '1') ? 1 : (line_buffer[k] == '0') ? 0 : 8; // convert temp string to int
        col++;
        k++;
      }
      else {
        k++;
      }
    }
    row++;
    k = 0;
    col = 0;
  }

  for (int i = 0; i < matrix_size; i++) {
    for (int j = 0; j < matrix_size; j++) {
      printf("%d ", matrix[i][j]);
    }
    printf("\n");
  }
  

  free(line_buffer);
  //for (int i = 0; i < matrix_size; i++) {
  //  free(matrix[i]);
  //}
  //free(matrix);
  //fclose(file_pointer);

  system("pause"); // So the terminal window remains open long enough for you to read it
  return 0; // Because main returns 0 for successful completion
}

