// MOD-3 : Added container object and struct definition
#include "types.h"
#define NPROC 64

// MOD-3 : Container struct

typedef struct{
  int containerIDs[NPROC];
  int numActive;
  int procIDs[NPROC][NPROC];
  int notAllowed[NPROC][100];
  int memorylog[NPROC];
  int gva[NPROC];
} containerStruct;

extern containerStruct container;