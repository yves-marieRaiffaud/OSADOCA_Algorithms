#include <sys/socket.h>
#include <arpa/inet.h> 
#include <unistd.h>
#include <stdbool.h> 
#include <math.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "../Constants.h"
#include "OSADOCA_Structs.h"

typedef struct Vector3d Vector3d;
typedef struct Quaterniond Quaterniond;
typedef struct Orbit_Shape Orbit_Shape;

double ClampValue(double lowerBound, double upperBound, double valueToClamp)
{
    double outputVal;
    if(valueToClamp <= lowerBound)
        outputVal = lowerBound;
    else if(valueToClamp >= upperBound)
        outputVal = upperBound;
    else
        outputVal = valueToClamp;
    return outputVal;
}
double ClampAngle_Deg(double lowerBound, double upperBound, double valueToClamp)
{
    int newAngle = valueToClamp;
    while(newAngle <= lowerBound) {
        newAngle = newAngle + 360;
    }
    while(newAngle > upperBound) {
        newAngle = newAngle - 360;
    }
    return newAngle;
}
double ClampAngle_RAD(double lowerBound, double upperBound, double valueToClamp)
{
    int newAngle = valueToClamp;
    while(newAngle <= lowerBound) {
        newAngle = newAngle + 2*PI;
    }
    while(newAngle > upperBound) {
        newAngle = newAngle - 2*PI;
    }
    return newAngle;
}

// Vector3d Constructor
Vector3d *NewVector3d(double x, double y, double z)
{
    Vector3d *vec = malloc(sizeof(Vector3d));
    vec->x = x;
    vec->y = y;
    vec->z = z;
    return vec;
}

// Shorthand for (1;0;0)
Vector3d *V3d_Right()
{
    return NewVector3d(1,0,0);
}
// Shorthand for (-1;0;0)
Vector3d *V3d_Left()
{
    return NewVector3d(-1,0,0);
}
// Shorthand for (0;0;1)
Vector3d *V3d_Forward()
{
    return NewVector3d(0,0,1);
}
// Shorthand for (0;0;-1)
Vector3d *V3d_Backward()
{
    return NewVector3d(0,0,-1);
}
// Shorthand for (0;1;0)
Vector3d *V3d_Up()
{
    return NewVector3d(0,1,0);
}
// Shorthand for (0;-1;0)
Vector3d *V3d_Down()
{
    return NewVector3d(0,-1,0);
}

