#ifndef LAMBERT
#define LAMBERT
//==========
typedef struct SimEnvStruct SimEnvStruct;
typedef struct Vector3d Vector3d;
typedef struct Quaterniond Quaterniond;

void Lambert_Initialization(SimEnvStruct *simEnvData);
void Lambert_Step(SimEnvStruct *simEnvData);
void Lambert_Termination();
//==========
#endif