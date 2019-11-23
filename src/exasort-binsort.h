#ifndef _EXASORT_BIN_H_
#define _EXASORT_BIN_H_

#include "exasort-impl.h"

int exaArraySetBin(exaUInt **proc_,exaSortData data,exaComm comm);
int exaBinSort(exaArray array,exaDataType t,exaUInt offset,int loadBalance,exaComm comm);

#endif