// Add two Vector3d
Vector3d *V3d_Add(Vector3d *vecA, Vector3d *vecB)
{
    struct Vector3d *vec = malloc(sizeof(Vector3d));
    vec->x = vecA->x + vecB->x;
    vec->y = vecA->y + vecB->y;
    vec->z = vecA->z + vecB->z;
    return vec;
}
// Substract two Vector3d
Vector3d *V3d_Substract(Vector3d *vecA, Vector3d *vecB)
{
    Vector3d *vec = malloc(sizeof(Vector3d));
    vec->x = vecA->x - vecB->x;
    vec->y = vecA->y - vecB->y;
    vec->z = vecA->z - vecB->z;
    return vec;
}
// Dot product of two Vector3d
double V3d_Dot(Vector3d *vecA, Vector3d *vecB)
{
    return vecA->x*vecB->x + vecA->y*vecB->y + vecA->z * vecB->z;
}
// Multiplication of a Vector3d by a scalar
Vector3d *V3d_Multiply(Vector3d *vecA, Vector3d *vecB)
{
    Vector3d *vec = malloc(sizeof(Vector3d));
    vec->x = vecA->x * vecB->x;
    vec->y = vecA->y * vecB->y;
    vec->z = vecA->z * vecB->z;
    return vec;
}
// Multiplication of a Vector3d by a scalar
Vector3d *V3d_Multiply_S(double factor, Vector3d *vecA)
{
    Vector3d *vec = malloc(sizeof(Vector3d));
    vec->x = vecA->x * factor;
    vec->y = vecA->y * factor;
    vec->z = vecA->z * factor;
    return vec;
}
// Division of a Vector3d by a scalar
Vector3d *V3d_Divide_S(Vector3d *vecA, double denominator)
{
    Vector3d *vec = malloc(sizeof(Vector3d));
    vec->x = vecA->x/denominator;
    vec->y = vecA->y/denominator;
    vec->z = vecA->z/denominator;
    return vec;
}
// Cross product of two Vector3d
Vector3d *V3d_Cross(Vector3d *vecA, Vector3d *vecB)
{
    Vector3d *vec = malloc(sizeof(Vector3d));
    vec->x = vecA->y*vecB->z - vecA->z*vecB->y;
    vec->y = vecA->z*vecB->x - vecA->x*vecB->z;
    vec->z = vecA->x*vecB->y - vecA->y*vecB->x;
    return vec;
}
// Normalize the vector3d
Vector3d *V3d_Normalize(Vector3d *vec)
{
    double norm = V3d_Magnitude(vec);
    Vector3d *outputVec = malloc(sizeof(Vector3d));
    outputVec->x = vec->x/norm;
    outputVec->y = vec->y/norm;
    outputVec->z = vec->z/norm;
    return outputVec;
}
// Square Magnitude of the Vector3d
double V3d_SqrMagnitude(Vector3d *vec)
{
    return pow(vec->x,2) + pow(vec->y,2) + pow(vec->z,2);
}
// Magnitude of the Vector3d
double V3d_Magnitude(Vector3d *vec)
{
    return sqrt(V3d_SqrMagnitude(vec));
}
// Element-Wise Sign of the Vector3d
Vector3d *V3d_Sign(Vector3d *vec)
{
    double x, y, z;
    if(vec->x < 0)
        x = -1;
    else if(vec->x > 0)
        x = 1;
    else
        x = 0;
    if(vec->y < 0)
        y = -1;
    else if(vec->y > 0)
        y = 1;
    else
        y = 0;
    if(vec->z < 0)
        z = -1;
    else if(vec->z > 0)
        z = 1;
    else
        z = 0;
    return NewVector3d(x, y, z);
}
// Check if the vector or one of its components is nan
bool V3d_IsNan(Vector3d *vec)
{
    if(isnan(vec->x) || isnan(vec->y) || isnan(vec->z))
        return true;
    return false;
}
// Get string representation of the Vector3d
char *V3d_ToString(Vector3d *vec, int decimalDigits)
{
    int factor = 10 + decimalDigits + 1;
    char *returnText = malloc((3*factor+5)*sizeof(char));
    strcpy(returnText, "");

    char *x_string = malloc(factor*sizeof(char));
    char *y_string = malloc(factor*sizeof(char));
    char *z_string = malloc(factor*sizeof(char));
    sprintf(x_string,"%.10f",vec->x);
    sprintf(y_string,"%.10f",vec->y);
    sprintf(z_string,"%.10f",vec->z);

    strcat(returnText, "(");
    strcat(returnText, x_string);
    strcat(returnText, ";");
    strcat(returnText, y_string);
    strcat(returnText, ";");
    strcat(returnText, z_string);
    strcat(returnText, ")\n");

    return returnText;
}
//=====================================================================
//=====================================================================
//=====================================================================
//=====================================================================

