#include <exasort-impl.h>

int initProbes(exaHyperCubeSortData data,exaComm comm)
{
  /* get input data */
  exaSortData input=data->data;

  /* Allocate space for probes and counts */
  int nProbes=data->nProbes=3;
  exaMallocArray(nProbes,sizeof(exaScalar),
    (void**)&data->probes);
  exaMallocArray(nProbes,sizeof(exaULong),
    (void**)&data->probeCounts);

  exaScalar extrema[2];
  getArrayExtrema((void*)extrema,data->data,0,comm);
  exaScalar range=extrema[1]-extrema[0];
  exaScalar delta=range/(nProbes-1);

  data->probes[0]=extrema[0];
  data->probes[1]=extrema[0]+delta;
  data->probes[2]=extrema[1];

  return 0;
}

int updateProbeCounts(exaHyperCubeSortData data,exaComm comm)
{
  exaSortData input=data->data;
  exaArray array=input->array;
  uint offset=input->offset[0];
  exaDataType t =input->t[0];

  sint size   =exaArrayGetSize(array);
  sint nProbes=data->nProbes;

  uint i;
  for(i=0;i<nProbes;i++){
    data->probeCounts[i]=0;
  }

  uint e;
  for(e=0;e<size;e++){
    exaScalar val_e=getValueAsScalar(array,e,offset,t);
    for(i=0;i<nProbes;i++)
      if(val_e<data->probes[i] ||
         fabs(val_e-data->probes[i])<EXA_TOL)
        data->probeCounts[i]++;
  }

  exaCommGop(comm,data->probeCounts,nProbes,exaULong_t,exaAddOp);

  return 0;
}

int reachedThreshold(slong nelem,exaHyperCubeSortData data,
  exaComm c)
{
  int converged=1;

  ulong expected=nelem/2;
  if(abs(data->probeCounts[1]-expected)>data->threshold)
    converged=0;

  return converged;
}

int updateProbes(slong nelem,exaHyperCubeSortData data,
  exaComm comm)
{
  ulong *probeCounts=data->probeCounts;
  exaScalar *probes=data->probes;

  slong expected=nelem/2;
  if(abs(data->probeCounts[1]-expected)<data->threshold)
    return 0;

  if(probeCounts[1]<expected) probes[0]=probes[1];
  else probes[2]=probes[1];

  probes[1]=probes[0]+(probes[2]-probes[0])/2;

  return 0;
}

int transferElements(exaHyperCubeSortData data,exaComm comm)
{
  exaSortData input=data->data;
  exaArray array=input->array;
  uint offset=input->offset[0];
  exaDataType t =input->t[0];

  sint size=exaArrayGetSize(array);

  uint e,lowerSize=0,upperSize=0;
  for(e=0;e<size;e++){
    exaScalar val_e=getValueAsScalar(array,e,offset,t);
    if(val_e<data->probes[1] ||
       fabs(val_e-data->probes[1])<EXA_TOL)
      lowerSize++;
    else upperSize++;
  }

  ulong out[2][2],in[2],buf[2][2];
  in[0]=lowerSize,in[1]=upperSize;
  exaCommScan(comm,out,in,buf,2,exaULong_t,exaAddOp);

  ulong lowerStart=out[0][0];
  ulong upperStart=out[0][1];
  ulong lowerElements=out[1][0];
  ulong upperElements=out[1][1];

  uint np=exaCommSize(comm);
  uint lowerNp=np/2;
  uint upperNp=np-lowerNp;

  uint *proc;
  exaCalloc(size,&proc);

  set_dest(proc,lowerNp,lowerStart,lowerSize,lowerElements);
  set_dest(&proc[lowerSize],upperNp,upperStart,upperSize,upperElements);

  for(e=lowerSize;e<size;e++) proc[e]+=lowerNp;

  exaArrayTransferExt(array,proc,comm);

  exaFree(proc);

  return 0;
}

int exaHyperCubeSort(exaHyperCubeSortData data,exaComm comm)
{
  sint size=exaCommSize(comm);
  sint rank=exaCommRank(comm);

  exaSortData input=data->data;
  exaArray array=input->array;
  exaDataType t =input->t[0];
  uint offset=input->offset[0];

  slong out[2][1];
  exaArrayScan(out,array,comm);
  slong start=out[0][0];
  slong nelem=out[1][0];

  uint threshold=(nelem/(10*size));
  if(threshold<2) threshold=2;
  data->threshold=threshold;

  exaSortLocal(data->data);

  if(size==1){
    exaDestroy(comm);
    return 0;
  }

  initProbes(data,comm);
  updateProbeCounts(data,comm);
  while(!reachedThreshold(nelem,data,comm)){
    updateProbes(nelem,data,comm);
    updateProbeCounts(data,comm);
  }
  transferElements(data,comm);

  // TODO exaFree data->probes

  // split the communicator
  sint lower=(rank<size/2)?1:0;
  exaComm newComm;
  exaCommSplit(comm,lower,rank,&newComm);

  int loadBalance=input->loadBalance;
  if(loadBalance) exaLoadBalance(input->array,newComm);

  exaHyperCubeSort(data,newComm);

  exaCommDestroy(newComm);

  return 0;
}
