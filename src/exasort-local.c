#include <exasort-impl.h>

int sort_field(struct array *arr,size_t usize,
    exaDataType t,uint fieldOffset,exaBuffer buf,int keep)
{
  uint nunits=arr->n;
  void *ptr  =arr->ptr;

  size_t doubleSize=sizeof(double);
  size_t floatSize =sizeof(float );
  size_t scalarSize=sizeof(exaScalar);

  switch(t){
    case exaScalar_t:
#if(doubleSize==scalarSize)
        gslib_sortp_double(&buf->buf,keep,
          (double*)((char*)ptr+fieldOffset),nunits,usize);
#elif(floatSize==scalarSize)
        gslib_sortp_float(&buf->buf,keep,
          (float*)((char*)ptr+fieldOffset),nunits,usize);
#endif
      break;
    case exaULong_t:
      gslib_sortp_ull(&buf->buf,keep,
        (exaULong*)((char*)ptr+fieldOffset),nunits,usize);
      break;
    case exaLong_t:
      gslib_sortp_ull(&buf->buf,keep,
        (exaLong* )((char*)ptr+fieldOffset),nunits,usize);
      break;
    case exaUInt_t:
      gslib_sortp_ui (&buf->buf,keep,
        (exaUInt* )((char*)ptr+fieldOffset),nunits,usize);
      break;
    case exaInt_t:
      gslib_sortp_ui (&buf->buf,keep,
        (exaInt*  )((char*)ptr+fieldOffset),nunits,usize);
      break;
    default:
      break;
  }

  return 0;
}

int sort_local(sort_data sd)
{
  exaBuffer buf; exaBufferCreate(&buf,0);

  exaArray arr=sd->array;
  int nfields=sd->nfields;
  int i=nfields-1;
  size_t usize=exaArrayGetUnitSize(arr);

  sort_field(&arr->arr,usize,sd->t[i],sd->offset[i],buf,0),i--;
  while(i>=0)
    sort_field(&arr->arr,usize,sd->t[i],sd->offset[i],buf,1),i--;

  uint n          =sd->array->arr.n;
  size_t unit_size=sd->unit_size;
  size_t align    =sd->align;
  void *ptr       =sd->array->arr.ptr;
  sarray_permute_buf_(align,unit_size,ptr,n,&buf->buf);

  exaFree(buf);
}

int exaSortArray(exaArray arr,exaDataType t,uint offset)
{
  sort_data_private data;

  data.array=arr,data.nfields=1;
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

  data.array=arr,data.nfields=2;
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

  data.array=arr,data.nfields=3;
  data.t[0]=t1,data.offset[0]=offset1;
  data.t[1]=t2,data.offset[1]=offset2;
  data.t[2]=t3,data.offset[2]=offset3;

  data.align=exaArrayGetAlign(arr);
  data.unit_size=exaArrayGetUnitSize(arr);

  buffer_init(&data.buf,1024);
  sort_local(&data);
  buffer_free(&data.buf);
}
