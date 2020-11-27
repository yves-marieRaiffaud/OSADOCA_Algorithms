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
//====
#include <sys/mman.h>
#include <alchemy/task.h>
#include <alchemy/timer.h>
//====
#include "TCP_IP_Client.c"
#include "OSADOCA_Structs.c"
#include "Lambert.c"
#include "Impact_Restitution.c"
#include "Constants.h"
#include "Constants.c"

typedef struct TCP_IP_Client TCP_IP_Client;
typedef struct Vector3d Vector3d;
typedef struct Quaterniond Quaterniond;
typedef struct SimEnvStruct SimEnvStruct;

//==========================================
#define SIM_ENV_DATA_DELIMITER ";"
#define PERIOD_POWER_NUMBER_RECEIVING_SIMENV 9
float SIM_ENV_RECEIVING_TASK_PERIOD;
SimEnvStruct *simEnvData;
//======
#define PERIOD_POWER_NUMBER_SENDING_CONTROL_ORDERS 9
float CONTROL_ORDERS_SENDING_TASK_PERIOD;
//============
#define PERIOD_POWER_NUMBER_NAV_ALGO 9
float NAV_ALGO_TASK_PERIOD;
//============
RT_TASK simEnvReceivingTask, controlOrdersSendingTask, navAlgoTask;
//==========================================
void ReceivingTask_Periodic(void *arg)
{
    rt_task_set_periodic(NULL, TM_NOW, SIM_ENV_RECEIVING_TASK_PERIOD);
    //===================================
    TCP_IP_Client *client = (TCP_IP_Client*)arg;
    while(1)
    {
        int read_size = client->m_SocketReceive(client);
        Parse_SimEnv_ReceivedData(client->receivedData, SIM_ENV_DATA_DELIMITER, simEnvData, true);
        //===================================
        rt_task_wait_period(NULL);
    }
}
//=============
void SendingTask_Periodic(void *arg)
{
    rt_task_set_periodic(NULL, TM_NOW, CONTROL_ORDERS_SENDING_TASK_PERIOD);
    //===================================
    TCP_IP_Client *client = (TCP_IP_Client*)arg;
    while(1)
    {
        //####SHOULD GENERATE/GATHER DATA TO SEND HERE#############
        float rdnNB = ((float)rand()/(float)(RAND_MAX)) * 1;
        char buf[10];
        gcvt(rdnNB, 10, buf);
        strcpy(client->toSendData, buf);
        printf("Sending [%s]\n", client->toSendData);
        int shortRetval = client->m_SocketSend(client);
        //===================================
        rt_task_wait_period(NULL);
    }
}
//=============
void Navigation_Algo_Periodic(void *arg)
{
    rt_task_set_periodic(NULL, TM_NOW, NAV_ALGO_TASK_PERIOD);
    //===================================
    Lambert_Initialization(simEnvData);
    while(1)
    {
        Lambert_Step(simEnvData);
        //===================================
        rt_task_wait_period(NULL);
    }
    Lambert_Termination();
}
//==========================================
//==========================================
TCP_IP_Client *New_TCP_IP_Client(char* ip, int port)
{
    TCP_IP_Client *client = malloc(sizeof(TCP_IP_Client));
    client->RECEIVED_DATA_LENGTH = 256;
    client->PRINT_RECEIVED_DATA = false;
    client->RECEIVE_TIMEOUT = 3600;
    client->ipAddress = ip;
    client->port = port;
    
    client->toSendData = malloc(sizeof(char)*256);
    //client->toSendData[0] = '\0'; ////add a null terminator in case the string is used before any other initialization.
    client->receivedData = malloc(sizeof(char)*256);
    //client->receivedData[0] = '\0'; ////add a null terminator in case the string is used before any other initialization.
    strcpy(client->toSendData, "");
    strcpy(client->receivedData, "");

    client->m_SocketCreate = SocketCreate;
    client->m_SocketConnect = SocketConnect;
    client->m_SocketSend = SocketSend;
    client->m_SocketReceive = SocketReceive;
    client->m_SocketTerminate = SocketTerminate;

    return client;
}
//==========================================
//==========================================
bool Check_ARGV(int argc)
{
    if(argc != 3 && argc != 4)
    {
        printf("At least 2 parameters must be specified : the IP to connect to and its port\n");
        printf("****HELP:\n-First arg: local IP of the computer hosting Unity\n-Second arg: port for the Simulation Env receiver\n");
        printf("(-OPTIONAL ARGS)\n-Third: port for the control orders sender\n");
        return false;
    }
    return true;
}

