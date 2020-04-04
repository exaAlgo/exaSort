#include <exasort-impl.h>

/* assumes array is locally sorted */
int setBin(exaUInt **proc_,exaSortData data,exaComm comm)
{
  exaArray arr  =data->array;
  exaDataType t =data->t[0];
  exaUInt offset=data->offset[0];

  exaInt np=exaCommSize(comm);

  exaInt size=exaArrayGetSize(arr);
  exaCalloc(size,proc_);
  if(size==0) return 0;

  exaUInt *proc=*proc_;

  exaScalar extrema[2];
  getArrayExtrema((void*)extrema,data,0,comm);
  exaScalar range=extrema[1]-extrema[0];

  exaUInt id=0;
  exaUInt index=0;
  do{
    exaScalar end=extrema[0]+(range/np)*(id+1);
    while(index<size){
      exaScalar val=getValueAsScalar(arr,index,offset,t);
      if(val<=end) proc[index++]=id;
      else break;
    }
    id++;
  }while(id<np && index<size);
  for(; index<size; index++)
    proc[index]=np-1;
}

int exaBinSort(exaSortData data,exaComm comm)
{
  // Local sort
  exaSortLocal(data);

  // Set destination bin
  exaUInt *proc;
  setBin(&proc,data,comm);

  // Transfer to destination processor
  exaArrayTransferExt(data->array,proc,comm);
  exaFree(proc);

  // Locally sort again
  exaSortLocal(data);
}
