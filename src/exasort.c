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

void getArrayExtrema(void *extrema_,exaSortData data,exaComm comm){
  exaArray arr  =data->array;
  exaUInt offset=data->offset;
  exaDataType t =data->t;

  exaInt size=exaArrayGetSize(arr);

  exaScalar *extrema=(exaScalar *)extrema_;
  extrema[0]=getValueAsScalar(arr,0     ,offset,t),extrema[0]*=-1;
  extrema[1]=getValueAsScalar(arr,size-1,offset,t);

  exaCommGop(comm,extrema,2,exaScalar_t,exaMaxOp);
  extrema[0]*=-1;
}
