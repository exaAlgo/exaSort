#ifndef _EXASORT_IMPL_H_
#define _EXASORT_IMPL_H_

#include <exa-impl.h>
#include <exa-memory.h>

#include <exasort.h>

#define min(a,b) ((a)<(b) ? (a) : (b))
#define max(a,b) ((a)>(b) ? (a) : (b))
//
// exaSort: general functions
//
typedef struct{
  int nFields;
  exaArray array;
  exaDataType t[3];
  uint offset[3];
  int loadBalance;
  exaSortAlgo algo;
} exaSortData_private;
typedef exaSortData_private* exaSortData;

exaScalar getValueAsScalar(exaArray arr,uint i,
  uint offset,exaDataType type);
void getArrayExtrema(void *extrema_,exaSortData data,
  unsigned field,exaComm comm);

int set_dest(uint *proc,uint np,ulong start,uint size,ulong nelem);

int exaSortLocal(exaSortData data);
int exaSortPermuteBuf(exaArray arr,exaBuffer buf);
int exaSortField(exaArray arr,exaDataType t,uint fieldOffset,
  exaBuffer buf,int keep);
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
  ulong *probeCounts;
  uint threshold;
} exaHyperCubeSortData_private;
typedef exaHyperCubeSortData_private* exaHyperCubeSortData;

int initProbes(exaHyperCubeSortData data,exaComm comm);
int updateProbeCounts(exaHyperCubeSortData data,exaComm comm);
int exaHyperCubeSort(exaHyperCubeSortData data,exaComm comm);

#endif // exasort-impl
