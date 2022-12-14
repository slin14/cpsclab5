#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "engine.h"

#define  MAX_MATRIX_SIZE 500
#define  BUFSIZE 256

// Function to display a matlab matrix in terminal
void display_matrix(char name[], int matrix_size, Engine* ep) {
  mxArray* result = NULL;
  printf("\nRetrieving %s\n", name);
  if ((result = engGetVariable(ep, name)) == NULL) {
    fprintf(stderr, "\nFailed to retrieve %s\n", name);
    system("pause");
    exit(1);
  }
  else {
    size_t sizeOfResult = mxGetNumberOfElements(result);
    size_t i = 0;
    printf("size of result: %d\n", sizeOfResult);
    printf("%s is: \n", name);
    for (i = 0; i < sizeOfResult; ++i) {
      printf("%f ", *(mxGetPr(result) + i));
      if ((i + 1) % matrix_size == 0) {
        printf("\n");
      }
    }
  }
}

int main(void) {
  //////////////////////////////////////////////////
  // Extract Connectivity Matrix from web.txt
  int matrix_size = 0;
  char* line_buffer = (char*)malloc(MAX_MATRIX_SIZE * sizeof(char));
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
  double** matrix = (double**)malloc(matrix_size * sizeof(double*));
  int row = 0;                      // matrix row
  int col = 0;                      // matrix col
  int k = 0;                        // line_buffer_index
  fseek(file_pointer, 0, SEEK_SET); // Move file_pointer to the beginning of the file
  while (fgets(line_buffer, MAX_MATRIX_SIZE, file_pointer) != NULL) {
    matrix[row] = (double*)malloc(matrix_size * sizeof(double)); // allocate matrix row
    while (line_buffer[k] != '\n' && k < MAX_MATRIX_SIZE) {
      if (line_buffer[k] == '1' || line_buffer[k] == '0') {
        // convert to int and store in matrix
        matrix[row][col] = (line_buffer[k] == '1') ? 1 : 0;
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

  printf("Connectivity Matrix:\n");
  for (int i = 0; i < matrix_size; i++) {
    for (int j = 0; j < matrix_size; j++) {
      printf("%f ", matrix[i][j]);
    }
    printf("\n");
  }

  //////////////////////////////////////////////////
  // MATLAB
  Engine* ep = NULL; // A pointer to a MATLAB engine object
  mxArray* result = NULL; // mxArray is the fundamental type underlying MATLAB data
  mxArray* m1 = NULL, * m2 = NULL;

  if (!(ep = engOpen(NULL))) { /* Starts a MATLAB process */
    fprintf(stderr, "\nCan't start MATLAB engine\n");
    system("pause");
    return 1;
  }
  m1 = mxCreateDoubleMatrix(1, matrix_size, mxREAL);
  m2 = mxCreateDoubleMatrix(1, matrix_size, mxREAL);
  // copy data from local 2D array time to Matlab var
  memcpy((void*)mxGetPr(m1), (void*)matrix[0], matrix_size * sizeof(double));

  // place the test array into the MATLAB workspace
  if (engPutVariable(ep, "m1", m1)) {
    fprintf(stderr, "\nCannot write m1 to MATLAB \n");
    system("pause");
    exit(1); // Same as return 1;
  }

  // construct matrix in matlab from row vectors
  for (int i = 1; i < matrix_size; i++) {
    memcpy((void*)mxGetPr(m2), (void*)matrix[i], matrix_size * sizeof(double));
    // store current row in matlab
    if (engPutVariable(ep, "m2", m2)) {
      fprintf(stderr, "\nCannot write m2 to MATLAB \n");
      system("pause");
      exit(1); // Same as return 1;
    }
    // append current row to matlab matrix
    if (engEvalString(ep, "m1 = [m1; m2]")) {
      fprintf(stderr, "\nError appending row to ConnectivityMatrix \n");
      system("pause");
      exit(1);
    }
    // testing
    //display_matrix("m2", matrix_size, ep);
    //display_matrix("m1", matrix_size, ep);
  }

  // Execute pagerank in MATLAB
  if (engEvalString(ep, "ConnectivityMatrix = m1")) {
    fprintf(stderr, "\nError executing matlab instruction\n");
    system("pause");
    exit(1);
  }
  if (engEvalString(ep, "dimension = size(ConnectivityMatrix, 1)")) {
    fprintf(stderr, "\nError executing matlab instruction\n");
    system("pause");
    exit(1);
  }
  if (engEvalString(ep, "columnsums = sum(ConnectivityMatrix, 1)")) {
    fprintf(stderr, "\nError executing matlab instruction\n");
    system("pause");
    exit(1);
  }
  if (engEvalString(ep, "p = 0.85")) {
    fprintf(stderr, "\nError executing matlab instruction\n");
    system("pause");
    exit(1);
  }
  if (engEvalString(ep, "zerocolumns = find(columnsums~=0)")) {
    fprintf(stderr, "\nError executing matlab instruction\n");
    system("pause");
    exit(1);
  }
  if (engEvalString(ep, "D = sparse( zerocolumns, zerocolumns, 1./columnsums(zerocolumns), dimension, dimension)")) {
    fprintf(stderr, "\nError executing matlab instruction\n");
    system("pause");
    exit(1);
  }
  if (engEvalString(ep, "StochasticMatrix = ConnectivityMatrix * D")) {
    fprintf(stderr, "\nError executing matlab instruction\n");
    system("pause");
    exit(1);
  }
  if (engEvalString(ep, "[row, column] = find(columnsums==0)")) {
    fprintf(stderr, "\nError executing matlab instruction\n");
    system("pause");
    exit(1);
  }
  if (engEvalString(ep, "StochasticMatrix(:, column) = 1./dimension")) {
    fprintf(stderr, "\nError executing matlab instruction\n");
    system("pause");
    exit(1);
  }
  if (engEvalString(ep, "Q = ones(dimension, dimension);")) {
    fprintf(stderr, "\nError executing matlab instruction\n");
    system("pause");
    exit(1);
  }
  if (engEvalString(ep, "TransitionMatrix = p * StochasticMatrix + (1 - p) * (Q/dimension)")) {
    fprintf(stderr, "\nError executing matlab instruction\n");
    system("pause");
    exit(1);
  }
  if (engEvalString(ep, "PageRank = ones(dimension, 1);")) {
    fprintf(stderr, "\nError executing matlab instruction\n");
    system("pause");
    exit(1);
  }
  if (engEvalString(ep, "for i = 1:100 PageRank = TransitionMatrix * PageRank; end")) {
    fprintf(stderr, "\nError executing matlab instruction\n");
    system("pause");
    exit(1);
  }
  if (engEvalString(ep, "PageRank = PageRank / sum(PageRank)")) {
    fprintf(stderr, "\nError executing matlab instruction\n");
    system("pause");
    exit(1);
  }

  //send the result of matlab calculation to our C program
  display_matrix("PageRank", matrix_size, ep);

  // free our memory and close our connection to MATLAB
  mxDestroyArray(result);
  result = NULL;
  mxDestroyArray(m1);
  m1 = NULL;
  mxDestroyArray(m2);
  m2 = NULL;
  if (engClose(ep)) {
    fprintf(stderr, "\nFailed to close MATLAB engine\n");
  }

  // free dynamic memory in C program (commented out because free gives an error in Visual Studio)
  free(line_buffer);
  for (int i = 0; i < matrix_size; i++) {
    free(matrix[i]);
  }
  free(matrix);
  fclose(file_pointer);


  system("pause"); // So the terminal window remains open long enough for you to read it
  return 0; // Because main returns 0 for successful completion
}
