#include "exa-impl.h"
#include "exasort-field.h"

#include <math.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>

#define exaSortBinSort(T,array_,S,field,proc,comm,buf) \
  sarray_sort(T,array_,exaArraySize(array_),field,exaTypeGetType(S),buf); /* local sort */ \
  exaArraySetProc(T,array_,S,field,proc,comm); /* Set destination bin */ \
  exaArrayTransfer(T,array_,proc,&(comm->cr)); /* Transfer to destination processor */ \
  sarray_sort(T,array_,exaArraySize(array_),field,exaTypeGetType(S),buf); /* locally sort again */

#define exaBinSort(h,T,array_,S,field,proc) \
  exaSortBinSort(T,array_,S,field,proc,exaGetComm(h),&(h->buf));
