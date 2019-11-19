#include <exasort-field.h>

int exaSortPermuteBuf(exaArray arr,exaBuffer buf){
  exaUInt nUnits =exaArrayGetSize(arr);
  size_t unitSize=exaArrayGetUnitSize(arr);
  size_t align   =exaArrayGetAlign(arr);
  void *ptr      =exaArrayGetPointer(arr);

  sarray_permute_buf_(align,unitSize,ptr,nUnifs,buf->buf)

  return 0;
}

int exaSortField(exaArray arr,exaDataType t,exaUInt fieldOffset,exaBuffer buf,
  int keep){
  exaUInt nUnits =exaArrayGetSize(arr);
  size_t unitSize=exaArrayGetUnitSize(arr);
  void *ptr      =exaArrayGetPointer(arr);

  switch(t){
    case exaScalar_t:
      size_t doubleSize=sizeof(double);
      if(doubleSize==sizeof(exaScalar))
        sortp_double(buf->buf,keep,(exaScalar*)((char*)ptr+fieldOffset),
          nUnits,unitSize);
       else
        sortp_float (buf->buf,keep,(exaScalar*)((char*)ptr+fieldOffset),
          nUnits,unitSize);
      break;
    case exaULong_t:
      sortp_long  (buf->buf,keep,(exaULong* )((char*)ptr+fieldOffset),
        nUnits,unitSize);
      break;
    case exaUInt_t:
      sortp       (buf->buf,keep,(exaUInt*  )((char*)ptr+fieldOffset),
        nUnits,unitSize);
      break;
    default:
      break;
  }

  return 0;
}
