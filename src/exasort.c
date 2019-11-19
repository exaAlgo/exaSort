#include <exasort.h>

int exaSortArray(exaArray arr,exaDataType t,exaUInt offset,exaBuffer buf){
  exaSortField(arr,offset,t,buf,0);
  exaSortPermuteBuf(arr,buf);
}