// Constructor
Quaterniond *NewQuaterniond(double x,double y,double z,double w)
{
    Quaterniond *quat = malloc(sizeof(Quaterniond));
    quat->x = x;
    quat->y = y;
    quat->z = z;
    quat->w = w;
    return quat;
}
// Constructor for a Pure Quaterniond (a scalar part W of 0)
Quaterniond *New_Pure_Quaterniond(double x,double y,double z)
{
    double w=0;
    return NewQuaterniond(x,y,z,w);
}
// Constructor for an Imaginary Quaterniond (a vectorial part (X,Y,Z) of (0,0,0))
Quaterniond *New_Imaginary_Quaterniond(double w)
{
    double vecVal=0;
    return NewQuaterniond(vecVal,vecVal,vecVal,w);
}
// Creates a Quaterniond from the rotation axis 'axis' and the rotation value 'angle'
Quaterniond *Q_AngleAxis(double angle, Vector3d *axis, bool angleInDegree)
{
    Vector3d *axisN = V3d_Normalize(axis);
    double angleRad = angle;
    if(angleInDegree == true)
        angleRad = angle * deg2rad;
    double qx = axisN->x*sin(angleRad/2);
    double qy = axisN->y*sin(angleRad/2);
    double qz = axisN->z*sin(angleRad/2);
    double qw = cos(angleRad/2);
    Quaterniond *quat = NewQuaterniond(qx, qy, qz, qw);
    return Q_Normalize(quat);
}
// Returns the conjugate of the specified Quaterniond
Quaterniond *Q_Conjugate(Quaterniond *quat)
{
    Quaterniond *outputQuat = malloc(sizeof(Quaterniond));
    outputQuat->x = -quat->x;
    outputQuat->y = -quat->y;
    outputQuat->z = -quat->z;
    outputQuat->w = quat->w;
    return outputQuat;
}
// Returns the vectorial part (X,Y,Z) of the Quaterniond
Vector3d *Q_GetVectorialPart(Quaterniond *quat)
{
    Vector3d *vec = malloc(sizeof(Vector3d));
    vec->x = quat->x;
    vec->y = quat->y;
    vec->z = quat->z;
    return vec;
}
// Returns the scalar part (W) of the Quaterniond
double Q_GetScalarPart(Quaterniond *quat)
{
    return quat->w;
}
// Returns the square magnitude of the specified quaterniond
double Q_SqrMagnitude(Quaterniond *quat)
{
    double norm = pow(quat->x,2) + pow(quat->y,2) + pow(quat->z,2) + pow(quat->w,2);
}
// Returns the magnitude of the specified quaterniond
double Q_Magnitude(Quaterniond *quat)
{
    return sqrt(Q_SqrMagnitude(quat));
}
// Normalize the specified Quaterniond
Quaterniond *Q_Normalize(Quaterniond *quat)
{
    Quaterniond *outputQuat = malloc(sizeof(Quaterniond));
    outputQuat = quat;
    double norm = Q_Magnitude(outputQuat);
    outputQuat->x = outputQuat->x/norm;
    outputQuat->y = outputQuat->y/norm;
    outputQuat->z = outputQuat->z/norm;
    outputQuat->w = outputQuat->w/norm;
    return outputQuat;
}
// Creates new Quaterniond from the specified Euler angles. Angles must be specified in degrees
Quaterniond *NewQuaterniond_From_Euler(double xAngle,double yAngle,double zAngle)
{
    double cr = cos(xAngle*PI/360);
	double sr = sin(xAngle*PI/360);
	double cy = cos(yAngle*PI/360);
	double sy = sin(yAngle*PI/360);
	double cp = cos(zAngle*PI/360);
	double sp = sin(zAngle*PI/360);
	
    Quaterniond *quat = malloc(sizeof(Quaterniond));
    quat->x = cp*sr*cy+cr*sy*sp;
    quat->y = cp*cr*sy-sp*sr*cy;
    quat->z = cp*sr*sy+sp*cr*cy;
    quat->w = cr*cy*cp-sp*sy*sr;
	return Q_Normalize(quat);
}
// Convert the specified Quaterniond to its representation using Euler angles in degrees
Vector3d *Q_ConvertToEulerAngles(Quaterniond *quat)
{
    // Quaternion order: XYZ
    double x = quat->x;
    double y = quat->y;
    double z = quat->z;
    double w = quat->w;
	
	// Angles computed in radian, then converted to degrees
	float roll = atan2(-2*(y*z-w*x),w*w-x*x-y*y+z*z)*180/PI;
	float yaw = asin(2*(x*z+w*y))*180/PI;
	float pitch = atan2(-2*(x*y-w*z),w*w+x*x-y*y-z*z)*180/PI;

	Vector3d *vec = malloc(sizeof(Vector3d));
	vec->x = roll;
	vec->y = yaw;
	vec->z = pitch;
    return vec;
}
// Multiply two quaternions, applying first rotation 'quatA', then 'quatB'
Quaterniond *Q_QuatMultiply(Quaterniond *quatA, Quaterniond *quatB)
{
    double a = quatA->x;
    double b = quatA->y;
    double c = quatA->z;
    double d = quatA->w;

    double e = quatB->x;
    double f = quatB->y;
    double g = quatB->z;
    double h = quatB->w;

	Quaterniond *outputQuat = malloc(sizeof(Quaterniond));
	outputQuat->x = d*e+a*h+b*g-c*f;
	outputQuat->y = d*f-a*g+b*h+c*e;
	outputQuat->z = d*g+a*f-b*e+c*h;
    outputQuat->w = d*h-a*e-b*f-c*g;
    return outputQuat;
}
// Rotates a 3D point 'vec' by rotation 'quatA'
Vector3d *Q_RotateVec(Quaterniond *quatA, Vector3d *vec)
{
    // Creating the pure Quaternion associated with 'pointVec'
    Quaterniond *point = New_Pure_Quaterniond(vec->x, vec->y, vec->z);
    
    Quaterniond *q = Q_Normalize(quatA);
    Quaterniond *qStar = Q_Conjugate(q);
    Quaterniond *output = Q_QuatMultiply(Q_QuatMultiply(q, point), qStar);
    Vector3d *rotatedVec = Q_GetVectorialPart(output);
    return rotatedVec;
}
// Returns the string representation of the specified Quaterniond: (x;y;z;w)
char *Q_ToString(Quaterniond *quat, int decimalDigits)
{
    int factor = 10 + decimalDigits + 1;
    char *returnText = malloc((3*factor+5)*sizeof(char));
    strcpy(returnText,"");

    char *x_string = malloc(factor*sizeof(char));
    char *y_string = malloc(factor*sizeof(char));
    char *z_string = malloc(factor*sizeof(char));
    char *w_string = malloc(factor*sizeof(char));
    sprintf(x_string,"%.10f",quat->x);
    sprintf(y_string,"%.10f",quat->y);
    sprintf(z_string,"%.10f",quat->z);
    sprintf(w_string,"%.10f",quat->w);

    strcat(returnText, "(");
    strcat(returnText, x_string);
    strcat(returnText, ";");
    strcat(returnText, y_string);
    strcat(returnText, ";");
    strcat(returnText, z_string);
    strcat(returnText, ";");
    strcat(returnText, w_string);
    strcat(returnText, ")\n");

    return returnText;
}

