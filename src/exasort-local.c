#include <exasort-impl.h>

int exaSortLocal(sort_data sd)
{
  exaBuffer buf; exaBufferCreate(&buf,0);

  exaArray arr=sd->array;
  int nfields=sd->nfields;
  int i=nfields-1;

  exaSortField(arr,sd->t[i],sd->offset[i],buf,0),i--;
  while(i>=0)
    exaSortField(arr,sd->t[i],sd->offset[i],buf,1),i--;

  uint n          =sd->array->arr.n;
  size_t unit_size=sd->unit_size;
  size_t align    =sd->align;
  void *ptr       =sd->array->arr.ptr;
  sarray_permute_buf_(align,unit_size,ptr,n,&buf->buf);

  exaFree(buf);
}

int exaSortArray(exaArray arr,exaDataType t,uint offset)
{
  sort_data data; exaMallocArray(1,sizeof(*data),(void**)&data);
  data->array=arr,data->nfields=1;
  data->t[0]=t,data->offset[0]=offset;
  data->align=exaArrayGetAlign(arr);
  data->unit_size=exaArrayGetUnitSize(arr);
  exaSortLocal(data);
  exaFree(data);
}

int exaSortArray2(exaArray arr,exaDataType t1,uint offset1,
  exaDataType t2,uint offset2)
{
  sort_data data; exaMallocArray(1,sizeof(*data),(void**)&data);
  data->array=arr,data->nfields=2;
  data->t[0]=t1,data->offset[0]=offset1;
  data->t[1]=t2,data->offset[1]=offset2;
  data->align=exaArrayGetAlign(arr);
  data->unit_size=exaArrayGetUnitSize(arr);
  exaSortLocal(data);
  exaFree(data);
}

int exaSortArray3(exaArray arr,exaDataType t1,uint offset1,
  exaDataType t2,uint offset2,exaDataType t3,uint offset3)
{
  sort_data data; exaMallocArray(1,sizeof(*data),(void**)&data);
  data->array=arr,data->nfields=3;
  data->t[0]=t1,data->offset[0]=offset1;
  data->t[1]=t2,data->offset[1]=offset2;
  data->t[2]=t3,data->offset[2]=offset3;
  data->align=exaArrayGetAlign(arr);
  data->unit_size=exaArrayGetUnitSize(arr);
  exaSortLocal(data);
  exaFree(data);
}
