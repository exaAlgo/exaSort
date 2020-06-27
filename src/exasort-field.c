#include <exasort-impl.h>

int exaSortPermuteBuf(exaArray arr,exaBuffer buf){
  uint nUnits =exaArrayGetSize(arr);
  size_t unitSize=exaArrayGetUnitSize(arr);
  size_t align   =exaArrayGetAlign(arr);
  void *ptr      =exaArrayGetPointer(arr);

  sarray_permute_buf_(align,unitSize,ptr,nUnits,&buf->buf);

  return 0;
}

int exaSortField(exaArray arr,exaDataType t,uint fieldOffset,
  exaBuffer buf,int keep)
{
  uint nUnits   =exaArrayGetSize(arr);
  unsigned unitSize=(unsigned)exaArrayGetUnitSize(arr);
  void *ptr        =exaArrayGetPointer(arr);

  size_t doubleSize=sizeof(double);
  size_t floatSize =sizeof(float );
  size_t scalarSize=sizeof(exaScalar);

  switch(t){
    case exaScalar_t:
#if(doubleSize==scalarSize)
        gslib_sortp_double(&buf->buf,keep,
          (double*)((char*)ptr+fieldOffset),nUnits,unitSize);
#elif(floatSize==scalarSize)
        gslib_sortp_float(&buf->buf,keep,
          (float*)((char*)ptr+fieldOffset),nUnits,unitSize);
#endif
      break;
    case exaULong_t:
      gslib_sortp_ull(&buf->buf,keep,
        (exaULong*)((char*)ptr+fieldOffset),nUnits,unitSize);
      break;
    case exaLong_t:
      gslib_sortp_ull(&buf->buf,keep,
        (exaLong* )((char*)ptr+fieldOffset),nUnits,unitSize);
      break;
    case exaUInt_t:
      gslib_sortp_ui (&buf->buf,keep,
        (exaUInt* )((char*)ptr+fieldOffset),nUnits,unitSize);
      break;
    case exaInt_t:
      gslib_sortp_ui (&buf->buf,keep,
        (exaInt*  )((char*)ptr+fieldOffset),nUnits,unitSize);
      break;
    default:
      break;
  }

  return 0;
}
