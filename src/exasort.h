#ifndef _EXASORT_H_
#define _EXASORT_H_

#include <exa.h>
#include <exa-impl.h>

int exaSortArray (exaArray arr,exaDataType t ,exaUInt offset);
int exaSortArray2(exaArray arr,exaDataType t1,exaUInt offset1,exaDataType t2,exaUInt offset2);

int exaBinSort(exaArray array,exaDataType t,exaUInt offset,int loadBalance,exaComm comm);

//#define exaSortLocalSort(h,T,array_,S1,field1) do{ \
//  exaInt size=exaArrayGetSize(array_); \
//  T *ptr=exaArrayGetPointer(array_); \
//  sarray_sort(T,ptr,(exaUInt)size,field1,exaTypeGetGSSortType(S1),&h->buf->buf); \
//}while(0)
//
//#define exaSortLocalSort2(h,T,array_,S1,field1,S2,field2) do{ \
//  exaInt size=exaArrayGetSize(array_); \
//  T *ptr=exaArrayGetPointer(array_); \
//  sarray_sort_2(T,ptr,(exaUInt)size,field1,exaTypeGetGSSortType(S1), \
//                                    field2,exaTypeGetGSSortType(S2),&h->buf->buf); \
//}while(0)
//
//#define exaSortLocalSort3(h,T,array_,S1,field1,S2,field2,S3,field3) do{ \
//  exaInt size=exaArrayGetSize(array_); \
//  T *ptr=exaArrayGetPointer(array_); \
//  sarray_sort_3(T,ptr,(exaUInt)size,field1,exaTypeGetGSSortType(S1), \
//                                    field2,exaTypeGetGSSortType(S2), \
//                                    field3,exaTypeGetGSSortType(S3),&h->buf->buf); \
//}while(0)

#endif // exasort
