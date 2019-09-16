#ifndef _EXASORT_BIN_
#define _EXASORT_BIN_

#include "exa-impl.h"
#include "exasort-field.h"

#define exaSortBinSort(T,array_,S,field,proc,comm,buf) do{ \
  exaInt size=exaArraySize(array_); \
  T *ptr=exaArrayPointer(array_); \
  sarray_sort(T,ptr,(exaUInt)size,field,exaTypeGetGSSortType(S),(buf)); /* local sort */ \
  exaArraySetProc(T,array_,S,field,proc,comm); /* Set destination bin */ \
  exaArrayTransfer(T,array_,proc,&(comm->cr)); /* Transfer to destination processor */ \
  size=exaArraySize(array_); \
  ptr=exaArrayPointer(array_); \
  sarray_sort(T,ptr,(exaUInt)size,field,exaTypeGetGSSortType(S),(buf)); /* locally sort again */ \
}while(0)

#define exaBinSort(h,T,array_,S,field,proc) \
  exaSortBinSort(T,array_,S,field,proc,(exaGetComm(h)),(&(h->buf))); \
  exaArrayLoadBalance(T,array_,S,field,proc,(exaGetComm(h)),(&(h->buf)));

#define exaSortLocalSort2(h,T,array_,S1,field1,S2,field2) do{ \
  exaInt size=exaArraySize(array_); \
  T *ptr=exaArrayPointer(array_); \
  sarray_sort2(T,ptr,(exaUInt)size,field1,exaTypeGetGSSortType(S1), \
                                   field2,exaTypeGetGSSortType(S2),&(h->buf)); \
}while(0)

#define exaSortLocalSort3(h,T,array_,S1,field1,S2,field2,S3,field3) do{ \
  exaInt size=exaArraySize(array_); \
  T *ptr=exaArrayPointer(array_); \
  sarray_sort_3(T,ptr,(exaUInt)size,field1,exaTypeGetGSSortType(S1), \
                                    field2,exaTypeGetGSSortType(S2), \
                                    field3,exaTypeGetGSSortType(S3),&(h->buf)); \
}while(0)

#endif
