#ifndef _EXASORT_HYPERCUBE_H_
#define _EXASORT_HYPERCUBE_H_

#include "exasort-impl.h"

#define histoSortInitProbes(T,array_,S,field,probes,probecounts,comm) \
  do { \
    /* Allocate space for probes and counts */ \
    exaInt size=exaCommSize(comm); \
    int nprobes=3*(size-1); \
    exaMalloc(nprobes,&probes); \
    exaMalloc(nprobes,&probecounts); \
    \
    /* TODO: Put in a macro */ \
    T *ptr=exaArrayGetPointer(array_); \
    exaInt n=exaArrayGetSize(array_); \
    S extrema[2]; \
    extrema[0]=-(ptr[0].field),extrema[1]=ptr[n-1].field; \
    exaCommGop(comm,extrema,2,exaTypeGetDataType(S),EXA_MAX); \
    extrema[0]*=-1; \
    exaScalar range=extrema[1]-extrema[0]; \
    exaScalar delta=range/size; \
    \
    exaInt i; \
    for(i=1; i<size; i++) { \
      h->histogram->probes[3*i-3]=extrema[0]; \
      h->histogram->probes[3*i-2]=extrema[0]+i*delta; \
      h->histogram->probes[3*i-1]=extrema[1]; \
    } \
  } while(0)

#define histoSortUpdateProbeCounts(T,array_,field,probes,probecounts,comm) \
  do { \
    exaInt lelt=exaArrayGetSize(array_); \
    T *ptr=exaArrayGetPointer(array_); \
    exaInt size=exaCommSize(comm); \
    exaInt nprobes=3*(size-1); \
    \
    exaInt i; \
    for(i=0; i<nprobes; i++) { \
      probecounts[i]=0; \
    } \
    \
    T *p,*e; \
    for(p=ptr,e=p+lelt; p!=e; p++){ \
      /* need to update as we don't keep the probes sorted */ \
      /* TODO: This could be improved */ \
      for(i=0; i<nprobes; i++){ \
        if(p->field<probes[i]){ \
          probecounts[i]++; \
        } \
      } \
    } \
    \
    /* global reduction */\
    exaReduce(c,probecounts,probecounts,nprobes,exaTypeGetDataType(exaLong),EXA_SUM); \
  } while(0);

inline int histoSortReachedThreshold(exaComm c,exaLong *probecounts,exaLong N,
                                     exaInt threshold)
{
  exaInt size=exaCommSize(c);
  exaInt rank=exaCommRank(c);

  exaInt localN=N/size;
  exaInt nrem=N-localN*size;

  exaInt converged=1;
  exaInt i;
  if(rank==0) {
    for(i=1; i<size; i++) {
      exaLong expected=i*localN+min(i,nrem);
      if(abs(probecounts[3*i-2]-expected)>threshold) {
        converged=0;
        break;
      }
    }
  }

  exaCommBcast(c,&converged,1,exaTypeGetDataType(exaInt),0);
  return converged;
}

#if 0
void histoSortUpdateProbeI(exaScalar *probes,exaLong *probecounts,int i,
		           exaLong expected,GenmapInt threshold)
{
  exaInt indx=3*i-2;
  if(abs(probecounts[indx]-expected)<threshold) {
    //splitter is achieved
    return;
  }

  if(count[indx]<expected) { // we are going to dump the smaller probe
    probes[indx-1]=probes[indx];
  } else { // we are going to dump the larger probe
    probes[indx+1]=probes[indx];
  }

  probes[indx]=probes[indx-1]+(probes[indx+1]-probes[indx-1])/2;
}

#define histoSortUpdateProbes(probes,probecounts,N,threshold,comm) \
  do { \
    exaInt size=exaCommSize(comm); \
    exaInt rank=exaCommRank(comm); \
    \
    exaInt localN=N/size; \
    exaInt nrem=N-localN*size; \
    exaInt i; \
    exaLong expected; \
    if(rank==0) { \
      for(i=1; i<size; i++) { \
        expected=i*localN+min(i,nrem); \
        histoSortUpdateProbeI(probes,probecounts,i,expected,threshold); \
      } \
    } \
  } while(0);

#define exaSortHistogramSort(T,array_,S,field,proc,comm,buf) \
  do { \
    exaInt lelt=exaArrayGetSize(array_); \
    T *ptr=exaArrayGetPointer(T,array_); \
    \
    exaInt size=exaCommSize(comm); \
    exaInt rank=exaCommRank(comm); \
    /* TODO: following can be put in a macro */ \
    exaLong out[2][1],buf[2][1],in[1]; \
    in[0]=lelt; \
    exaCommScan(comm,out,in,buf,1,exaTypeGetDataType(exaLong),EXA_ADD); \
    exaLong start=out[0][0]; \
    exaLong nelg =out[1][0]; \
    \
    /* 10% of load balanced partition size */ \
    exaInt threshold=(nelg/(10*size)); \
    if(threshold<2) threshold=2; \
    \
    /* sort locally. */ \
    sarray_sort(T,ptr,(exaUInt)lelt,field,exaTypeGetGSSortType(S),(buf)); \
    \
    /* We are done if size==1 */ \
    if(size==1) return; \
    \
    /* else we continue */ \
    S *probes=NULL; \
    exLong *probecounts=NULL; \
    histoSortInitProbes(T,array_,S,field,probes,probecounts,comm); \
    \
    /* update probecounts */ \
    histoSortUpdateProbeCounts(T,array_,field,probes,probecounts,comm); \
    \
    int iter=0; \
    while(!histoSortReachedThreshold(comm,probecounts,nelg,threshold)){ \
      histoSortUpdateProbes(h,c,probecounts,threshold,field); \
      exaBcast(c,probes,nprobes,exaTypeGetDataType(exaLong)); \
      histoSortUpdateProbeCounts(); \
      iter++; \
    } \
    /* set destination processor id for each element */ \
    parRSBHistoSortSetProc(h,c,field,buf0); \
    \
    /* send elements to right processor */ \
    parRSBHistoSortTransferToProc(h,field,buf0); \
    exaScan(h,c); \
    /* sort locally */ \
    parRSBHistoSortLocalSort(h,c,field,buf0); \
    \
    /* do a load balancing step */ \
    parRSBHistoSortLoadBalance(h,c); \
    \
    /* send elements to the correct processor */ \
    parRSBHistoSortTransferToProc(h,field,buf0); \
    /* Finalize sort */ \
    exaFree(probes); \
    exaFree(probecounts); \
  } while(0);

#define exaHistogramSort(h,T,array_,S,field,proc) \
  exaSortHistogramSort(T,array_,S,field,proc,(exaGetComm(h)),(&(h->buf)));

#endif
#endif
