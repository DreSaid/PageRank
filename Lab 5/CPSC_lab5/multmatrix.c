#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "engine.h"

#define  BUFSIZE 256

int main(void) {

    /* Variables */
    Engine* ep = NULL; // A pointer to a MATLAB engine object
    mxArray* Array1 = NULL, * result = NULL; // mxArray is the fundamental type underlying MATLAB data
    mxArray* Array2 = NULL;
    double matrixOne[3][3] = { { 1.0, 2.0, 3.0 }, {4.0, 5.0, 6.0 }, {7.0, 8.0, 9.0 } }; // Our test 'matrix', a 2-D array
    double matrixTwo[3][3] = { { 2.0, 2.0, 2.0 }, {3.0, 3.0, 3.0 }, {4.0, 4.0, 4.0 } };
    char buffer[BUFSIZE + 1];

    /* Starts a MATLAB process */
    if (!(ep = engOpen(NULL))) {
        fprintf(stderr, "\nCan't start MATLAB engine\n");
        system("pause");
        return 1;
    }

    Array1 = mxCreateDoubleMatrix(3, 3, mxREAL);
    Array2 = mxCreateDoubleMatrix(3, 3, mxREAL);

    memcpy((void*)mxGetPr(Array1), (void*)matrixOne, 9 * sizeof(double));
    memcpy((void*)mxGetPr(Array2), (void*)matrixTwo, 9 * sizeof(double));

    //
    if (engPutVariable(ep, "Array1", Array1)) {
        fprintf(stderr, "\nCannot write Array1 to MATLAB \n");
        system("pause");
        exit(1); // Same as return 1;
    }


    //PRINTING matrixOne

    //printf("\nRetrieving Array1\n");
    if ((Array1 = engGetVariable(ep, "Array1")) == NULL) {
        fprintf(stderr, "\nFailed to retrieve Array1 matrix\n");
        system("pause");
        exit(1);
    }
    else {
        size_t sizeOfArray1 = mxGetNumberOfElements(Array1);
        size_t i = 0;
        size_t j = 0;
        printf("The first matrix was:\n");
        for (i = 0; i < 3; ++i) {
            for (j = 0; j < 3; ++j) {
                printf("%f ", *(mxGetPr(Array1) + j * 3 + i));
            }
            printf("\n");
        }
    }

    if (engPutVariable(ep, "Array2", Array2)) {
        fprintf(stderr, "\nCannot write Array2 to MATLAB \n");
        system("pause");
        exit(1); // Same as return 1;
    }

    //PRINTING matrixTwo

    //printf("\nRetrieving Array2\n");
    if ((Array2 = engGetVariable(ep, "Array2")) == NULL) {
        fprintf(stderr, "\nFailed to retrieve Array2 matrix\n");
        system("pause");
        exit(1);
    }
    else {
        size_t sizeOfArray2 = mxGetNumberOfElements(Array2);
        size_t i = 0;
        size_t j = 0;
        printf("The second matrix was:\n");
        for (i = 0; i < 3; ++i) {
            for (j = 0; j < 3; ++j) {
                printf("%f ", *(mxGetPr(Array2) + j * 3 + i));
            }
            printf("\n");
        }
    }


    if (engEvalString(ep, "Product = Array1 * Array2")) {
        fprintf(stderr, "\nError calculating product  \n");
        system("pause");
        exit(1);
    }

    //printf("\nRetrieving Product\n");
    if ((result = engGetVariable(ep, "Product")) == NULL) {
        fprintf(stderr, "\nFailed to retrieve Product matrix\n");
        system("pause");
        exit(1);
    }
    else {
        size_t sizeOfResult = mxGetNumberOfElements(result);
        size_t i = 0;
        size_t j = 0;
        printf("The matrix product was:\n");
        for (i = 0; i < 3; ++i) {
            for (j = 0; j < 3; ++j) {
                printf("%f ", *(mxGetPr(result) + j * 3 + i));
            }
            printf("\n");
        }
    }


    if (engOutputBuffer(ep, buffer, BUFSIZE)) {
        fprintf(stderr, "\nCan't create buffer for MATLAB output\n");
        system("pause");
        return 1;
    }
    buffer[BUFSIZE] = '\0';


    engEvalString(ep, "whos"); // whos is a handy MATLAB command that generates a list of all current variables
    printf("%s\n", buffer);


    mxDestroyArray(Array1);
    mxDestroyArray(Array2);
    mxDestroyArray(result);
    Array1 = NULL;
    Array2 = NULL;
    result = NULL;
    if (engClose(ep)) {
        fprintf(stderr, "\nFailed to close MATLAB engine\n");
    }

    system("pause"); // So the terminal window remains open long enough for you to read it
    return 0; // Because main returns 0 for successful completion
}