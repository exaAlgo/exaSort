#include <exasort-impl.h>

int exaSortLocal(sort_data data)
{
  exaBuffer buf; exaBufferCreate(&buf,0);

  exaArray arr=data->array;
  int nFields=data->nFields;
  int i=nFields-1;

  exaSortField(arr,data->t[i],data->offset[i],buf,0),i--;
  while(i>=0)
    exaSortField(arr,data->t[i],data->offset[i],buf,1),i--;

  exaSortPermuteBuf(arr,buf);
  exaFree(buf);
}

int exaSortArray(exaArray arr,exaDataType t,uint offset)
{
  sort_data data; exaMallocArray(1,sizeof(*data),(void**)&data);
  data->array=arr,data->nFields=1;
  data->t[0]=t,data->offset[0]=offset;
  exaSortLocal(data);
  exaFree(data);
}

int exaSortArray2(exaArray arr,exaDataType t1,uint offset1,
  exaDataType t2,uint offset2)
{
  sort_data data; exaMallocArray(1,sizeof(*data),(void**)&data);
  data->array=arr,data->nFields=2;
  data->t[0]=t1,data->offset[0]=offset1;
  data->t[1]=t2,data->offset[1]=offset2;
  exaSortLocal(data);
  exaFree(data);
}

int exaSortArray3(exaArray arr,exaDataType t1,uint offset1,
  exaDataType t2,uint offset2,exaDataType t3,uint offset3)
{
  sort_data data; exaMallocArray(1,sizeof(*data),(void**)&data);
  data->array=arr,data->nFields=3;
  data->t[0]=t1,data->offset[0]=offset1;
  data->t[1]=t2,data->offset[1]=offset2;
  data->t[2]=t3,data->offset[2]=offset3;
  exaSortLocal(data);
  exaFree(data);
}
