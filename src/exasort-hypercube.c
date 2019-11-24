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

  exaInt size   =exaArrayGetSize(array);
  exaInt nProbes=data->nProbes;

  exaUInt i;
  for(i=0;i<nProbes;i++){
    data->probeCounts[i]=0;
  }

  exaUInt e;
  for(e=0;e<size;e++){
    exaScalar val_e=getValueAsScalar(array,e,offset,t);
    for(i=0;i<nProbes;i++)
      if(val_e<data->probes[i]) data->probeCounts[i]++;
  }

  exaCommGop(comm,data->probeCounts,nProbes,exaULong_t,exaAddOp);

  return 0;
}

int reachedThreshold(exaLong nElements,exaHyperCubeSortData data,exaComm c)
{
  int converged=1;

  exaULong expected=nElements/2;
  if(abs(data->probeCounts[1]-expected)>data->threshold) converged=0;

  return converged;
}

int updateProbes(exaLong nElements,exaHyperCubeSortData data,exaComm comm)
{
  exaULong *probeCounts=data->probeCounts;
  exaScalar *probes=data->probes;

  exaLong expected=nElements/2;
  if(abs(data->probeCounts[1]-expected)<data->threshold) return 0;

  if(probeCounts[1]<expected) probes[0]=probes[1];
  else probes[2]=probes[1];

  probes[1]=probes[0]+(probes[2]-probes[0])/2;

  return 0;
}

int setDestination(exaInt *proc,exaInt np,exaULong start,exaUInt size,exaULong nElements)
{
  exaUInt partitionSize=nElements/np;
  exaUInt nrem=nElements-np*partitionSize;

  exaUInt i;
  if(partitionSize==0){
    for(i=0;i<size;i++)
      proc[i]=i;
    return 0;
  }

  for(i=0;i<size;i++){
    exaUInt id=start+i+1;
    exaUInt offset=id%partitionSize;
    exaUInt rank=(id-offset)/partitionSize;
    proc[i]=rank;
  }

  return 0;
}

int transferElements(exaHyperCubeSortData data,exaComm comm)
{
  exaSortData input=(exaSortData)data;
  exaArray array=input->array;
  exaUInt offset=input->offset[0];
  exaDataType t =input->t[0];

  exaInt size=exaArrayGetSize(array);

  exaUInt e,lowerSize=0,upperSize=0;
  for(e=0;e<size;e++){
    exaScalar val_e=getValueAsScalar(array,e,offset,t);
    if(val_e<data->probes[1]) lowerSize++;
    else upperSize++;
  }

  exaLong out[2][2],in[2],buf[2][2];
  in[0]=lowerSize,in[1]=upperSize;
  exaCommScan(comm,out,in,buf,2,exaLong_t,exaAddOp);

  exaLong lowerStart=out[0][0];
  exaLong upperStart=out[0][1];
  exaLong lowerElements=out[1][0];
  exaLong upperElements=out[1][1];

  exaInt np=exaCommSize(comm);
  exaInt lowerNp=np/2;
  exaInt upperNp=np-lowerNp;

  exaUInt *proc;
  exaCalloc(size,&proc);

  setDestination(proc            ,lowerNp,lowerStart,lowerSize,lowerElements);
  setDestination(&proc[lowerSize],upperNp,upperStart,upperSize,upperElements);

  exaArrayTransferExt(array,proc,comm);

  exaFree(proc);

  return 0;
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

  exaUInt threshold=(nElements/(10*size));
  if(threshold<2) threshold=2;
  data->threshold=threshold;

  exaSortLocal((exaSortData)data);

  if(size==1) return 0;

  initProbes(data,comm);
  updateProbeCounts(data,comm);
  while(!reachedThreshold(nElements,data,comm)){
    updateProbes(nElements,data,comm);
    updateProbeCounts(data,comm);
  }
  transferElements(data,comm);
  // split the communicator
  exaHyperCubeSort(data,comm);

  return 0;
}