int main(int argc, char *argv[])
{
    Vector3d *target = NewVector3d(equaREarth,0,0);
    Vector3d *position = NewVector3d(0,6578000,0);
    Vector3d *velocity = NewVector3d(6000,0,0);

    int counter = 0; // Keeping track of the number of iterations
    Vector3d *hitError = NewVector3d(INFINITY,INFINITY,INFINITY); // Error in meters w.r.t the target impact point
    Vector3d *lastValidVel = velocity; // Kepping in memory the last velocity whose trajectory intersected the Earth
    double startTheta = atan2(position->x,position->y)*rad2deg; // Start angle in degrees
    double requiredTheta = atan2(target->x,target->y)*rad2deg; // Angle in degrees to reach between the start & end position
    double theta; // angle computed during loops

    double startTime = rt_timer_read(); // for performance measure

    while(V3d_Magnitude(hitError) > 100000 && counter < 100 || V3d_IsNan(hitError)) {
        printf("\nStarting iteration %d\n", counter);
        printf("Velocity = %s", V3d_ToString(velocity,5));
        OrbitParams *orbitParams = Orbit_From_RV(position, velocity, muEarth*pow(10,muExponent));

        if(orbitParams->rp <= equaREarth) {
            printf("Will crash on Earth's surface !\n");
            lastValidVel = velocity;

            Vector3d *impactPoint_inPlane = Compute_ImpactPoints_inPlane(orbitParams, equaREarth);

            hitError = V3d_Substract(impactPoint_inPlane, target);
            theta = 90-atan2(impactPoint_inPlane->x, impactPoint_inPlane->y)*rad2deg;

            double circleHitError = sqrt(6378*6378 - pow(abs(hitError->y/1000)-6378, 2));
            double lb = -circleHitError/100; // m/s
            double ub = circleHitError/100; // m/s
            double xMin = -6378; // km
            double xMax = 6378; // km
            double velocityIncr = ((ub-lb)/(xMax-xMin)*(circleHitError-xMax)+ub);

            if(theta >= requiredTheta)
                velocityIncr = -velocityIncr;
            velocity = V3d_Add(velocity, V3d_Multiply_S(velocityIncr, NewVector3d(cos(startTheta), sin(startTheta), 0)));

            printf("CircleHitError = %.7f km ; Velocity Incr = %.6f m/s\n", circleHitError, velocityIncr);
            printf("impactPoint (km) = %s", V3d_ToString(V3d_Divide_S(impactPoint_inPlane,1000),5));
            printf("hitError (km) = %s", V3d_ToString(V3d_Divide_S(hitError,1000),5));
            printf("magnitude hitError (km) = %.5f\n", V3d_Magnitude(hitError));
        }
        else {
            velocity = V3d_Substract(lastValidVel, V3d_Multiply_S(1, NewVector3d(cos(startTheta), sin(startTheta), 0)));
            printf("Will NOT crash on Earth's surface !\n");
        }
        counter = counter + 1;
    }

    double taskDuration = (rt_timer_read() - startTime)*pow(10,-9);
    printf("Time spent on this task = %.10f s.\n", taskDuration);



    /*simEnvData = malloc(sizeof(SimEnvStruct));
    //=====DEBUG PURPOSES
    //char *str = "1 ; 2 ; 3.14 ; 4 ; 5 ; 6 ; 7.241 ; 8.2412 ; 9.141 ; 10.21 ; 11.531 ; 12.2352 ; 0.01 ; 0.02 ; 0.03 ; 0.04";
    //Parse_SimEnv_ReceivedData(str, SIM_ENV_DATA_DELIMITER, simEnvData, true);

    SIM_ENV_RECEIVING_TASK_PERIOD = pow(10,PERIOD_POWER_NUMBER_RECEIVING_SIMENV);
    CONTROL_ORDERS_SENDING_TASK_PERIOD = pow(10,PERIOD_POWER_NUMBER_SENDING_CONTROL_ORDERS);
    NAV_ALGO_TASK_PERIOD = pow(10, PERIOD_POWER_NUMBER_NAV_ALGO);

    bool ok = Check_ARGV(argc);
    if(ok == false) { return 1; }

    mlockall(MCL_CURRENT | MCL_FUTURE); //Lock the memory to avoid memory swapping for this program

    TCP_IP_Client *clientSimEnv = New_TCP_IP_Client(argv[1], strtol(argv[2],NULL,10));
    clientSimEnv->m_SocketCreate(clientSimEnv);
    if(clientSimEnv->hSocket == -1) { printf("Could not create socket\n"); return 1; }
    if (clientSimEnv->m_SocketConnect(clientSimEnv) < 0) { perror("connect failed.\n"); return 1; }
    printf("Sucessfully conected with server at IP %s and port %d\n", clientSimEnv->ipAddress, clientSimEnv->port);

    TCP_IP_Client *clientControlOrders = New_TCP_IP_Client(argv[1], strtol(argv[3],NULL,10));
    clientControlOrders->m_SocketCreate(clientControlOrders);
    if(clientControlOrders->hSocket == -1) { printf("Could not create socket\n"); return 1; }
    if (clientControlOrders->m_SocketConnect(clientControlOrders) < 0) { perror("connect failed.\n"); return 1; }
    printf("Sucessfully conected with server at IP %s and port %d\n", clientControlOrders->ipAddress, clientControlOrders->port);

    rt_task_spawn(&simEnvReceivingTask,"SimEnvReceiving_Task",0,99,T_JOINABLE,&ReceivingTask_Periodic, clientSimEnv);
    rt_task_spawn(&controlOrdersSendingTask,"ControlOrderSending_Task",0,98,T_JOINABLE,&SendingTask_Periodic, clientControlOrders);
    rt_task_spawn(&navAlgoTask,"NavigationAlgo_Task",0,97,T_JOINABLE,&Navigation_Algo_Periodic, NULL);

    pause();
    clientSimEnv->m_SocketTerminate(clientSimEnv);
    clientControlOrders->m_SocketTerminate(clientControlOrders);*/
    return 0;
}