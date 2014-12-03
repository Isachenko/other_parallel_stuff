#include "sparsematrix.h"
#include <pthread.h>
#include <memory.h>
#include <stdio.h>

struct SparseMatrix::Elem{
    int value;
    int rowNum;
};

struct SparseMatrix::Column{
    int columnNum;
    int elemCount;
    Elem *elems;
};

struct ParamStruct{
    SparseMatrix::Column *fromColumn;
    SparseMatrix::Column *toColumn;
    int* vector;
    bool continueWork;
    ParamStruct(SparseMatrix::Column *fromColumn, SparseMatrix::Column *toColumn, int* vector, bool continueWork) :
        fromColumn(fromColumn),
        toColumn(toColumn),
        vector(vector),
        continueWork(continueWork)
    {
    }
    ParamStruct() :
        fromColumn(nullptr),
        toColumn(nullptr),
        vector(nullptr),
        continueWork(true)
    {
    }
};

struct WraperParamStruct{
    SparseMatrix* matrix;
    ParamStruct* param;
    WraperParamStruct(SparseMatrix* matrix, ParamStruct* param) :
        matrix(matrix), param(param)
    {

    }

};

ParamStruct globalParam;
int* globalAns;

pthread_cond_t condSlaveWaitForData = PTHREAD_COND_INITIALIZER;
pthread_cond_t condDataIsReady = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutexData = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexThreads= PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexAns = PTHREAD_MUTEX_INITIALIZER;

void* multOnVectorThreadFuncWraper(void* param) {
    WraperParamStruct* wraperParam = (WraperParamStruct*) param;
    ParamStruct* matrixParam = wraperParam->param;
    SparseMatrix* matrix = wraperParam->matrix;
    return matrix->multOnVectorThreadFunc(matrixParam);
}

//slave function
void* SparseMatrix::multOnVectorThreadFunc(void* param) {
    printf("slave thread start\n");
    ParamStruct* paramStruct = (ParamStruct*)param;
    int *vector = paramStruct->vector;
    Column *fromColumn = paramStruct->fromColumn;
    Column *toColeumn = paramStruct->toColumn;
    delete paramStruct;

    int* localResult = new int(size);
    memset(localResult, 0, size*sizeof(int));
    bool continueWork = true;

    while(continueWork) {
        for(Column *column = fromColumn; column != toColeumn; ++column) {
            for(int i = 0; i < column->elemCount; ++i) {
                Elem* elem = column->elems + i;
                localResult[elem->rowNum] += (elem->value) * vector[column->columnNum];
            }
        }
        printf("slave wait mutexThread\n");
        pthread_mutex_lock(&mutexThreads);
        printf("slave lock mutexThread\n");
        pthread_cond_signal(&condSlaveWaitForData);
        printf("slave signaled wantDataEvent\n");
        printf("slave wait DataIsReady and mutexData\n");
        pthread_cond_wait(&condDataIsReady, &mutexData);
        printf("slave lock mutexData\n");
        fromColumn = globalParam.fromColumn;
        toColeumn = globalParam.toColumn;
        continueWork = globalParam.continueWork;
        pthread_mutex_unlock(&mutexData);
        printf("slave unlock mutexData\n");
        pthread_mutex_unlock(&mutexThreads);
        printf("slave unlock mutexThread\n");
    }

    pthread_mutex_lock(&mutexAns);
    for(int i = 0; i < size; ++i) {
        globalAns[i] += localResult[i];
    }

    pthread_mutex_unlock(&mutexAns);

    return 0;
}

//master function
int *SparseMatrix::multOnVector(int *vector)
{
    printf("master thread start\n");
    pthread_t* pthreads = new pthread_t[threadCount];
    globalAns = new int[size];
    memset(globalAns, 0, size*sizeof(int));

    int curColumn = 0;
    for(int i = 0; i < threadCount; ++i) {
        int startColumn = curColumn;
        curColumn += blockSize;
        int endColumn = curColumn;
        if (endColumn > columnsCount) {
            endColumn = columnsCount;
        }
        ParamStruct* param = new ParamStruct(columns + startColumn, columns + endColumn, vector, true);
        WraperParamStruct* wraperParam = new WraperParamStruct(this, param);
        pthread_create((pthreads + i), NULL, multOnVectorThreadFuncWraper, (void*)wraperParam);
    }

    //data preparation
    int stopedThreadsCount = 0;
    while(stopedThreadsCount < threadCount) {
        int startColumn = curColumn;
        curColumn += blockSize;
        if (curColumn > columnsCount) {
            curColumn = columnsCount;
        }
        int endColumn = curColumn;
        Column* startColumnPtr = columns + startColumn;
        Column* endColumnPtr = columns + endColumn;
        bool continueWork = true;
        if (startColumn == endColumn) {
            continueWork = false;
            stopedThreadsCount += 1;
        }
        //wait until slave done its work
        printf("master wait condSlaveWaitForData and mutexData\n");
        pthread_cond_wait(&condSlaveWaitForData, &mutexData);
        printf("master lock mutexData\n");
        globalParam.fromColumn = startColumnPtr;
        globalParam.toColumn = endColumnPtr;
        globalParam.continueWork = continueWork;
        pthread_cond_signal(&condDataIsReady);
        printf("master signaled DataIsReady\n");
        pthread_mutex_unlock(&mutexData);
        printf("master unlocked mutexData\n");
    }

    for(int i = 0; i < threadCount; ++i) {
        pthread_join(pthreads[i], NULL);
    }




    return globalAns;
}

void SparseMatrix::printMe()
{
    for(int i = 0; i < columnsCount; ++i) {
        printf("column %d: ", columns[i].columnNum);
        for(int j = 0; j < columns[i].elemCount; ++j) {
            printf("{%d, %d} ", columns[i].elems[j].rowNum, columns[i].elems[j].value);
        }
        printf("\n");
    }
}

SparseMatrix::SparseMatrix(int** matrix, int size, int threadCount, int blockSize):
    size(size),
    threadCount(threadCount),
    blockSize(blockSize)
{
    columnsCount = 0;
    int* nonZeroColumns = new int[size];
    for(int i = 0; i < size; ++i){
        for(int j =0; j < size; ++j){
            if (matrix[i][j] != 0) {
                nonZeroColumns[columnsCount] = i;
                ++columnsCount;
                break;
            }
        }
    }

    columns = new Column[columnsCount];

    for(int i = 0; i < columnsCount; ++i){
        columns[i].elemCount = 0;
        columns[i].columnNum = nonZeroColumns[i];
        for(int j =0; j < size; ++j){
            if (matrix[i][j] != 0) {
                columns[i].elemCount += 1;
            }
        }
        columns[i].elems = new Elem[columns[i].elemCount];
        for(int j =0; j < size; ++j){
            if (matrix[i][j] != 0) {
                columns[i].elems[j].value = matrix[i][j];
                columns[i].elems[j].rowNum = j;
            }
        }
    }

    delete[] (nonZeroColumns);
}


int SparseMatrix::getSize() const
{
    return size;
}

int SparseMatrix::getColumnsCount() const
{
    return columnsCount;
}
