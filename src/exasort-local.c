#include <exasort-impl.h>

int exaSortLocal(exaSortData data)
{
  exaBuffer buf; exaBufferCreate(&buf,0); // Not initializing on purpose

  exaArray arr=data->array;
  int nFields=data->nFields,i=0;

  while(i<nFields)
    exaSortField(arr,data->t[i],data->offset[i],buf,i!=0),i++;

  exaSortPermuteBuf(arr,buf);
  exaFree(buf);
}

int exaSortArray(exaArray arr,exaDataType t,exaUInt offset)
{
  exaSortData data; exaMallocArray(1,sizeof(*data),(void**)&data);
  data->array=arr,data->nFields=1;
  data->t[0]=t,data->offset[0]=offset;
  exaSortLocal(data);
  exaFree(data);
}

int exaSortArray2(exaArray arr,exaDataType t1,exaUInt offset1,exaDataType t2,exaUInt offset2)
{
  exaSortData data; exaMallocArray(1,sizeof(*data),(void**)&data);
  data->array=arr,data->nFields=1;
  data->t[0]=t1,data->offset[0]=offset1;
  data->t[1]=t2,data->offset[1]=offset2;
  exaSortLocal(data);
  exaFree(data);
}

#define exaArrayLoadBalance(T,array_,S,field,proc,comm,buff) do{ \
  exaInt np=exaCommSize(comm); \
  T *ptr=exaArrayGetPointer(array_); \
  exaInt lelt=exaArrayGetSize(array_); \
  \
  exaLong out[2][1],buf[2][1],in[1]; \
  in[0]=lelt; \
  exaCommScan(comm,out,in,buf,1,exaTypeGetDataType(exaLong),exaAddOp); \
  exaLong start=out[0][0]; \
  exaLong nel  =out[1][0]; \
  exaInt pNel   = nel/np; \
  exaInt nrem   = nel-pNel*np; \
  \
  exaInt idCount= 0; \
  exaLong upLimit=0; \
  while(upLimit<start) {\
    idCount++; \
    upLimit=pNel*idCount+min(idCount,nrem); \
  } \
  \
  exaLong downLimit = start; \
  exaInt i=0; \
  do { \
    exaInt end=min(upLimit-start,lelt); \
    for(; i<end; i++) \
      ptr[i].proc = idCount - 1; \
    downLimit = upLimit; \
    idCount++; \
    upLimit = idCount*pNel+min(idCount,nrem); \
  } while(i<lelt); \
  exaArrayTransfer(array_,offsetof(T,proc),1,comm); \
  ptr=exaArrayGetPointer(array_); \
  lelt=exaArrayGetSize(array_); \
  sarray_sort(T,ptr,(exaUInt)lelt,field,exaTypeGetGSSortType(S),&(buff->buf)); \
} while(0)
