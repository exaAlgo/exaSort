#include <exa-impl.h>
#include <exasort-impl.h>

gs_dom exa_to_gs(exaDataType t){
  gs_dom out;
  switch(t){
    case exaULong_t:
      out=gs_long;
      break;
    case exaUInt_t:
      out=gs_int;
      break;
    case exaScalar_t:
      out=gs_double;
      break;
    default:
      break;
  }
  return out;
}

int exaSortArray(exaArray arr,exaDataType t,uint offset)
{
  sort_data_private data;

  data.a=&arr->arr,data.nfields=1;
  data.t[0]=exa_to_gs(t),data.offset[0]=offset;

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
  data.t[0]=exa_to_gs(t1),data.offset[0]=offset1;
  data.t[1]=exa_to_gs(t2),data.offset[1]=offset2;

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
  data.t[0]=exa_to_gs(t1),data.offset[0]=offset1;
  data.t[1]=exa_to_gs(t2),data.offset[1]=offset2;
  data.t[2]=exa_to_gs(t3),data.offset[2]=offset3;

  data.align=exaArrayGetAlign(arr);
  data.unit_size=exaArrayGetUnitSize(arr);

  buffer_init(&data.buf,1024);
  sort_local(&data);
  buffer_free(&data.buf);
}

int exaSort(exaArray array,exaDataType t,uint offset,
  exaSortAlgo algo,int balance,exaComm comm)
{
  sort_data_private sd;

  sd.nfields=1;
  sd.unit_size=exaArrayGetUnitSize(array);
  sd.align    =exaArrayGetAlign(array);
  sd.t[0]=exa_to_gs(t),sd.offset[0]=offset;

  sd.a=&array->arr;

  sd.balance=balance;
  sd.algo=algo;

  buffer_init(&sd.buf,1024);
  sort_private(&sd,&comm->gsComm);
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
  sd.t[0]=exa_to_gs(t1),sd.offset[0]=offset1;
  sd.t[1]=exa_to_gs(t2),sd.offset[1]=offset2;

  sd.a=&array->arr;

  sd.algo=algo;
  sd.balance=balance;

  buffer_init(&sd.buf,1024);
  sort_private(&sd,&comm->gsComm);
  buffer_free(&sd.buf);

  return 0;
}
