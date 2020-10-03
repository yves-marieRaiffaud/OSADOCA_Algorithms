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
    double omega;
    if(denom == 0)
        omega = 0;
    else
        omega = acos(V3d_Dot(ascNVec, eVec)/denom) * rad2deg;

    // nu, true Anomaly
    double nu = acos(V3d_Dot(eVec, radial)/(V3d_Magnitude(eVec)*V3d_Magnitude(radial))) * rad2deg;
    if(V3d_Dot(radial, velocity) > 0)
        nu = ClampAngle_Deg(0, 180, nu);
    else
        nu = ClampAngle_Deg(180, 360, nu);

    return New_OrbitParams_pe_all(p, e, i, lAscN, omega, nu);
}

Vector3d *Compute_ImpactPoints_inPlane(OrbitParams *orbit, double planetRadius)
{
    printf("i = %.5f ; lAscN = %.5f ; omega = %.5f\n", orbit->i, orbit->lAscN, orbit->omega);

    double a = orbit->a;
    double b = orbit->b;
    double r = planetRadius;
    // Offsetting the ellipse orbit by c along the x-axis so that the planet origin and one of the two foci point are the same
    double c = orbit->c;
    // Solving the ellipse-circle intersection in the XY plane
    double b2 = b*b;
    double b4 = pow(b,4);
    double a2 = a*a;
    double r2 = r*r;
    double c2 = c*c;

    double x = (-c*b2+sqrt(-a2*(a2*b2-a2*r2-b4-b2*c2+b2*r2)))/(a2-b2);
    double y = sqrt(r2-x*x);

    return NewVector3d(x, y, 0);
}

Vector3d *ComputeImpactPoints(OrbitParams *orbit, Vector3d *impactPoint)
{
    // By definition, the +X-axis will represent the inclination rotation
    // Without any rotation, the apogee line is along the +X-axis
    Vector3d *apogeeLineDir = V3d_Right();
    Vector3d *ascendingNodeLineDir = V3d_Forward();
    Quaterniond *iRotQuat = Q_AngleAxis(orbit->i, ascendingNodeLineDir, true);
    Vector3d *rotatedApogeeDir = Q_RotateVec(iRotQuat, apogeeLineDir);

    Vector3d *normalUp = V3d_Cross(ascendingNodeLineDir, rotatedApogeeDir);
    Quaterniond *perihelionArgRot = Q_AngleAxis(orbit->omega, normalUp, true);
    Quaterniond *lAscN_Rot = Q_AngleAxis(orbit->lAscN, V3d_Up(), true);
    Quaterniond *rot = Q_QuatMultiply(lAscN_Rot, Q_QuatMultiply(perihelionArgRot, iRotQuat));

    impactPoint = Q_RotateVec(rot, impactPoint);
    return impactPoint;
}