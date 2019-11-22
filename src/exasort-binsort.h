#ifndef _EXASORT_BIN_H_
#define _EXASORT_BIN_H_

#include "exasort-impl.h"

int exaArraySetBin(exaUInt **proc_,exaArray arr,exaDataType t,exaUInt offset,exaComm comm);
int exaBinSort(exaArray array,exaDataType t,exaUInt offset,int loadBalance,exaComm comm);

#endif
