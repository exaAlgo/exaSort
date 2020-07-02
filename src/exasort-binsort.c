#include <exasort-impl.h>

/* assumes array is locally sorted */
int setBin(uint **proc_,sort_data data,exaComm comm)
{
  exaArray arr  =data->array;
  exaDataType t =data->t[0];
  uint offset=data->offset[0];

  sint np=exaCommSize(comm);

  sint size=exaArrayGetSize(arr);
  exaCalloc(size,proc_);
  if(size==0) return 0;

  uint *proc=*proc_;

  double extrema[2];
  getArrayExtrema((void*)extrema,data,0,comm);
  double range=extrema[1]-extrema[0];

  uint id=0;
  uint index=0;
  do{
    double end=extrema[0]+(range/np)*(id+1);
    while(index<size){
      double val=get_scalar(&arr->arr,index,offset,
          data->unit_size,t);
      if(val<=end) proc[index++]=id;
      else break;
    }
    id++;
  }while(id<np && index<size);
  for(; index<size; index++)
    proc[index]=np-1;
}

int exaBinSort(sort_data data,exaComm comm)
{
  // Local sort
  sort_local(data);

  // Set destination bin
  uint *proc;
  setBin(&proc,data,comm);

  // Transfer to destination processor
  exaArrayTransferExt(data->array,proc,comm);
  exaFree(proc);

  // Locally sort again
  sort_local(data);
}
