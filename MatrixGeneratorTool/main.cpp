#include <stdio.h>
#include <stdlib.h>

int* generateVector(int size) {
    int *vector = new int[size];
    for (int i = 0; i < size; ++i) {
        vector[i] = 11 + rand() % 85;
        if ((vector[i] % 2) == 0) {
            vector[i] = 0;
        }
    }
    return vector;
}

void generateMatrix(int size, FILE* file) {
    fprintf(file, "%d\n", size);
    for (int i = 0; i < size; ++i) {
        int *vector = generateVector(size);
        int realSize = 0;
        for(int i = 0; i < size; ++i) {
            if (vector[i] != 0) {
                realSize += 1;
            }
        }
        fprintf(file, "%d ", realSize);
        for(int i = 0; i < size; ++i) {
            if (vector[i] != 0) {
                fprintf(file, "%d %d ", i, vector[i]);
            }
        }
        fprintf(file, "\n");
    }
}


int main(int argc, char** argv)
{
    if (argc != 3) {
        printf("please enter matrix size and fileName\n");
        return 0;
    }
    printf("Don't forget that matrix generate as spare columns\n");
    int size;
    sscanf(argv[1], "%d", &size);
    char* fileName = argv[2];
    FILE* file = fopen(fileName, "w");
    generateMatrix(size, file);
    fclose(file);
    printf("matrix generation complite\n");
    return 0;
}

