#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <unistd.h>
#include <stdbool.h> 
#include <math.h>
#include <time.h>
#include <Lambert.h>

typedef struct SimEnvStruct SimEnvStruct;

// simEnvData at step (t-1)
SimEnvStruct *simEnv_t_m_1;
// simEnvData at step (t)
SimEnvStruct *simEnv_t;

// Initialization of the algorithm
void Lambert_Initialization(SimEnvStruct *simEnvData)
{
    simEnv_t_m_1 = simEnvData;
    simEnv_t = simEnvData;
}

// Make a step
void Lambert_Step(SimEnvStruct *simEnvData)
{
    
}

// Terminate algorithm
void Lambert_Termination()
{

}