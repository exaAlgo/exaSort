#include <exasort-impl.h>

exaScalar getValueAsScalar(exaArray arr,exaUInt i,exaUInt offset,exaDataType type)
{
 char* v=((char*)exaArrayGetPointer(arr)+i*exaArrayGetUnitSize(arr)+offset);
 exaInt dataI;
 exaUInt dataUi;
 exaLong dataL;
 exaULong dataUl;
 exaScalar data;

 switch(type){
   case exaInt_t:
     dataI=*((exaInt*)v);
     data=dataI;
     break;
   case exaUInt_t:
     dataUi=*((exaUInt*)v);
     data=dataUi;
     break;
   case exaLong_t:
     dataL=*((exaLong*)v);
     data=dataL;
     break;
   case exaULong_t:
     dataUl=*((exaULong*)v);
     data=dataUl;
     break;
   case exaScalar_t:
     data=*((exaScalar*)v);
     break;
   default:
     break;
 }

 return data;
}
