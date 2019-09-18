#ifndef _EXASORT_BIN_
#define _EXASORT_BIN_

#include "exa-impl.h"
#include "exasort-field.h"

#define exaSortBinSort(T,array_,S,field,proc,comm,buf) do{ \
  exaInt size=exaArrayGetSize(array_); \
  T *ptr=exaArrayGetPointer(array_); \
  sarray_sort(T,ptr,(exaUInt)size,field,exaTypeGetGSSortType(S),(buf)); /* local sort */ \
  exaArraySetProc(T,array_,S,field,proc,comm); /* Set destination bin */ \
  exaArrayTransfer(T,array_,proc,&(comm->cr)); /* Transfer to destination processor */ \
  size=exaArrayGetSize(array_); \
  ptr=exaArrayGetPointer(array_); \
  sarray_sort(T,ptr,(exaUInt)size,field,exaTypeGetGSSortType(S),(buf)); /* locally sort again */ \
}while(0)

#define exaBinSort(h,T,array_,S,field,proc) \
  exaSortBinSort(T,array_,S,field,proc,(exaGetComm(h)),(&(h->buf))); \
  exaArrayLoadBalance(T,array_,S,field,proc,(exaGetComm(h)),(&(h->buf)));

#endif
