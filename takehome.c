#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "engine.h"

#define  MAX_MATRIX_SIZE 500
#define  BUFSIZE 256

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

// TODO
void display_transposed(char name[], int matrix_size, Engine* ep) {
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
    // need to transpose the matrix before displaying
    double** matrix = (double**)malloc(matrix_size * sizeof(double*));
    int row = 0;
    int col = 0;
    for (i = 0; i < sizeOfResult; ++i) {
      matrix[row] = (double*)malloc(matrix_size * sizeof(double));
      printf("%f ", *(mxGetPr(result) + i));
      matrix[row][col] = *(mxGetPr(result) + i);
      if ((i + 1) % matrix_size == 0) {
        printf("\n");
        row++;
      }
      col++;
    }

    //
    printf("\ntransposed matrix\n");
    for (int i = 0; i < matrix_size; i++) {
      for (int j = 0; j < matrix_size; j++) {
        printf("%f ", matrix[i][j]);
      }
      printf("\n");
    }
  }

  // free dynamic memory (gives an error in Visual Studio)
  // for (int i = 0; i < matrix_size; i++) {
  //  free(matrix[i]);
  // }
  // free(matrix);
  // fclose(file_pointer);
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

  for (int i = 0; i < matrix_size; i++) {
    for (int j = 0; j < matrix_size; j++) {
      printf("%f ", matrix[i][j]);
    }
    printf("\n");
  }

  //////////////////////////////////////////////////
  // MATLAB

  /* Variables */
  Engine* ep = NULL; // A pointer to a MATLAB engine object
  mxArray* testArray = NULL, * result = NULL; // mxArray is the fundamental type underlying MATLAB data


  mxArray* m1 = NULL, * m2 = NULL;
  //double matrixOne[3][3] = { { 1.0, 2.0, 3.0 }, {4.0, 5.0, 6.0 }, {7.0, 8.0, 9.0 } }; // Our test 'matrix', a 2-D array
  //double matrixTwo[3][3] = { { 1.0, 2.0, 3.0 }, {4.0, 5.0, 6.0 }, {7.0, 8.0, 9.0 } }; // Our test 'matrix', a 2-D array
  //double matrixResult[3][3] = mtimes(matrixOne, matrixTwo);

  // char buffer[BUFSIZE + 1];


  /* Starts a MATLAB process */
  if (!(ep = engOpen(NULL))) {
    fprintf(stderr, "\nCan't start MATLAB engine\n");
    system("pause");
    return 1;
  }

  // // capture MATLAB output directly in order to echo it
  // if (engOutputBuffer(ep, buffer, BUFSIZE)) {
  //   fprintf(stderr, "\nCan't create buffer for MATLAB output\n");
  //   system("pause");
  //   return 1;
  // }
  // buffer[BUFSIZE] = '\0';

  // Matlab-friendly var for our test data
  m1 = mxCreateDoubleMatrix(1, matrix_size, mxREAL);
  m2 = mxCreateDoubleMatrix(1, matrix_size, mxREAL);
  // copy data from local 2D array time to Matlab var
  memcpy((void*)mxGetPr(m1), (void*)matrix[0], matrix_size * sizeof(double));
  // memcpy((void*)mxGetPr(m2), (void*)matrix[1], matrix_size * sizeof(double));
  
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
      fprintf(stderr, "\nError matrix mult  \n");
      system("pause");
      exit(1);
    }

    // testing
    display_matrix("m2", matrix_size, ep);
 
    printf("\nRetrieving m1\n");
    if ((result = engGetVariable(ep, "m1")) == NULL) {
      fprintf(stderr, "\nFailed to retrieve m1\n");
      system("pause");
      exit(1);
    }
    else {
      size_t sizeOfResult = mxGetNumberOfElements(result);
      size_t i = 0;
      printf("size of result: %d\n", sizeOfResult);
      printf("m1 is: \n");
      for (i = 0; i < sizeOfResult; ++i) {
        printf("%f ", *(mxGetPr(result) + i));
        if ((i + 1) % matrix_size == 0) {
          printf("\n");
        }
      }
    }
  }

  if (engEvalString(ep, "mult = m1")) {
    fprintf(stderr, "\nError matrix mult  \n");
    system("pause");
    exit(1);
  }
  

  /*printf("\nRetrieving m1\n");
  if ((result = engGetVariable(ep, "m1")) == NULL) {
    fprintf(stderr, "\nFailed to retrieve m1\n");
    system("pause");
    exit(1);
  }
  else {
    size_t sizeOfResult = mxGetNumberOfElements(result);
    size_t i = 0;
    printf("size of result: %d\n", sizeOfResult);
    printf("m1 is: \n");
    for (i = 0; i < sizeOfResult; ++i) {
      printf("%f\n", *(mxGetPr(result) + i));
    }
  }

  printf("\nRetrieving m2\n");
  if ((result = engGetVariable(ep, "m2")) == NULL) {
    fprintf(stderr, "\nFailed to retrieve m2\n");
    system("pause");
    exit(1);
  }
  else {
    size_t sizeOfResult = mxGetNumberOfElements(result);
    size_t i = 0;
    printf("size of result: %d\n", sizeOfResult);
    printf("m2 is: \n");
    for (i = 0; i < sizeOfResult; ++i) {
      printf("%f\n", *(mxGetPr(result) + i));
    }
  }*/

  //send the result of matlab calculation to our C program
  printf("\nRetrieving mult\n");
  if ((result = engGetVariable(ep, "mult")) == NULL) {
    fprintf(stderr, "\nFailed to retrieve mult\n");
    system("pause");
    exit(1);
  }
  else {
    size_t sizeOfResult = mxGetNumberOfElements(result);
    size_t i = 0;
    printf("size of result: %d\n", sizeOfResult);
    printf("mult is: \n");
    for (i = 0; i < sizeOfResult; ++i) {
      printf("%f ", *(mxGetPr(result) + i));
      if ((i + 1) % matrix_size == 0) {
        printf("\n");
      }
    }
  }


  // // use Matlab output buffer -  buffer is automatically cleared and refilled with the results of the command contained in the string passed to the engine
  // engEvalString(ep, "whos"); // whos is a handy MATLAB command that generates a list of all current variables
  // printf("%s\n", buffer);

  // free our memory and close our connection to MATLAB
  mxDestroyArray(testArray);
  mxDestroyArray(result);
  testArray = NULL;
  result = NULL;
  if (engClose(ep)) {
    fprintf(stderr, "\nFailed to close MATLAB engine\n");
  }

  // free dynamic memory in C program (gives an error in Visual Studio)
  free(line_buffer);
  // for (int i = 0; i < matrix_size; i++) {
  //  free(matrix[i]);
  // }
  // free(matrix);
  // fclose(file_pointer);


  system("pause"); // So the terminal window remains open long enough for you to read it
  return 0; // Because main returns 0 for successful completion
}


