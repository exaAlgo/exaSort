#include <exasort.h>

typedef struct{
  exaInt data;
  exaUInt proc;
} Data;

#define N 5

int main(int argc,char *argv[]){
  MPI_Init(&argc,&argv);

  exaSettings s; exaSettingsInit(&s);
  exaSettingsSetSetting("backend","/opencl/cpu",s);
  exaSettingsSetSetting("debug","on",s);

  exaHandle h;
  exaInit(&h,MPI_COMM_WORLD,s);
  exaInt rank=exaRank(h);
  exaInt Size=exaSize(h);

  exaArray arr;
  exaArrayInit(&arr,Data,1);

  exaDestroy(s);
  exaDestroy(arr);
  exaFinalize(h);

  MPI_Finalize();
  return 0;
}
