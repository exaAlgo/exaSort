#include <exasort-impl.h>

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
  exaHyperCubeSortData hdata;

  int balance =data->balance;
  exaSortAlgo algo=data->algo;
  exaComm dup; exaCommDup(&dup,comm);

  struct array *a =&data->array->arr;
  size_t usize    =exaArrayGetUnitSize(data->array);
  struct crystal cr; crystal_init(&cr,&comm->gsComm);

  switch(algo){
    case exaSortAlgoBinSort:
      exaBinSort(data,comm);
      break;
    case exaSortAlgoHyperCubeSort:
      exaMallocArray(1,sizeof(*hdata),(void**)&hdata);
      hdata->data=data;
      exaHyperCubeSort(hdata,dup);
      exaFree(hdata);
      break;
    default:
      break;
  }

  if(balance){
    load_balance(a,usize,&comm->gsComm,&cr);
    exaSortLocal(data);
  }

  crystal_free(&cr);
  exaDestroy(dup);

  return 0;
}

int exaSort(exaArray array,exaDataType t,uint offset,
  exaSortAlgo algo,int balance,exaComm comm)
{
  sort_data_private sd;
  sd.nfields=1;
  sd.array=array;
  sd.unit_size=exaArrayGetUnitSize(array);

  sd.t[0]=t,sd.offset[0]=offset;

  sd.balance=balance;
  sd.algo=algo;

  exaSortPrivate(&sd,comm);

  return 0;
}

int exaSort2(exaArray array,exaDataType t1,uint offset1,
  exaDataType t2,uint offset2,exaSortAlgo algo,
  int balance,exaComm comm)
{
  sort_data data; exaMallocArray(1,sizeof(*data),(void**)&data);
  data->array=array;
  data->t[0]=t1,data->offset[0]=offset1;
  data->t[1]=t2,data->offset[1]=offset2;
  data->nfields=2;
  data->algo=algo;
  data->balance=balance;

  exaSortPrivate(data,comm);

  exaFree(data);

  return 0;
}

void exaArrayScan(slong out[2][1],exaArray array,exaComm comm)
{
  slong buf[2][1],in[1];
  in[0]=exaArrayGetSize(array);
  exaCommScan(comm,out,in,buf,1,exaLong_t,exaAddOp);
}

exaScalar getValueAsScalar(exaArray arr,uint i,
  uint offset,exaDataType type)
{
  char* v=(char*)exaArrayGetPointer(arr)+
    i*exaArrayGetUnitSize(arr)+offset;

  sint dataI;
  uint dataUi;
  slong dataL;
  ulong dataUl;
  exaScalar data;

  switch(type){
    case exaInt_t:
      dataI=*((exaInt*)v);
      data=dataI;
      break;
    case exaUInt_t:
      dataUi=*((exaUInt*)v);
      data=dataUi;
      break;
    case exaLong_t:
      dataL=*((exaLong*)v);
      data=dataL;
      break;
    case exaULong_t:
      dataUl=*((exaULong*)v);
      data=dataUl;
      break;
    case exaScalar_t:
      data=*((exaScalar*)v);
      break;
    default:
      break;
  }

  return data;
}

void getArrayExtrema(void *extrema_,sort_data data,
  unsigned field,exaComm comm)
{
  exaArray arr  =data->array;
  uint offset=data->offset[field];
  exaDataType t =data->t[field];

  exaScalar *extrema=(exaScalar *)extrema_;

  sint size=exaArrayGetSize(arr);
  if(size==0){
    extrema[0]=-DBL_MAX;
    extrema[1]=-DBL_MAX;
  } else {
    extrema[0]=getValueAsScalar(arr,0     ,offset,t)*-1;
    extrema[1]=getValueAsScalar(arr,size-1,offset,t);
  }

  exaCommGop(comm,extrema,2,exaScalar_t,exaMaxOp);
  extrema[0]*=-1;
}
