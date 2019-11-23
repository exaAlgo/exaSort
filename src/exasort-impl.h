#ifndef _EXASORT_IMPL_H_
#define _EXASORT_IMPL_H_

#include <exa-impl.h>
#include <exa-memory.h>

#include <exasort.h>

#define min(a,b) ((a)<(b) ? (a) : (b))

typedef struct{
  int nFields;
  exaArray array;
  exaDataType t[3];
  exaUInt offset[3];
  int loadBalance;
  exaSortAlgo algo;
} exaSortData_private;
typedef exaSortData_private* exaSortData;

typedef struct{
  exaSortData data;
  int nProbes;
  exaScalar *probes;
  exaULong *probeCounts;
} exaHyperCubeSortData_private;
typedef exaHyperCubeSortData_private* exaHyperCubeSortData;

/* Helper functions */
exaScalar getValueAsScalar(exaArray arr,exaUInt i,exaUInt offset,exaDataType type);
void getArrayExtrema(void *extrema,exaSortData data,unsigned field,exaComm comm);
void arrayScan(exaLong out[2][1],exaArray array,exaComm comm);

/* Local sort */
int exaSortLocal(exaSortData data);
int exaSortPermuteBuf(exaArray arr,exaBuffer buf);
int exaSortField(exaArray arr,exaDataType t,exaUInt fieldOffset,exaBuffer buf,int keep);

#endif // exasort-impl
