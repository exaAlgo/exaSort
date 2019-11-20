#ifndef _EXASORT_FIELD_H_
#define _EXASORT_FIELD_H_

#include <exa.h>
#include <exa-impl.h>

int exaSortPermuteBuf(exaArray arr,exaBuffer buf);
int exaSortField(exaArray arr,exaDataType t,exaUInt fieldOffset,exaBuffer buf,int keep);

#define min(a,b) ((a)<(b) ? (a) : (b))

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

#endif
