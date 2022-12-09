#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "engine.h"

#define  BUFSIZE 256

int main(void) {
  /* Variables */
  Engine* ep = NULL; // A pointer to a MATLAB engine object
  mxArray* testArray = NULL, * result = NULL; // mxArray is the fundamental type underlying MATLAB data
  //double time[3][3] = { { 1.0, 2.0, 3.0 }, {4.0, 5.0, 6.0 }, {7.0, 8.0, 9.0 } }; // Our test 'matrix', a 2-D array


  mxArray* m1 = NULL, * m2 = NULL;
  double matrixOne[3][3] = { { 1.0, 2.0, 3.0 }, {4.0, 5.0, 6.0 }, {7.0, 8.0, 9.0 } }; // Our test 'matrix', a 2-D array
  double matrixTwo[3][3] = { { 1.0, 2.0, 3.0 }, {4.0, 5.0, 6.0 }, {7.0, 8.0, 9.0 } }; // Our test 'matrix', a 2-D array
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
  m1 = mxCreateDoubleMatrix(3, 3, mxREAL);
  m2 = mxCreateDoubleMatrix(3, 3, mxREAL);
  // copy data from local 2D array time to Matlab var
  //memcpy((void*) mxGetPr(testArray), (void *)time, 9 * sizeof(double));
  memcpy((void*)mxGetPr(m1), (void*)matrixOne, 9 * sizeof(double));
  memcpy((void*)mxGetPr(m2), (void*)matrixTwo, 9 * sizeof(double));
  //memcpy((void*)mxGetPr(testArray), (void*)mtimes(matrixOne, matrixTwo), 9 * sizeof(double));
  // place the test array into the MATLAB workspace
  if (engPutVariable(ep, "m1", m1)) {
    fprintf(stderr, "\nCannot write m1 to MATLAB \n");
    system("pause");
    exit(1); // Same as return 1;
  }
  if (engPutVariable(ep, "m2", m2)) {
    fprintf(stderr, "\nCannot write m2 to MATLAB \n");
    system("pause");
    exit(1); // Same as return 1;
  }
  if (engEvalString(ep, "mult = mtimes(m1, m2)")) {
    fprintf(stderr, "\nError matrix mult  \n");
    system("pause");
    exit(1);
  }

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
  }

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
      printf("%f\n", *(mxGetPr(result) + i));
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

  system("pause"); // So the terminal window remains open long enough for you to read it
  return 0; // Because main returns 0 for successful completion
}

