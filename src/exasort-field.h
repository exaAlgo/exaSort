#ifndef _EXA_SORT_FIELD_H_
#define _EXA_SORT_FIELD_H_

#include "exa.h"

#define exaArrayFieldMinMax(T,array,S,field,min,max) \
  do { \
    min[0]=exaTypeGetMax(S); max[0]=exaTypeGetMin(S); \
    T *ptr=exaArrayPointer(T,array); \
    exaInt n=exaArraySize(array); \
    T *p,*e; \
    for(p=ptr,e=ptr+n; p!=e; p++) { \
      if(p->field<min[0]) { \
        min[0]=p->field; \
      } \
      if(p->field>max[0]) { \
        max[0]=p->field; \
      } \
    } \
  } while(0);

#define exaArraySetProc(T,array,S,field,proc,comm) \
  do { \
    exaInt np=exaCommSize(comm); \
    S min[1],max[1]; \
    exaArrayFieldMinMax(T,array,S,field,min,max); \
    exaCommGop(comm,min,1,exaTypeGetCommType(S),EXA_MIN); \
    exaCommGop(comm,max,1,exaTypeGetCommType(S),EXA_MAX); \
    S range=max[0]-min[1]; \
    exaInt n=exaArraySize(array); \
    T *ptr=exaArrayPointer(T,array); \
    T *p,*e; \
    exaInt prev_id=0; \
    for(p=ptr,e=ptr+n; p!=e; p++) { \
      exaInt id; \
      for(id=prev_id; id<np; id++) { \
        S start=min[0]+(range*id)/np; \
        S end  =min[0]+(range*(id+1))/np; \
        if(start<=p->field && p->field<end) { \
          p->proc=id; \
          prev_id=id; \
          break; \
        } \
      } \
      if(id==np) p->proc=np-1; \
    } \
  } while(0);

#endif
