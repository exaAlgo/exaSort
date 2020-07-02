#include <exasort-impl.h>
#include <exa-memory.h>

int init_probes(hypercube_sort_data data,struct comm *c)
{
  /* get input data */
  sort_data input=data->data;

  /* Allocate space for probes and counts */
  int nprobes=data->nProbes=3;
  if(!data->probes     ) exaMalloc(nprobes,&data->probes     );
  if(!data->probeCounts) exaMalloc(nprobes,&data->probeCounts);

  exaScalar extrema[2];
  get_extrema((void*)extrema,data->data,0,c);
  exaScalar range=extrema[1]-extrema[0];
  exaScalar delta=range/(nprobes-1);

  data->probes[0]=extrema[0];
  data->probes[1]=extrema[0]+delta;
  data->probes[2]=extrema[1];

  return 0;
}

int update_probe_counts(hypercube_sort_data data,struct comm *c)
{
  sort_data input=data->data;
  uint offset  =input->offset[0];
  exaDataType t=input->t[0];

  uint nprobes =data->nProbes;

  struct array *a=&input->array->arr;
  uint size      =a->n;
  uint i;
  for(i=0;i<nprobes;i++) data->probeCounts[i]=0;

  uint e;
  for(e=0;e<size;e++){
    double val_e=get_scalar(a,e,offset,input->unit_size,t);
    for(i=0;i<nprobes;i++)
      if(val_e<data->probes[i]) data->probeCounts[i]++;
  }

  ulong buf[3];
  comm_allreduce(c,gs_long,gs_add,data->probeCounts,nprobes,buf);

  return 0;
}

int reachedThreshold(slong nelem,hypercube_sort_data data,
  exaComm c)
{
  int converged=1;

  ulong expected=nelem/2;
  if(abs(data->probeCounts[1]-expected)>data->threshold)
    converged=0;

  return converged;
}

int update_probes(slong nelem,hypercube_sort_data data)
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

int transferElements(hypercube_sort_data data,exaComm comm)
{
  sort_data input=data->data;
  exaArray array=input->array;
  uint offset=input->offset[0];
  exaDataType t =input->t[0];

  sint size=exaArrayGetSize(array);

  uint e,lowerSize=0,upperSize=0;
  for(e=0;e<size;e++){
    exaScalar val_e=get_scalar(&array->arr,e,offset,input->unit_size,t);
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

int exaHyperCubeSort(hypercube_sort_data data,exaComm comm)
{
  sort_data input=data->data;
  exaArray array=input->array;
  exaDataType t =input->t[0];
  uint offset=input->offset[0];

  struct comm *c =&comm->gsComm;
  struct array *a=&array->arr;

  sint size=c->np,rank=c->id;

  slong out[2][1],buf[2][1],in=a->n;
  comm_scan(out,c,gs_long,gs_add,&in,1,buf);
  slong start=out[0][0];
  slong nelem=out[1][0];

  uint threshold=(nelem/(10*size));
  if(threshold<2) threshold=2;
  data->threshold=threshold;

  sort_local(data->data);
  if(size==1) return 0;

  init_probes        (data,c);
  update_probe_counts(data,c);

  int maxIter=log2((data->probes[2]-data->probes[0])/EXA_TOL),iter=0;
  while(!reachedThreshold(nelem,data,comm) && iter++<maxIter){
    update_probes(nelem,data);
    update_probe_counts(data,c);
  }
  transferElements(data,comm);

  // TODO exaFree data->probes

  // split the communicator
  exaComm newComm;
  sint lower=(rank<size/2)?1:0;
  exaCommSplit(comm,lower,rank,&newComm);

  // FIXME
  //int balance=input->balance;
  //if(balance) exaLoadBalance(input->array,newComm);

  exaHyperCubeSort(data,newComm);

  exaDestroy(newComm);

  return 0;
}
