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
  exaArraySetSize(arr,size);

  kvPair *data=exaArrayGetPointer(arr);
  int i,j;
  for(i=0; i<size; i++){
    data[i].key=i;
    data[i].value=-(exaRank(h)+ i*(1./exaArrayGetSize(arr)));
  }

  exaBinSort(h,kvPair,arr,exaScalar,value,proc);

  data=exaArrayGetPointer(arr);
  if(exaRank(h)==0) printf("After sorting\n");
  for(j=0; j<exaSize(h); j++) {
    exaBarrier(h);
    if(j==exaRank(h)) {
      for(i=0; i<exaArrayGetSize(arr); i++){
        printf("rank: %d %d key=" exaIntFormat ", value=" exaScalarFormat "\n",exaRank(h),i,\
            data[i].key,data[i].value);
      }
      fflush(stdout);
    }
  }

  exaArrayFree(arr);
  exaFinalize(h);

  MPI_Finalize();

  return 0;
}
