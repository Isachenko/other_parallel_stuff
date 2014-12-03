#include <stdio.h>
#include <stdlib.h>

void generateVector(int size, FILE* file) {
    int *vector = new int[size];
    for (int i = 0; i < size; ++i) {
        vector[i] = 11 + rand() % 85;
    }
    fprintf(file, "%d\n", size);
    for(int i = 0; i < size; ++i) {
        if (vector[i] != 0) {
            fprintf(file, "%d ", vector[i]);
        }
    }
}


int main(int argc, char** argv)
{
    if (argc != 3) {
        printf("please enter vectro size and fileName\n");
        return 0;
    }
    int size;
    sscanf(argv[1], "%d", &size);
    char* fileName = argv[2];
    FILE* file = fopen(fileName, "w");
    generateVector(size, file);
    fclose(file);
    printf("vector generation complite\n");
    return 0;
}

