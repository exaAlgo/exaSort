#include <stdio.h>
#include <exasort.h>

typedef struct {
  exaInt proc;
  int key;
  double value;
} kvPair;

int main() {
  exaArray array;
  exaArrayCreate(kvPair,array,5);

  return 0;
}
