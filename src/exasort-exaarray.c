#include <exasort-impl.h>
#include <exa-memory.h>

int exaSortArray(exaArray arr,exaDataType t,uint offset)
{
  sort_data_private data;

  data.a=&arr->arr,data.nfields=1;
  data.t[0]=t,data.offset[0]=offset;

  data.align=exaArrayGetAlign(arr);
  data.unit_size=exaArrayGetUnitSize(arr);

  buffer_init(&data.buf,1024);
  sort_local(&data);
  buffer_free(&data.buf);
}

int exaSortArray2(exaArray arr,exaDataType t1,uint offset1,
  exaDataType t2,uint offset2)
{
  sort_data_private data;

  data.a=&arr->arr,data.nfields=2;
  data.t[0]=t1,data.offset[0]=offset1;
  data.t[1]=t2,data.offset[1]=offset2;

  data.align=exaArrayGetAlign(arr);
  data.unit_size=exaArrayGetUnitSize(arr);

  buffer_init(&data.buf,1024);
  sort_local(&data);
  buffer_free(&data.buf);
}

int exaSortArray3(exaArray arr,exaDataType t1,uint offset1,
  exaDataType t2,uint offset2,exaDataType t3,uint offset3)
{
  sort_data_private data;

  data.a=&arr->arr,data.nfields=3;
  data.t[0]=t1,data.offset[0]=offset1;
  data.t[1]=t2,data.offset[1]=offset2;
  data.t[2]=t3,data.offset[2]=offset3;

  data.align=exaArrayGetAlign(arr);
  data.unit_size=exaArrayGetUnitSize(arr);

  buffer_init(&data.buf,1024);
  sort_local(&data);
  buffer_free(&data.buf);
}
int exaSortPrivate(sort_data data,exaComm comm){
  struct comm *c=&comm->gsComm,dup; comm_dup(&dup,c);

  hypercube_sort_data hdata;

  int balance =data->balance;
  exaSortAlgo algo=data->algo;

  struct array *a=data->a;
  size_t usize   =data->unit_size;

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
  sd.unit_size=exaArrayGetUnitSize(array);
  sd.align    =exaArrayGetAlign(array);
  sd.t[0]=t,sd.offset[0]=offset;

  sd.a=&array->arr;

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
  sd.unit_size=exaArrayGetUnitSize(array);
  sd.align    =exaArrayGetAlign(array);
  sd.t[0]=t1,sd.offset[0]=offset1;
  sd.t[1]=t2,sd.offset[1]=offset2;

  sd.a=&array->arr;

  sd.algo=algo;
  sd.balance=balance;

  buffer_init(&sd.buf,1024);
  exaSortPrivate(&sd,comm);
  buffer_free(&sd.buf);

  return 0;
}
