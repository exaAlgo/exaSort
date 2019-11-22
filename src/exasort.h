#ifndef _EXASORT_H_
#define _EXASORT_H_

#include <exa.h>

int exaSortArray (exaArray arr,exaDataType t ,exaUInt offset);
int exaSortArray2(exaArray arr,exaDataType t1,exaUInt offset1,exaDataType t2,exaUInt offset2);

int exaBinSort(exaArray array,exaDataType t,exaUInt offset,int loadBalance,exaComm comm);

#endif // exasort
