#ifndef _EXA_SORT_FIELD_H_
#define _EXA_SORT_FIELD_H_

#include "exa.h"

#define exaArraySetProc(T,array,S,field,proc,comm) /* assumes array is locally sorted */ \
  do { \
    exaInt np=exaCommSize(comm); \
    T *ptr=exaArrayPointer(array); \
    exaInt n=exaArraySize(array); \
    \
    S extrema[2]; \
    extrema[0]=-(ptr[0].field),extrema[1]=ptr[n-1].field; \
    exaCommGop(comm,extrema,2,exaTypeGetDataType(S),exaMaxOp); \
    extrema[0]*=-1; \
    S range=extrema[1]-extrema[0]; \
    \
    T *p=ptr,*e; \
    exaInt id = 0; \
    do { \
      S start=extrema[0]+(range*id)/np; \
      S end  =extrema[0]+(range*(id+1))/np; \
      if(p->field>=end) {id++; continue;} \
      if(p->field<start) break; \
      for(e=ptr+n; p!=e && p->field<end; p++) \
        p->proc=id; \
      id++; \
    } while(id<np); \
    for(e=ptr+n; p!=e; p++) \
      p->proc=id-1; \
  } while(0);

#define min(a,b) ((a)<(b) ? (a) : (b))

#define exaSortLoadBalance(T,array_,S,field,proc,comm,buff) \
  do { \
    exaInt np=exaCommSize(comm); \
    T *ptr=exaArrayPointer(array_); \
    exaInt lelt=exaArraySize(array_); \
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
    exaArrayTransfer(T,array_,proc,&comm->cr); \
    ptr=exaArrayPointer(array_); \
    lelt=exaArraySize(array_); \
    sarray_sort(T,ptr,(exaUInt)lelt,field,exaTypeGetGSSortType(S),(buff)); \
  } while(0);

#endif
