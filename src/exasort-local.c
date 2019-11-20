#include <exasort.h>
#include <exasort-field.h>

int exaSortArray(exaArray arr,exaDataType t,exaUInt offset,exaBuffer buf){
  exaSortField(arr,offset,t,buf,0);
  exaSortPermuteBuf(arr,buf);
}

int exaSortArray2(exaArray arr,exaDataType t1,exaUInt offset1,exaDataType t2,exaUInt offset2,
  exaBuffer buf){
  exaSortField(arr,offset2,t2,buf,0);
  exaSortField(arr,offset1,t1,buf,1);
  exaSortPermuteBuf(arr,buf);
}
