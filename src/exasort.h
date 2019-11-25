#ifndef _EXASORT_H_
#define _EXASORT_H_

#include <exa.h>

typedef enum{
  exaSortAlgoBinSort      =0,
  exaSortAlgoHyperCubeSort=1
} exaSortAlgo;

int exaSortArray (exaArray arr,exaDataType t ,exaUInt offset);
int exaSortArray2(exaArray arr,exaDataType t1,exaUInt offset1,exaDataType t2,exaUInt offset2);

void exaArrayScan(exaLong out[2][1],exaArray array,exaComm comm);
int exaSort(exaArray array,exaDataType t,exaUInt offset,exaSortAlgo algo,int loadBalance,
  exaComm comm);
int exaLoadBalance(exaArray array,exaComm comm);

#endif // exasort
