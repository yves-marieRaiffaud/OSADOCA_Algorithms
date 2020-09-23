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
#include "Impact_Restitution.h"
#include "OSADOCA_Structs.h"

typedef struct Vector3d Vector3d;
typedef struct Quaterniond Quaterniond;
typedef struct OrbitParams OrbitParams;

// Estimate orbital shape parameters from the spacecraft's radial vector, velocity vector and the µ of the orbited planet
// Input: radial vector in meters, velocity vector in m/s, mu in m3.s^-2
OrbitParams *Orbit_From_RV(Vector3d *radial, Vector3d *velocity, double mu)
{
    double velNorm = V3d_Magnitude(velocity);
    double rNorm = V3d_Magnitude(radial);

    // Angular momentum
    Vector3d *h = V3d_Cross(radial, velocity);
    // Eccentricity vector, no dimension
    Vector3d *eVec = V3d_Substract(V3d_Divide_S(V3d_Cross(velocity,h), mu), V3d_Divide_S(radial, rNorm));
    double e = V3d_Magnitude(eVec);

    // Semi-major axis a in meters, and Parameter of the conic p
    double a, p;
    if(e == 1) {
        printf("Orbit is parabolic\n");
        a = INFINITY;
        p = V3d_SqrMagnitude(h)/mu;
    }
    else if(e < 1) {
        a = rNorm*mu / (2*mu - rNorm*pow(velNorm,2));
        p = a*(1-pow(e,2));
    }

    // Ascending node vector
    Vector3d *ascNVec = V3d_Cross(V3d_Up(), h);

    // Inclination
    double i = acos(h->y/V3d_Magnitude(h)) * rad2deg;
    
    // Longitude of the ascending node
    double lAscN = acos(ascNVec->x/V3d_Magnitude(ascNVec)) * rad2deg;
    if(ascNVec->z >= 0)
        lAscN = ClampAngle_Deg(0, 180, lAscN);
    else
        lAscN = ClampAngle_Deg(180, 360, lAscN);

    // Argument of the perihelion, omega
    double denom = V3d_Magnitude(ascNVec)*V3d_Magnitude(eVec);
    double omega = acos(V3d_Dot(ascNVec, eVec)/denom) * rad2deg;

    // nu, true Anomaly
    double nu = acos(V3d_Dot(eVec, radial)/(V3d_Magnitude(eVec)*V3d_Magnitude(radial))) * rad2deg;
    if(V3d_Dot(radial, velocity) > 0)
        nu = ClampAngle_Deg(0, 180, nu);
    else
        nu = ClampAngle_Deg(180, 360, nu);

    return New_OrbitParams_pe_all(p, e, i, lAscN, omega, nu);
}

Vector3d *ComputeImpactPoints(OrbitParams *orbit, double planetRadius)
{
    Vector3d *impactPoints = malloc(sizeof(Vector3d)*4);
    double a = orbit->a *0.001;
    double b = orbit->b *0.001;

    double r = planetRadius *0.001;

    // Starting with calculations in the plane X-Y
    double insideRoot = (r*r-a*a)*b*b/(b*b-a*a);
    double rraa = r*r-a*a;
    double bbaa = b*b-a*a;
    printf("r*r-a*a    = %.5f\n", rraa);
    printf("b*b-a*a    = %.5f\n", bbaa);
    printf("insideRoot = %.5f\n", insideRoot);
    printf("r*r =        %.5f\n", r*r);
    if(insideRoot == 0) {
        // y equal 0, thus solving x*x + 0*0 = r^2
        impactPoints[0] = *NewVector3d(sqrt(r), 0, 0);
        impactPoints[1] = *NewVector3d(-sqrt(r), 0, 0);
        impactPoints[2] = *NewVector3d(0, 0, 0);
        impactPoints[3] = *NewVector3d(0, 0, 0);
    }
    else {
        double sqrIn = sqrt(insideRoot);
        impactPoints[0] = *NewVector3d(sqrt(r*r-insideRoot), sqrIn, 0);
        impactPoints[1] = *NewVector3d(-sqrt(r*r-insideRoot), -sqrIn, 0);
        impactPoints[2] = *NewVector3d(-sqrt(r*r-insideRoot), sqrIn, 0);
        impactPoints[3] = *NewVector3d(sqrt(r*r-insideRoot), -sqrIn, 0);
    }
    printf("impactPoint[0]: x = %.5f; y = %.5f; z = %.5f\n", impactPoints[0].x, impactPoints[0].y, impactPoints[0].z);
    printf("impactPoint[1]: x = %.5f; y = %.5f; z = %.5f\n", impactPoints[1].x, impactPoints[1].y, impactPoints[1].z);
    printf("impactPoint[2]: x = %.5f; y = %.5f; z = %.5f\n", impactPoints[2].x, impactPoints[2].y, impactPoints[2].z);
    printf("impactPoint[3]: x = %.5f; y = %.5f; z = %.5f\n", impactPoints[3].x, impactPoints[3].y, impactPoints[3].z);
    return impactPoints;
}

