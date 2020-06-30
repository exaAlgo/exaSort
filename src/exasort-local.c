#include <exasort-impl.h>

int sort_field(struct array *arr,size_t usize,exaDataType t,
    uint off,buffer *buf,int keep)
{
  uint nunits=arr->n;
  void *ptr  =arr->ptr;

  size_t dsize=sizeof(double);
  size_t fsize =sizeof(float );
  size_t ssize=sizeof(exaScalar);

  switch(t){
    case exaScalar_t:
#if(dsize==ssize)
        gslib_sortp_double(buf,keep,(double*)((char*)ptr+off),
            nunits,usize);
#elif(fsize==ssize)
        gslib_sortp_float (buf,keep,(float *)((char*)ptr+off),
            nunits,usize);
#endif
      break;
    case exaULong_t:
      gslib_sortp_ull(buf,keep,(exaULong*)((char*)ptr+off),nunits,usize);
      break;
    case exaLong_t:
      gslib_sortp_ull(buf,keep,(exaLong* )((char*)ptr+off),nunits,usize);
      break;
    case exaUInt_t:
      gslib_sortp_ui (buf,keep,(exaUInt* )((char*)ptr+off),nunits,usize);
      break;
    case exaInt_t:
      gslib_sortp_ui (buf,keep,(exaInt*  )((char*)ptr+off),nunits,usize);
      break;
    default:
      break;
  }

  return 0;
}

int sort_local(sort_data sd)
{
  exaArray arr=sd->array;
  int nfields=sd->nfields;
  int i=nfields-1;
  size_t usize=exaArrayGetUnitSize(arr);

  sort_field(&arr->arr,usize,sd->t[i],sd->offset[i],&sd->buf,0),i--;
  while(i>=0)
    sort_field(&arr->arr,usize,sd->t[i],sd->offset[i],&sd->buf,1),i--;

  uint n          =sd->array->arr.n;
  size_t unit_size=sd->unit_size;
  size_t align    =sd->align;
  void *ptr       =sd->array->arr.ptr;
  sarray_permute_buf_(align,unit_size,ptr,n,&sd->buf);
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