SimEnvStruct *Parse_SimEnv_ReceivedData(char *simEnvReceivedData, char *delimiter, SimEnvStruct *simEnvData, bool printToConsole)
{
    //================================
    // Ship acceleration (in m/s^2)
    // Ship velocity (in m/s)
    // Ship velocity increment (in m/s)
    // Ship world position in the scene (in unity units)
    // Ship Delta Rotation Quaterniond
    //================================
    int nbElementsExtracted = 0;
    char **tokens = Split_String(simEnvReceivedData, delimiter, &nbElementsExtracted);
    assert(nbElementsExtracted-1 == 19); // '-1' because last extracted character is the null terminator 
    simEnvData->shipAcc = NewVector3d(atof(*(tokens + 0)), atof(*(tokens + 1)), atof(*(tokens + 2)));
    simEnvData->shipVelocity = NewVector3d(atof(*(tokens + 3)), atof(*(tokens + 4)), atof(*(tokens + 5)));
    simEnvData->shipVelocityIncr = NewVector3d(atof(*(tokens + 6)), atof(*(tokens + 7)), atof(*(tokens + 8)));
    simEnvData->shipWorldPos = NewVector3d(atof(*(tokens + 9)), atof(*(tokens + 10)), atof(*(tokens + 11)));
    simEnvData->shipWorldPos = NewVector3d(atof(*(tokens + 12)), atof(*(tokens + 13)), atof(*(tokens + 14)));
    simEnvData->deltaRotation = NewQuaterniond(atof(*(tokens + 15)), atof(*(tokens + 16)), atof(*(tokens + 17)), atof(*(tokens + 18)));

    if(printToConsole)
    {
        printf(V3d_ToString(simEnvData->shipAcc, 5));
        printf("\n");
        printf(V3d_ToString(simEnvData->shipVelocity, 5));
        printf("\n");
        printf(V3d_ToString(simEnvData->shipVelocityIncr, 5));
        printf("\n");
        printf(V3d_ToString(simEnvData->shipWorldPos, 5));
        printf("\n");
        printf(Q_ToString(simEnvData->deltaRotation, 5));
        printf("\n\n");
    }
    // Prints the tokens to the console
    /*int i;
    for (i = 0; *(tokens + i); i++)
    {
        printf("data=%s\n", *(tokens + i));
        free(*(tokens + i));
    }
    printf("\n");
    free(tokens);*/
}

