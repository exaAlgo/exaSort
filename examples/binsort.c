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
  exaArrayInit(&arr,sizeof(kvPair),exaRank(h)+5);
  kvPair *data=exaArrayPointer(arr);

  int i,j;
  for(i=0; i<exaArraySize(arr); i++){
    data[i].key=i;
    data[i].value=-(exaRank(h)+ i*(1./exaArraySize(arr)));
  }

  exaBinSort(h,kvPair,arr,exaScalar,value,proc);

  data=exaArrayPointer(arr);
  if(exaRank(h)==0) printf("After sorting\n");
  for(j=0; j<exaSize(h); j++) {
    exaBarrier(h);
    if(j==exaRank(h)) {
      for(i=0; i<exaArraySize(arr); i++){
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
