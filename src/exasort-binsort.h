#include "exa-impl.h"
#include "exasort-field.h"

#define exaSortBinSort(T,array_,S,field,proc,comm,buf) \
  do { \
    exaInt size=exaArraySize(array_); \
    T *ptr=exaArrayPointer(T,array_); \
    sarray_sort(T,ptr,(exaUInt)size,field,exaTypeGetGSSortType(S),(buf)); /* local sort */ \
    exaArraySetProc(T,array_,S,field,proc,comm); /* Set destination bin */ \
    exaArrayTransfer(T,array_,proc,&(comm->cr)); /* Transfer to destination processor */ \
    size=exaArraySize(array_); \
    ptr=exaArrayPointer(T,array_); \
    sarray_sort(T,ptr,(exaUInt)size,field,exaTypeGetGSSortType(S),(buf)); /* locally sort again */ \
  } while(0);

#define exaBinSort(h,T,array_,S,field,proc) \
  exaSortBinSort(T,array_,S,field,proc,(exaGetComm(h)),(&(h->buf))); \
  exaSortLoadBalance(T,array_,S,field,proc,(exaGetComm(h)),(&(h->buf)));
