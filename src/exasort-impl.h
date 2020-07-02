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
  int nfields;
  exaArray array;
  size_t unit_size,align;

  exaDataType t[3];
  uint offset[3];

  int balance;
  exaSortAlgo algo;

  buffer buf;
} sort_data_private;

typedef sort_data_private* sort_data;

exaScalar get_scalar(struct array *a,uint i,uint offset,uint usize,
  exaDataType type);

void get_extrema(void *extrema_,sort_data data,uint field,struct comm *c);

int set_dest(uint *proc,uint np,ulong start,uint size,ulong nelem);

int load_balance(struct array *a,size_t size,struct comm *c,
    struct crystal *cr);

int sort_local(sort_data data);
//
// exaBinSort
//
int exaBinSort(sort_data data,struct comm *c);
//
// exaHyperCubeSort
//
typedef struct{
  sort_data data;
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