char **Split_String(char *stringToParse, char *delimiter, int *nbElemsToExtract)
{
    char** result = 0;
    char *tmp = stringToParse;
    char *rest = NULL;
    char *token;
    char* last_comma = 0; // last delimiter
    //=======
    int nbElements = 0;
    int i;
    int count = strlen(stringToParse)+1;
    char strArr[count];
    for(i = 0; stringToParse[i]; i++)
    {
        strArr[i] = stringToParse[i];
        if(stringToParse[i] == *delimiter)
            nbElements++;
    }
    //=======
    /* Add space for trailing token. */
    nbElements += last_comma < (stringToParse + strlen(stringToParse) - 1);
    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    nbElements++;

    *nbElemsToExtract = nbElements;
    result = malloc(sizeof(char*) * nbElements);
    size_t idx  = 0;
    for(token = strtok_r(strArr, delimiter, &rest); token != NULL; token = strtok_r(NULL, delimiter, &rest))
    {   
        assert(idx < nbElements);
        *(result + idx++) = strdup(token);
    }
    assert(idx == nbElements - 1);
    *(result + idx) = 0;
    //=======
    return result;
}
//=====================================================================
//=====================================================================
//=====================================================================
//=====================================================================
OrbitParams *New_OrbitParams_rarp(double ra, double rp)
{
    OrbitParams *orbit = malloc(sizeof(OrbitParams));
    orbit->ra = ra;
    orbit->rp = rp;
    orbit->e = (ra-rp)/(ra+rp);
    orbit->p = 2*ra*rp/(ra+rp);
    orbit->a = (ra+rp)/2;
    orbit->c = orbit->e*orbit->a;
    orbit->b = sqrt(orbit->p*orbit->a);
    return orbit;
}

OrbitParams *New_OrbitParams_rae(double ra, double e)
{
    OrbitParams *orbit = malloc(sizeof(OrbitParams));
    orbit->ra = ra;
    orbit->e = e;
    orbit->rp = ra*(1-e)/(1+e);
    orbit->p = 2*ra*orbit->rp/(ra+orbit->rp);
    orbit->a = (ra+orbit->rp)/2;
    orbit->c = e*orbit->a;
    orbit->b = sqrt(orbit->p*orbit->a);
    return orbit;
}

OrbitParams *New_OrbitParams_rpe(double rp, double e)
{
    OrbitParams *orbit = malloc(sizeof(OrbitParams));
    orbit->rp = rp;
    orbit->e = e;
    orbit->ra = rp*(e-1)/(1-e);
    orbit->p = 2*orbit->ra*rp/(orbit->ra+rp);
    orbit->a = (orbit->ra+rp)/2;
    orbit->c = e*orbit->a;
    orbit->b = sqrt(orbit->p*orbit->a);
    return orbit;
}

OrbitParams *New_OrbitParams_pe(double p, double e)
{
    OrbitParams *orbit = malloc(sizeof(OrbitParams));
    orbit->p = p;
    orbit->e = e;
    orbit->a = p/(1-pow(e,2));
    orbit->b = sqrt(p*orbit->a);
    orbit->c = orbit->a*e;
    orbit->ra = orbit->a + orbit->c;
    orbit->rp = 2*orbit->a - orbit->ra;
    return orbit;
}

OrbitParams *New_OrbitParams_pe_all(double p, double e, double i, double lAscN, double omega, double trueAnomaly)
{
    OrbitParams *orbit = New_OrbitParams_pe(p, e);
    orbit->i = i;
    orbit->lAscN = lAscN;
    orbit->omega = omega;
    orbit->trueAnomaly = trueAnomaly;
    return orbit;
}

char *OrbitParams_ToString(OrbitShape *orbitShape, int decimalDigits)
{
    int factor = 10 + decimalDigits + 1;
    char *returnText = malloc((7*factor+23)*sizeof(char));
    strcpy(returnText,"");

    char *ra_string = malloc(factor*sizeof(char));
    char *rp_string = malloc(factor*sizeof(char));
    char *e_string = malloc(factor*sizeof(char));
    char *p_string = malloc(factor*sizeof(char));
    char *a_string = malloc(factor*sizeof(char));
    char *b_string = malloc(factor*sizeof(char));
    char *c_string = malloc(factor*sizeof(char));
    sprintf(ra_string,"%.10f",orbitShape->ra);
    sprintf(rp_string,"%.10f",orbitShape->rp);
    sprintf(e_string,"%.10f",orbitShape->e);
    sprintf(p_string,"%.10f",orbitShape->p);
    sprintf(a_string,"%.10f",orbitShape->a);
    sprintf(b_string,"%.10f",orbitShape->b);
    sprintf(c_string,"%.10f",orbitShape->c);

    strcat(returnText, "ra:");
    strcat(returnText, ra_string);
    strcat(returnText, "; rp:");
    strcat(returnText, rp_string);
    strcat(returnText, "; e:");
    strcat(returnText, e_string);
    strcat(returnText, "; p:");
    strcat(returnText, p_string);
    strcat(returnText, "; a:");
    strcat(returnText, a_string);
    strcat(returnText, "; b:");
    strcat(returnText, b_string);
    strcat(returnText, "; c:");
    strcat(returnText, c_string);
    strcat(returnText, "\n");

    return returnText;
}




