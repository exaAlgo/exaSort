#ifndef _EXASORT_H_
#define _EXASORT_H_

#include <exa.h>

typedef enum{
  exaSortAlgoBinSort      =0,
  exaSortAlgoHyperCubeSort=1
} exaSortAlgo;

int exaSortArray (exaArray arr,exaDataType t ,uint offset);
int exaSortArray2(exaArray arr,exaDataType t1,uint offset1,
  exaDataType t2,uint offset2);
int exaSortArray3(exaArray arr,exaDataType t1,uint offset1,
  exaDataType t2,uint offset2,exaDataType t3,uint offset3);

void exaArrayScan(slong out[2][1],exaArray array,exaComm comm);

int exaSort(exaArray array,exaDataType t,uint offset,
  exaSortAlgo algo,int balance,exaComm comm);
int exaSort2(exaArray array,exaDataType t1,uint offset1,
  exaDataType t2,uint offset2,exaSortAlgo algo,
  int balance,exaComm comm);

#endif // exasort
