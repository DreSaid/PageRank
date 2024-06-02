#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "engine.h"

#define  BUFSIZE 256

int get_web_dimension(FILE* web_file);
int** parse_web(FILE* web_file, int dimension);

int main(void) {


    FILE* web_file;
    int **web_matrix = NULL;
    int dimension = 0;
    int count = 0;

    Engine* ep = NULL;
    mxArray* ConnectivityMatrix = NULL,* Pagerank = NULL;
    char buffer[BUFSIZE + 1];

    char line_buffer[BUFSIZE];

    web_file = fopen("web.txt", "r");

    if (web_file) {
        dimension = ((get_web_dimension(web_file)) + 1)/2;
        web_matrix = parse_web(web_file, dimension);
    }

    else {
        fprintf(stderr, "Unable to parse web file: %s\n", web_file);
        system("pause");
    }

    //testing line for dimension
    //printf("%d\n", dimension);
    
    //testing lines for file scan -> matrix
    /*for (int j = 0; j < dimension; j++) {
        for (int i = 0; i < dimension; i++) {
            printf("%d", web_matrix[j][i]);
        }
        printf("\n");
    }*/

    

    /*starts a MATLAB process*/
    if (!(ep = engOpen(NULL))) {
        fprintf(stderr, "\nCan't start MATLAB engine\n");
        system("pause");
        return 1;
    }

    ConnectivityMatrix = mxCreateDoubleMatrix(dimension, dimension, mxREAL);

    //memcpy((void*)mxGetPr(ConnectivityMatrix), (void*)web_matrix, dimension * sizeof(double));

    //replacement for the above commented line
    double* destPtr = mxGetPr(ConnectivityMatrix);
    for (int i = 0; i < dimension; i++) {
        for (int j = 0; j < dimension; j++) {
            destPtr[i * dimension + j] = (double)web_matrix[j][i];
        }
    }

    if (engPutVariable(ep, "ConnectivityMatrix", ConnectivityMatrix)) {
        fprintf(stderr, "\nCannot write Connectivity Matrix to MATLAB \n");
        system("pause");
        exit(1); // Same as return 1;
    }
    //testing lines for checking if connectivity matrix got transferred properly to MATLAB
    /*if ((ConnectivityMatrix = engGetVariable(ep, "ConnectivityMatrix")) == NULL) {
        fprintf(stderr, "\nFailed to retrieve ConnectivityMatrix\n");
        system("pause");
        exit(1);
    }
    else {
        size_t sizeOfResult = mxGetNumberOfElements(ConnectivityMatrix);
        for (size_t i = 0; i < sizeOfResult; ++i) {
            //printf("%d", i + 1);
            printf("%f", *(mxGetPr(ConnectivityMatrix) + i));
            printf("\n");
        }
    }*/

    //Run MATLAB commands
    if(engEvalString(ep, "[rows, columns] = size(ConnectivityMatrix);"
        "dimension = size(ConnectivityMatrix, 1);"
        "columnsums = sum(ConnectivityMatrix, 1);"
        "p = 0.85;"
        "zerocolumns = find(columnsums~=0);"
        "D = sparse( zerocolumns, zerocolumns, 1./columnsums(zerocolumns), dimension, dimension);"
        "StochasticMatrix = ConnectivityMatrix * D;"
        "[row, column] = find(columnsums==0);"
        "StochasticMatrix(:, column) = 1./dimension;"
        "Q = ones(dimension, dimension);"
        "TransitionMatrix = p * StochasticMatrix + (1 - p) * (Q/dimension);"
        "Pagerank = ones(dimension, 1);"
        "for i = 1:100 Pagerank = TransitionMatrix * Pagerank; end;"
        "Pagerank = Pagerank / sum(Pagerank);")) {
        fprintf(stderr, "\n MATLAB Error");
        system("pause");
        exit(1);
    }

    if ((Pagerank = engGetVariable(ep, "Pagerank")) == NULL) {
        fprintf(stderr, "\nFailed to retrieve Pagerank vector\n");
        system("pause");
        exit(1);
    }
    else {
        size_t sizeOfResult = mxGetNumberOfElements(Pagerank);
        printf("NODE  RANK\n---   ----\n");
        for (size_t i = 0; i < sizeOfResult; ++i) {
            printf("%d     ", i + 1);
            printf("%.4f", *(mxGetPr(Pagerank) + i));
            printf("\n");
        }
    }


    if (engOutputBuffer(ep, buffer, BUFSIZE)) {
        fprintf(stderr, "\nCan't create buffer for MATLAB output\n");
        system("pause");
        return 1;
    }

    mxDestroyArray(ConnectivityMatrix);
    mxDestroyArray(Pagerank);
    ConnectivityMatrix = NULL;
    Pagerank = NULL;


    if (engClose(ep)) {
        fprintf(stderr, "\nFailed to close MATLAB engine\n");
    }

    fclose(web_file);

    return 0; // Return success
}


//function for getting dimension of the web
int get_web_dimension(FILE* web_file) {

    int  dimension = 0;
    char line_buffer[BUFSIZE];

    dimension = strlen(fgets(line_buffer, BUFSIZE, web_file));

    /* You don't need to know this.  It 'resets' the file's internal pointer to the
       beginning of the file. */
    fseek(web_file, 0, SEEK_SET);

    /* Checks if text file was created in Windows and contains '\r'
       IF TRUE reduce strlen by 2 in order to omit '\r' and '\n' from each line
       ELSE    reduce strlen by 1 in order to omit '\n' from each line */
    if (strchr(line_buffer, '\r') != NULL) {
        return strlen(line_buffer) - 2;
    }
    else {
        return strlen(line_buffer) - 1;
    }
}

//function for parsing the web (and dynamically allocating the required memory)
int** parse_web(FILE * web_file, int dimension)
{
    /* Variables */
    char        line_buffer[BUFSIZE];
    int         row = 0;
    int         column = 0;
    int** web_matrix = NULL;
    int index = 0;

    /* Allocates memory for correctly-sized maze */
    // INSERT CODE HERE (1 line)
    web_matrix = (int**)calloc(dimension, sizeof(int*));

    for (row = 0; row < dimension; ++row) {
        // INSERT CODE HERE (1 line)
        web_matrix[row] = (int*)calloc(dimension, sizeof(int));
    }

    /* Copies maze file to memory */
    row = 0;
    while (fgets(line_buffer, BUFSIZE, web_file)) {
        for (column = 0; column < dimension; column++) {
            // INSERT CODE HERE (2 lines)
            web_matrix[row][column] = line_buffer[index] - '0'; //copies from buffer (which gets its values from the fgets)
            index += 2;
        }
        index = 0;
        row++;
    }
    return web_matrix;
}

