#include <exasort-impl.h>

/* assumes array is locally sorted */
int setBin(uint **proc_,sort_data data,uint field,struct comm *c)
{
  struct array *a=&data->array->arr;
  exaDataType t  =data->t[field];
  uint offset    =data->offset[field];

  sint np=c->np;
  uint size=a->n; exaCalloc(size,proc_); uint *proc=*proc_;

  if(size==0) return 0;

  double extrema[2]; get_extrema((void*)extrema,data,field,c);
  double range=extrema[1]-extrema[0];

  uint id=0;
  uint index=0;
  do{
    double end=extrema[0]+(range/np)*(id+1);
    while(index<size){
      double val=get_scalar(a,index,offset,data->unit_size,t);
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
  struct array *a=&data->array->arr;
  struct comm *c=&comm->gsComm;

  // Local sort
  sort_local(data);

  // Set destination bin
  uint *proc;
  setBin(&proc,data,0,c);

  // Transfer to destination processor
  struct crystal cr; crystal_init(&cr,c);
  sarray_transfer_ext_(a,data->unit_size,proc,sizeof(uint),&cr);
  crystal_free(&cr);

  exaFree(proc);

  // Locally sort again
  sort_local(data);
}
