#include <exasort-impl.h>
#include <exasort-hypercube.h>

int initProbes(exaHyperCubeSortData data,exaComm comm)
{
  exaInt np=exaCommSize(comm);
  /* get input data */
  exaSortData input=(exaSortData)data;

  /* Allocate space for probes and counts */
  int nProbes=data->nProbes=3;
  exaMallocArray(nProbes,exaDataTypeGetSize(input->t[0]),&data->probes     );
  exaMallocArray(nProbes,sizeof(exaULong),(void**)&data->probeCounts);

  exaScalar extrema[2];
  getArrayExtrema((void*)extrema,(exaSortData)data,0,comm);
  exaScalar range=extrema[1]-extrema[0];
  exaScalar delta=range/np;

  return 0;
}
