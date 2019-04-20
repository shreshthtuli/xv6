// MOD-3 : Added container object and struct definition
#include "types.h"
#include "stat.h"
#define NPROC 64

// MOD-3 : Container struct

typedef struct{
  int containerIDs[NPROC];
  int numActive;
  int procIDs[NPROC][NPROC];
} containerStruct;

extern containerStruct container;