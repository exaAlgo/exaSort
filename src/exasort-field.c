#include <exasort-field.h>

int exaSortPermuteBuf(exaArray arr,exaBuffer buf){
  exaUInt nUnits =exaArrayGetSize(arr);
  size_t unitSize=exaArrayGetUnitSize(arr);
  size_t align   =exaArrayGetAlign(arr);
  void *ptr      =exaArrayGetPointer(arr);

  sarray_permute_buf_(align,unitSize,ptr,nUnits,&buf->buf);

  return 0;
}

int exaSortField(exaArray arr,exaDataType t,exaUInt fieldOffset,exaBuffer buf,
  int keep)
{
  exaUInt nUnits   =exaArrayGetSize(arr);
  unsigned unitSize=(unsigned)exaArrayGetUnitSize(arr);
  void *ptr        =exaArrayGetPointer(arr);

  size_t doubleSize=sizeof(double);
  switch(t){
    case exaScalar_t:
      if(doubleSize==sizeof(exaScalar))
        gslib_sortp_double(&buf->buf,keep,(double*)((char*)(ptr)+fieldOffset),
          nUnits,unitSize);
      printf("I am here\n");
      break;
    case exaULong_t:
      gslib_sortp_ull(&buf->buf,keep,(exaULong*)((char*)ptr+fieldOffset),
        nUnits,unitSize);
      break;
    case exaUInt_t:
      gslib_sortp_ui (&buf->buf,keep,(exaUInt* )((char*)ptr+fieldOffset),
        nUnits,unitSize);
      break;
    default:
      break;
  }

  return 0;
}
