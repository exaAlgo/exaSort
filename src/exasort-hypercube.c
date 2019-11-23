#include <exasort-impl.h>
#include <exasort-hypercube.h>

int initProbes(exaHyperCubeSortData data,exaComm comm)
{
  exaInt np=exaCommSize(comm);
  /* get input data */
  exaSortData input=(exaSortData)data;

  /* Allocate space for probes and counts */
  int nProbes=data->nProbes=3;
  exaMallocArray(nProbes,exaDataTypeGetSize(input->t),&data->probes     );
  exaMallocArray(nProbes,exaDataTypeGetSize(input->t),(void**)&data->probeCounts);

  exaScalar *extrema;
  getArrayExtrema((void**)&extrema,(exaSortData)data,comm);
  exaScalar range=extrema[1]-extrema[0];
  exaScalar delta=range/np;

  return 0;
}
