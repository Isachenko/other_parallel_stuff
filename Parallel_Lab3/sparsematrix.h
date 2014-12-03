#pragma once

class SparseMatrix
{
private:
    struct Elem;

    struct Column;

    friend struct ParamStruct;
    friend struct WraperParamStruct;
    friend void* multOnVectorThreadFuncWraper(void* param);

    int size;
    int columnsCount;
    Column *columns;


    //params for parallel caunting
    int threadCount;
    int blockSize;

    void* multOnVectorThreadFunc(void* param);

public:
    SparseMatrix(int** matrix, int size, int threadCount, int blockSize);
    int getSize() const;
    int getColumnsCount() const;
    int* multOnVector(int* vector);
    void printMe();
};

