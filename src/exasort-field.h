#ifndef _EXA_SORT_FIELD_H_
#define _EXA_SORT_FIELD_H_

#include "exa.h"

#define exaArraySetProc(T,array,S,field,proc,comm) /* assumes array is locally sorted */ \
  do { \
    exaInt np=exaCommSize(comm); \
    T *ptr=exaArrayPointer(T,array); \
    exaInt n=exaArraySize(array); \
    \
    S extrema[2]; \
    extrema[0]=-(ptr[0].field),extrema[1]=ptr[n-1].field; \
    exaCommGop(comm,extrema,2,exaTypeGetCommType(S),EXA_MAX); \
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

#endif
