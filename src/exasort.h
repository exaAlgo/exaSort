#ifndef _EXASORT_H_
#define _EXASORT_H_

#include "exasort-binsort.h"
#include "exasort-histogram.h"

#define exaSortLocalSort(h,T,array_,S1,field1) do{ \
  exaInt size=exaArrayGetSize(array_); \
  T *ptr=exaArrayGetPointer(array_); \
  sarray_sort(T,ptr,(exaUInt)size,field1,exaTypeGetGSSortType(S1),&(h->buf)); \
}while(0)

#define exaSortLocalSort2(h,T,array_,S1,field1,S2,field2) do{ \
  exaInt size=exaArrayGetSize(array_); \
  T *ptr=exaArrayGetPointer(array_); \
  sarray_sort2(T,ptr,(exaUInt)size,field1,exaTypeGetGSSortType(S1), \
                                   field2,exaTypeGetGSSortType(S2),&(h->buf)); \
}while(0)

#define exaSortLocalSort3(h,T,array_,S1,field1,S2,field2,S3,field3) do{ \
  exaInt size=exaArrayGetSize(array_); \
  T *ptr=exaArrayGetPointer(array_); \
  sarray_sort_3(T,ptr,(exaUInt)size,field1,exaTypeGetGSSortType(S1), \
                                    field2,exaTypeGetGSSortType(S2), \
                                    field3,exaTypeGetGSSortType(S3),&(h->buf)); \
}while(0)

#endif // exasort
