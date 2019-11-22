#include <exasort-impl.h>
#include <exasort-binsort.h>

/* assumes array is locally sorted */
int exaArraySetBin(exaUInt **proc_,exaArray arr,exaDataType t,exaUInt offset,exaComm comm)
{
  exaInt np=exaCommSize(comm);

  exaInt size=exaArrayGetSize(arr);
  exaCalloc(size,proc_);
  exaUInt *proc=*proc_;

  exaScalar extrema[2];
  extrema[0]=getValueAsScalar(arr,0     ,offset,t),extrema[0]*=-1;
  extrema[1]=getValueAsScalar(arr,size-1,offset,t);

  exaCommGop(comm,extrema,2,exaScalar_t,exaMaxOp);
  extrema[0]*=-1;
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
  exaSortArray(array,t,offset); /* local sort */

  exaUInt *proc;
  exaArraySetBin(&proc,array,t,offset,comm); /* Set destination bin */

  exaArrayTransferExt(array,proc,comm); /* Transfer to destination processor */
  exaFree(proc);

  exaSortArray(array,t,offset); /* locally sort again */
  //if(loadBalance)
  //  exaArrayLoadBalance(array,t,offset,comm,buf);
}
