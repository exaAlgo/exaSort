#include <exasort-impl.h>
#include <exa-memory.h>

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
      hdata->data=data; hdata->probes=NULL; hdata->probe_cnt=NULL;
      exaHyperCubeSort(hdata,&dup);
      exaFree(hdata->probes); exaFree(hdata->probe_cnt);
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

