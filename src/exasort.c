#include <exasort-impl.h>

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

  exaUInt id1=start/partitionSize;
  exaUInt id2=(start+size+1-nrem)/partitionSize+1;

  i=0;
  for(;id1<=id2;id1++){
    exaULong s=id1*partitionSize+min(id1,nrem);
    exaULong e=(id1+1)*partitionSize+min(id1+1,nrem);
    while(s<=start+i && start+i<e && i<size) proc[i++]=id1;
  }

  return 0;
}

int exaLoadBalance(exaArray array,exaComm comm)
{
  exaLong out[2][1];
  exaArrayScan(out,array,comm);
  exaULong start=out[0][0];
  exaULong nElements=out[1][0];

  exaUInt size=exaArrayGetSize(array);
  exaInt *proc; exaCalloc(size,&proc);
  setDestination(proc,exaCommSize(comm),start,size,nElements);
  exaArrayTransferExt(array,proc,comm);
  exaFree(proc);

  return 0;
}

int exaSort(exaArray array,exaDataType t,exaUInt offset,exaSortAlgo algo,int loadBalance,
  exaComm comm)
{
  exaSortData data; exaMallocArray(1,sizeof(*data),(void**)&data);
  data->array=array,data->t[0]=t,data->offset[0]=offset;
  data->nFields=1;
  data->loadBalance=loadBalance;

  exaHyperCubeSortData hdata;
  exaComm comm_;

  switch(algo){
    case exaSortAlgoBinSort:
      exaBinSort(data,comm);
      break;
    case exaSortAlgoHyperCubeSort:
      exaMallocArray(1,sizeof(*hdata),(void**)&hdata);
      exaCommDup(&comm_,comm);
      hdata->data=data;
      exaHyperCubeSort(hdata,comm_);
      exaFree(hdata);
      break;
    default:
      break;
  }

  if(loadBalance){
    exaLoadBalance(data->array,comm);
    exaSortLocal(data);
  }

  exaFree(data);
}

void exaArrayScan(exaLong out[2][1],exaArray array,exaComm comm)
{
  exaLong buf[2][1],in[1];
  in[0]=exaArrayGetSize(array);
  exaCommScan(comm,out,in,buf,1,exaLong_t,exaAddOp);
}

exaScalar getValueAsScalar(exaArray arr,exaUInt i,exaUInt offset,exaDataType type)
{
  char* v=((char*)exaArrayGetPointer(arr)+i*exaArrayGetUnitSize(arr)+offset);
  exaInt dataI;
  exaUInt dataUi;
  exaLong dataL;
  exaULong dataUl;
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

void getArrayExtrema(void *extrema_,exaSortData data,unsigned field,exaComm comm)
{
  exaArray arr  =data->array;
  exaUInt offset=data->offset[field];
  exaDataType t =data->t[field];

  exaScalar *extrema=(exaScalar *)extrema_;

  exaInt size=exaArrayGetSize(arr);
  if(size==0){
    extrema[0]=-DBL_MAX;
    extrema[1]=-DBL_MAX;
  } else {
    extrema[0]=getValueAsScalar(arr,0     ,offset,t),extrema[0]*=-1;
    extrema[1]=getValueAsScalar(arr,size-1,offset,t);
  }

  exaCommGop(comm,extrema,2,exaScalar_t,exaMaxOp);
  extrema[0]*=-1;
}
