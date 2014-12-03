#include "matrixGenerator.h"
#include "paramparser.h"
#include "sparsematrix.h"
#include <string.h>
#include <stdio.h>

//options
bool rnd = 1;
int size = 2;
int threadCount = 1;
double zeroesProbability = 0.5;
int blockSize = 1;
//matrix and vector;
SparseMatrix *matrix;
int *vector;
int *result;

void setOptions(int *argc, char ***argv) {
    ParamParser pp(argc, argv);
    char* tmp;
    if (pp.getParam("-rnd", &tmp)) {
        rnd = atoi(tmp);
    }
    if (pp.getParam("-sz", &tmp)) {
        size = atoi(tmp);
    }
    if (pp.getParam("-tc", &tmp)) {
        threadCount = atoi(tmp);
    }
    if (pp.getParam("-zp", &tmp)) {
        zeroesProbability = atoi(tmp);
    }
    if (pp.getParam("-bs", &tmp)) {
        blockSize = atoi(tmp);
    }
    return;
}


void init() {
    matrix = new SparseMatrix(generateMatrix(size), size, threadCount, blockSize);
    vector = generateVector(size);
    result = new int[size];
    memset(result, 0, sizeof(int) * size);
}

void output(int* ans) {
    printf("RESULTS:\n");
    bool print_res = true;
    if (print_res) {
        printf("res:\n");
        for(int i = 0; i < size; ++i) {
            printf("%d ", ans[i]);
        }
        printf("\n");
    }

}

int main(int argc, char** argv)
{
    printf("start\n");
    printf("set options start\n");
    setOptions(&argc, &argv);
    printf("set options finis\n");
    printf("init start\n");
    init();
    printf("init finish\n");
    matrix->printMe();
    printf("vector:\n");
    for(int i = 0; i < size; ++i) {
        printf("%d ", vector[i]);
    }
    printf("\n");
    printf("mult start\n");
    int* result = matrix->multOnVector(vector);
    printf("mult finish\n");
    output(result);






    return 0;
}

