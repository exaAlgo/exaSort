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
  exaArrayCreate(kvPair,arr,5);
  kvPair *data=exaArrayPointer(kvPair,arr);

  int i,j;
  printf("Before sorting -- Array size: %d\n",exaArraySize(arr));
  for(j=0; j<exaSize(h); j++) {
    exaBarrier(h);
    if(j==exaRank(h)) {
      for(i=0; i<exaArraySize(arr); i++){
        data[i].key=i;
        data[i].value=5*exaSize(h)-(5*exaRank(h)+i);
        printf("rank: %d %d key=" exaIntFormat ", value=" exaScalarFormat "\n",exaRank(h),i,\
            data[i].key,data[i].value);
      }
    }
    fflush(stdout);
  }

  exaBinSort(h,kvPair,arr,exaScalar,value,proc);

  data=exaArrayPointer(kvPair,arr);
  printf("After sorting -- Array size: %d\n",exaArraySize(arr));
  for(j=0; j<exaSize(h); j++) {
    exaBarrier(h);
    if(j==exaRank(h)) {
      for(i=0; i<exaArraySize(arr); i++){
        printf("rank: %d %d key=" exaIntFormat ", value=" exaScalarFormat "\n",exaRank(h),i,\
            data[i].key,data[i].value);
      }
    }
    fflush(stdout);
  }

  //exaArrayFree(arr);
  exaFinalize(h);

  MPI_Finalize();

  return 0;
}
