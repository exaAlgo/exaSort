#include "exa.h"
#include "exasort-field.h"

#include <math.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>

#define exaSortBinSort(T,array,S,field,proc,comm,buf) \
  sarray_sort(T,array,exaArrayGetSize(array),field,exaTypeGetType(S),buf); /* local sort */ \
  exaArraySetProc(T,array,S,field,proc,comm); /* Set destination bin */ \
  exaArrayTransfer(T,array,proc,comm->cr); /* Transfer to destination processor */ \
  sarray_sort(T,array,exaArrayGetSize(array),field,exaTypeGetType(S),buf); /* locally sort again */

#define exaBinSort(h,T,array,S,field,proc) \
  exaSortBinSort(T,array,S,field,proc,exaGetComm(h),h->buf);
