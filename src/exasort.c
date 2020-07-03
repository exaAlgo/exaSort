#include <exasort-impl.h>
#include <exa-memory.h>

int set_dest(uint *proc,uint np,ulong start,uint size,ulong nelem)
{
  uint psize=nelem/np,i;
  if(psize==0){
    for(i=0;i<size;i++) proc[i]=start+i;
    return 0;
  }

  uint nrem =nelem-np*psize;
  uint lower=nrem*(psize+1);

  uint id1,id2;
  if(start<lower) id1=start/(psize+1);
  else id1=nrem+(start-lower)/psize;

  if((start+size)<lower) id2=(start+size)/(psize+1);
  else id2=nrem+(start+size-lower)/psize;

  i=0;
  while(id1<=id2 && i<size){
    ulong s=id1*psize+min(id1,nrem);
    ulong e=(id1+1)*psize+min(id1+1,nrem);
    e=min(e,nelem);
    while(s<=start+i && start+i<e && i<size) proc[i++]=id1;
    id1++;
  }

  return 0;
}

int load_balance(struct array *a,size_t size,struct comm *c,
    struct crystal *cr)
{
  slong in=a->n,out[2][1],buf[2][1];
  comm_scan(out,c,gs_long,gs_add,&in,1,buf);
  ulong start=out[0][0];
  ulong nelem=out[1][0];

  uint *proc; exaCalloc(a->n,&proc);

  set_dest(proc,c->np,start,a->n,nelem);
  sarray_transfer_ext_(a,size,proc,sizeof(uint),cr);

  exaFree(proc);

  return 0;
}

int exaSortPrivate(sort_data data,exaComm comm){
  struct comm *c=&comm->gsComm,dup; comm_dup(&dup,c);

  hypercube_sort_data hdata;

  int balance =data->balance;
  exaSortAlgo algo=data->algo;

  struct array *a =&data->array->arr;
  size_t usize    =exaArrayGetUnitSize(data->array);

  switch(algo){
    case exaSortAlgoBinSort:
      exaBinSort(data,c);
      break;
    case exaSortAlgoHyperCubeSort:
      exaMalloc(1,&hdata);
      hdata->data=data;
      exaHyperCubeSort(hdata,&dup);
      exaFree(hdata);
      break;
    default:
      break;
  }

  if(balance){
    struct crystal cr; crystal_init(&cr,&comm->gsComm);
    load_balance(a,usize,&comm->gsComm,&cr);
    sort_local(data);
    crystal_free(&cr);
  }

  comm_free(&dup);

  return 0;
}

int exaSort(exaArray array,exaDataType t,uint offset,
  exaSortAlgo algo,int balance,exaComm comm)
{
  sort_data_private sd;

  sd.nfields=1;
  sd.array=array;
  sd.unit_size=exaArrayGetUnitSize(array);
  sd.align=exaArrayGetAlign(array);

  sd.t[0]=t,sd.offset[0]=offset;

  sd.balance=balance;
  sd.algo=algo;

  buffer_init(&sd.buf,1024);
  exaSortPrivate(&sd,comm);
  buffer_free(&sd.buf);

  return 0;
}

int exaSort2(exaArray array,exaDataType t1,uint offset1,
  exaDataType t2,uint offset2,exaSortAlgo algo,
  int balance,exaComm comm)
{
  sort_data_private sd;

  sd.nfields=2;
  sd.array=array;
  sd.unit_size=exaArrayGetUnitSize(array);
  sd.align=exaArrayGetAlign(array);

  sd.t[0]=t1,sd.offset[0]=offset1;
  sd.t[1]=t2,sd.offset[1]=offset2;

  sd.algo=algo;
  sd.balance=balance;

  buffer_init(&sd.buf,1024);
  exaSortPrivate(&sd,comm);
  buffer_free(&sd.buf);

  return 0;
}

exaScalar get_scalar(struct array *a,uint i,uint offset,uint usize,
    exaDataType type)
{
  char* v=(char*)a->ptr+i*usize+offset;
  double data;

  switch(type){
    case exaInt_t:
      data=*((sint*)v);
      break;
    case exaUInt_t:
      data=*((uint*)v);
      break;
    case exaLong_t:
      data=*((slong*)v);
      break;
    case exaULong_t:
      data=*((ulong*)v);
      break;
    case exaScalar_t:
      data=*((double*)v);
      break;
    default:
      break;
  }

  return data;
}

void get_extrema(void *extrema_,sort_data data,uint field,
    struct comm* c)
{
  struct array *a=&data->array->arr;
  uint usize     =data->unit_size;
  uint offset    =data->offset[field];
  exaDataType t  =data->t[field];

  exaScalar *extrema=(exaScalar *)extrema_;

  sint size=a->n;
  if(size==0){
    extrema[0]=-DBL_MAX;
    extrema[1]=-DBL_MAX;
  } else {
    extrema[0]=get_scalar(a,0     ,offset,usize,t)*-1;
    extrema[1]=get_scalar(a,size-1,offset,usize,t);
  }

  double buf[2];
  comm_allreduce(c,gs_double,gs_max,extrema,2,buf);
  extrema[0]*=-1;
}
