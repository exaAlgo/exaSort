#include <exasort-impl.h>
#include <exasort-binsort.h>

/* assumes array is locally sorted */
int exaArraySetBin(exaUInt **proc_,exaSortData data,exaComm comm)
{
  exaArray arr  =data->array;
  exaDataType t =data->t[0];
  exaUInt offset=data->offset[0];

  exaInt np=exaCommSize(comm);

  exaInt size=exaArrayGetSize(arr);
  exaCalloc(size,proc_);
  exaUInt *proc=*proc_;

  exaScalar extrema[2];
  getArrayExtrema((void*)extrema,data,0,comm);
  exaScalar range=extrema[1]-extrema[0];

  exaInt id = 0;
  exaUInt index=0;
  do{
    exaScalar start=extrema[0]+(range*id)/np;
    exaScalar end  =extrema[0]+(range*(id+1))/np;
    if(getValueAsScalar(arr,index,offset,t)>=end ) {id++; continue;}
    if(getValueAsScalar(arr,index,offset,t)<start) break;
    for(; index<size && getValueAsScalar(arr,index,offset,t)<end; index++)
      proc[index]=id;
    id++;
  }while(id<np);
  for(; index<size; index++)
    proc[index]=id-1;
}

int exaBinSort(exaArray array,exaDataType t,exaUInt offset,int loadBalance,exaComm comm)
{
  exaSortData data; exaMallocArray(1,sizeof(*data),(void**)&data);
  data->array=array,data->t[0]=t,data->offset[0]=offset;
  data->nFields=1,data->algo=exaSortAlgoBinSort;

  exaSortLocal(data); /* local sort */

  exaUInt *proc;
  exaArraySetBin(&proc,data,comm); /* Set destination bin */

  exaArrayTransferExt(array,proc,comm); /* Transfer to destination processor */
  exaFree(proc);

  exaSortLocal(data); /* locally sort again */
  //if(loadBalance)
  //  exaArrayLoadBalance(array,t,offset,comm,buf);
  exaFree(data);
}
