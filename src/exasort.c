#include <exasort-impl.h>

int setDestination(uint *proc,int np,ulong start,
  uint size,ulong nElements)
{
  uint partitionSize=nElements/np;

  uint i;
  if(partitionSize==0){
    for(i=0;i<size;i++)
      proc[i]=start+i;
    return 0;
  }

  uint id1,id2;

  uint nrem=nElements-np*partitionSize;
  uint lower=nrem*(partitionSize+1);
  if(start<=lower) id1=start/(partitionSize+1);
  else id1=nrem+(start-lower)/partitionSize;

  if((start+size)<=lower) id2=(start+size)/(partitionSize+1);
  else id2=nrem+(start+size-lower)/partitionSize;

  i=0;
  while(id1<=id2 && i<size){
    ulong s=id1*partitionSize+min(id1,nrem);
    ulong e=(id1+1)*partitionSize+min(id1+1,nrem);
    e=min(e,nElements);
    while(s<=start+i && start+i<e && i<size){
      proc[i++]=id1;
    }
    id1++;
  }

  return 0;
}

int exaLoadBalance(exaArray array,exaComm comm)
{
  slong out[2][1];
  exaArrayScan(out,array,comm);
  ulong start=out[0][0];
  ulong nElements=out[1][0];

  sint np=exaCommSize(comm);
  uint size=exaArrayGetSize(array);

  uint *proc; exaCalloc(size,&proc);

  setDestination(proc,np,start,size,nElements);

  exaArrayTransferExt(array,proc,comm);

  exaFree(proc);

  return 0;
}

int exaSortPrivate(exaSortData data,exaComm comm){
  exaHyperCubeSortData hdata;

  int loadBalance=data->loadBalance;
  exaSortAlgo algo=data->algo;

  switch(algo){
    case exaSortAlgoBinSort:
      exaBinSort(data,comm);
      break;
    case exaSortAlgoHyperCubeSort:
      exaMallocArray(1,sizeof(*hdata),(void**)&hdata);
      hdata->data=data;
      exaHyperCubeSort(hdata,comm);
      exaFree(hdata);
      break;
    default:
      break;
  }

  if(loadBalance){
    exaLoadBalance(data->array,comm);
    exaSortLocal(data);
  }

  return 0;
}

int exaSort(exaArray array,exaDataType t,uint offset,
  exaSortAlgo algo,int loadBalance,exaComm comm)
{
  exaSortData data; exaMallocArray(1,sizeof(*data),(void**)&data);
  data->array=array;
  data->t[0]=t,data->offset[0]=offset;
  data->nFields=1;
  data->algo=algo;
  data->loadBalance=loadBalance;

  exaSortPrivate(data,comm);

  exaFree(data);

  return 0;
}

int exaSort2(exaArray array,exaDataType t1,uint offset1,
  exaDataType t2,uint offset2,exaSortAlgo algo,
  int loadBalance,exaComm comm)
{
  exaSortData data; exaMallocArray(1,sizeof(*data),(void**)&data);
  data->array=array;
  data->t[0]=t1,data->offset[0]=offset1;
  data->t[1]=t2,data->offset[1]=offset2;
  data->nFields=2;
  data->algo=algo;
  data->loadBalance=loadBalance;

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

void getArrayExtrema(void *extrema_,exaSortData data,
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
