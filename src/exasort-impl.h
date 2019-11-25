#ifndef _EXASORT_IMPL_H_
#define _EXASORT_IMPL_H_

#include <exa-impl.h>
#include <exa-memory.h>

#include <exasort.h>

#define min(a,b) ((a)<(b) ? (a) : (b))
//
// exaSort: general functions
//
typedef struct{
  int nFields;
  exaArray array;
  exaDataType t[3];
  exaUInt offset[3];
  int loadBalance;
  exaSortAlgo algo;
} exaSortData_private;
typedef exaSortData_private* exaSortData;

int       setDestination(exaInt *proc,exaInt np,exaULong start,exaUInt size,exaULong nElements);
exaScalar getValueAsScalar(exaArray arr,exaUInt i,exaUInt offset,exaDataType type);
void      getArrayExtrema(void *extrema_,exaSortData data,unsigned field,exaComm comm);

int exaSortLocal(exaSortData data);
int exaSortPermuteBuf(exaArray arr,exaBuffer buf);
int exaSortField(exaArray arr,exaDataType t,exaUInt fieldOffset,exaBuffer buf,int keep);
//
// exaBinSort
//
int exaBinSort(exaSortData data,exaComm comm);
//
// exaHyperCubeSort
//
typedef struct{
  exaSortData data;
  int nProbes;
  exaScalar *probes;
  exaULong *probeCounts;
  exaUInt threshold;
} exaHyperCubeSortData_private;
typedef exaHyperCubeSortData_private* exaHyperCubeSortData;

int exaHyperCubeSort(exaHyperCubeSortData data,exaComm comm);
#endif // exasort-impl
