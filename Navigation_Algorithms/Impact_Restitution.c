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
#include "Impact_Restitution.h"
#include "OSADOCA_Structs.h"

typedef struct Vector3d Vector3d;
typedef struct Quaterniond Quaterniond;
typedef struct Orbit_Shape Orbit_Shape;

Orbit_Shape *Orbit_From_RV(Vector3d *radial, Vector3d *velocity, double mu)
{
    double velNorm = V3d_Magnitude(velocity);
    double rNorm = V3d_Magnitude(radial);

    Vector3d aNum = V3d_Multiply_S(mu, radial);
    double a = aNum/(2*mu - rNorm*pow(velNorm,2));
    Vector3d h = Cross(radial, velocity);

    Vector3d eVec = Cross(velocity,h)/mu - radial/rNorm;
    double e = V3d_Magnitude(eVec);

    double p = a*(1-pow(e,2));

    return New_OrbitShape_pe(p, e);
}
