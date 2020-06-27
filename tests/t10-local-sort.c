#include <exasort.h>
#include <time.h>

typedef struct{
  exaScalar data1;
  slong data2;
  exaUInt proc;
} Data;

#define N 5
#define M 3

int main(int argc,char *argv[]){
  MPI_Init(&argc,&argv);

  if(argc!=2){
    printf("Usage: %s <backend>\n",argv[0]);
    exit(0);
  }

  exaHandle h;
  exaInit(&h,MPI_COMM_WORLD,argv[1]);
  exaInt rank=exaRank(h);
  exaInt Size=exaSize(h);

  exaArray arr;
  exaArrayInit(&arr,Data,1);

  srand(time(0));

  int i,j; Data d;
  for(i=N;i>0;i--){
    int t=rand();
    for(j=M;j>0;j--){
      d.data1=j;
      d.data2=t;
      exaArrayAppend(arr,&d);
    }
  }

  exaSortArray2(arr,exaScalar_t,offsetof(Data,data1),
    exaLong_t,offsetof(Data,data2));

  Data *ptr=exaArrayGetPointer(arr);
  for(j=0;j<M;j++){
    int start=j*N;
    if(start>0) assert(ptr[start-1].data1<=ptr[start].data1);
    for(i=0;i<N-1;i++){
      if(ptr[start+i].data1!=ptr[start+i+1].data1)
        fprintf(stderr,"Error: element %d and %d are not the same.",start+i,start+i+1);
      if(ptr[start+i].data2>ptr[start+i+1].data2)
        fprintf(stderr,"Error: element %d is larger than  %d.",start+i,start+i+1);
    }
  }

  exaDestroy(arr);
  exaFinalize(h);

  MPI_Finalize();
  return 0;
}
