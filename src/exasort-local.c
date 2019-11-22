#include <exasort-impl.h>

int exaSortArray(exaArray arr,exaDataType t,exaUInt offset){
  exaBuffer buf; exaBufferCreate(&buf,0); // Not initializing on purpose
  exaSortField(arr,t,offset,buf,0);
  exaSortPermuteBuf(arr,buf);
  exaFree(buf);
}

int exaSortArray2(exaArray arr,exaDataType t1,exaUInt offset1,exaDataType t2,exaUInt offset2){
  exaBuffer buf; exaBufferCreate(&buf,0); // Not initializing on purpose
  exaSortField(arr,t2,offset2,buf,0);
  exaSortField(arr,t1,offset1,buf,1);
  exaSortPermuteBuf(arr,buf);
  exaFree(buf);
}
