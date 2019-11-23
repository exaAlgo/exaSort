#include <exasort-impl.h>
#include <exasort-hypercube.h>

int initProbes(exaHyperCubeSortData data,exaComm comm)
{
  /* get input data */
  exaSortData input=(exaSortData)data;

  /* Allocate space for probes and counts */
  int nProbes=data->nProbes=3;
  exaMallocArray(nProbes,sizeof(exaScalar),(void**)&data->probes     );
  exaMallocArray(nProbes,sizeof(exaULong) ,(void**)&data->probeCounts);

  exaScalar extrema[2];
  getArrayExtrema((void*)extrema,(exaSortData)data,0,comm);
  exaScalar range=extrema[1]-extrema[0];
  exaScalar delta=range/(nProbes-1);

  data->probes[0]=extrema[0];
  data->probes[1]=extrema[0]+delta;
  data->probes[2]=extrema[1];
  return 0;
}

int updateProbeCounts(exaHyperCubeSortData data,exaComm comm)
{
  exaSortData input=(exaSortData)data;
  exaArray array=input->array;
  exaUInt offset=input->offset[0];
  exaDataType t =input->t[0];

  exaInt lelt   =exaArrayGetSize(array);
  exaInt size   =exaCommSize(comm);
  exaInt nProbes=data->nProbes;

  exaUInt i;
  for(i=0;i<nProbes;i++){
    data->probeCounts[i]=0;
  }

  exaUInt e;
  for(e=0;e<lelt;e++){
    exaScalar val_e=getValueAsScalar(array,e,offset,t);
    for(i=0;i<nProbes;i++)
      if(val_e<data->probes[i]) data->probeCounts[i]++;
  }

  exaCommReduce(comm,data->probeCounts,data->probeCounts,nProbes,exaULong_t,exaAddOp);
}

int exaHyperCubeSort(exaHyperCubeSortData data,exaComm comm)
{
  exaInt size=exaCommSize(comm);
  exaInt rank=exaCommRank(comm);

  exaSortData input=(exaSortData)data;

  exaArray array=input->array;
  exaDataType t =input->t[0];
  exaUInt offset=input->offset[0];

  exaLong out[2][1];
  arrayScan(out,array,comm);
  exaLong start=out[0][0];
  exaLong nElements=out[1][0];

  exaInt threshold=(nElements/(10*size));
  if(threshold<2) threshold=2;

  exaSortLocal((exaSortData)data);

  if(size==1) return 0;

  initProbes(data,comm);

  return 0;
}
