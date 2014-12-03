#ifndef MATRIXGENERATOR_H
#define MATRIXGENERATOR_H

#include "stdio.h"
#include <cstdlib>

int** generateMatrix(int size) {
    int **matrix = new int*[size];
    for (int i = 0; i < size; ++i) {
        matrix[i] = new int[size];
        for(int j = 0; j < size; j++) {
            matrix[i][j] = rand() % 10;
        }
    }
    return matrix;
}

int* generateVector(int size) {
    int *vector = new int[size];
    for (int i = 0; i < size; ++i) {
        vector[i] = rand() % 10;
    }
    return vector;
}



#endif // MATRIXGENERATOR_H
