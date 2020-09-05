#ifndef OSADOCA_STRUCTS
#define OSADOCA_STRUCTS

extern const double PI;

// Equivalent in C of the Unity Vector3d class
typedef struct Vector3d {
    double x;
    double y;
    double z;
}Vector3d;
typedef struct Vector3d Vector3d;

Vector3d *NewVector3d(double x, double y, double z);
Vector3d *V3d_Add(Vector3d *vecA, Vector3d *vecB);
Vector3d *V3d_Substract(Vector3d *vecA, Vector3d *vecB);
// Element wise multiplication of two Vector3d
Vector3d *V3d_Multiply(Vector3d *vecA, Vector3d *vecB);
// Multiplication of a Vector3d by a scalar
Vector3d *V3d_Multiply_S(double factor, Vector3d *vecA);
// Cross product of two Vector3d
Vector3d *Cross(Vector3d *vecA, Vector3d *vecB);
// Magnitude of the Vector3d
double V3d_Magnitude(Vector3d *vec);
// Square Magnitude of the Vector3d
double V3d_SqrMagnitude(Vector3d *vec);
char *V3d_ToString(Vector3d *vec, int decimalDigits);

// Equivalent in C of the Unity Quaterniond class
typedef struct Quaterniond {
    double x;
    double y;
    double z;
    double w;
} Quaterniond;
typedef struct Quaterniond Quaterniond;

// Constructor
Quaterniond *NewQuaterniond(double x,double y,double z,double w);
// Constructor for a Pure Quaterniond (a scalar part W of 0)
Quaterniond *New_Pure_Quaterniond(double x,double y,double z);
// Constructor for an Imaginary Quaterniond (a vectorial part (X,Y,Z) of (0,0,0))
Quaterniond *New_Imaginary_Quaterniond(double w);
// Creates new Quaterniond from the specified Euler angles
Quaterniond *NewQuaterniond_From_Euler(double xAngle,double yAngle,double zAngle);
// Convert the specified Quaterniond to its representation using Euler angles in degrees
Vector3d *Q_ConvertToEulerAngles(Quaterniond *quat);
// Returns the vectorial part (X,Y,Z) of the Quaterniond
Vector3d *Q_GetVectorialPart(Quaterniond *quat);
// Returns the scalar part (W) of the Quaterniond
double Q_GetScalarPart(Quaterniond *quat);
// Returns the conjugate of the specified Quaterniond
Quaterniond *Q_Conjugate(Quaterniond *quat);
// Normalize the specified Quaterniond
Quaterniond *Q_Normalize(Quaterniond *quat);
// Multiply two quaternions, applying first rotation 'quatA', then 'quatB'
Quaterniond *Q_QuatMultiply(Quaterniond *quatA, Quaterniond *quatB);
// Rotates a 3D point 'vec' by rotation 'quatA'
Vector3d *Q_RotateVec(Quaterniond *quatA, Vector3d *vec);
// Returns the magnitude of the specified quaterniond
double Q_Magnitude(Quaterniond *quat);
// Returns the square magnitude of the specified quaterniond
double Q_SqrMagnitude(Quaterniond *quat);
// Returns the string representation of the specified Quaterniond: (x;y;z;w)
char *Q_ToString(Quaterniond *quat, int decimalDigits);

// Struct containing the received data of the Simulation Env
typedef struct SimEnvStruct{
    Vector3d *shipAcc;
    Vector3d *shipVelocity;
    Vector3d *shipVelocityIncr;
    Vector3d *shipWorldPos;
    Quaterniond *deltaRotation;
} SimEnvStruct;
typedef struct SimEnvStruct SimEnvStruct;
SimEnvStruct *Parse_SimEnv_ReceivedData(char *simEnvReceivedData, char *delimiter, SimEnvStruct *simEnvData, bool printToConsole);

char **Split_String(char *stringToParse, char *delimiter, int *nbElemsToExtract);
//==========================
//==========================
#endif