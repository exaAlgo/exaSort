#include <stdio.h>
#include <mpi.h>

#include <exasort.h>

typedef struct {
  exaInt proc;
  exaInt key;
  exaScalar value;
} kvPair;

int main() {
  MPI_Init(0,0);

  exaHandle h;
  exaInit(&h,MPI_COMM_WORLD);

  exaArray arr;
  size_t size=exaRank(h)+5;
  exaArrayInit(&arr,sizeof(kvPair),size);

  kvPair data;
  int i,j;
  for(i=0; i<size; i++){
    data.key=i;
    data.value=-(exaRank(h)+ i*(1./exaArrayGetMaxSize(arr)));
    exaArrayAppend(arr,&data);
  }

  exaBinSort(h,kvPair,arr,exaScalar,value,proc);

  kvPair *arrayData=exaArrayGetPointer(arr);
  if(exaRank(h)==0) printf("After sorting\n");
  for(j=0; j<exaSize(h); j++) {
    exaBarrier(h);
    if(j==exaRank(h)) {
      for(i=0; i<exaArrayGetSize(arr); i++){
        printf("rank: %d %d key=" exaIntFormat ", value=" exaScalarFormat "\n",exaRank(h),i,\
            arrayData[i].key,arrayData[i].value);
      }
      fflush(stdout);
    }
  }

  exaArrayFree(arr);
  exaFinalize(h);

  MPI_Finalize();

  return 0;
}
