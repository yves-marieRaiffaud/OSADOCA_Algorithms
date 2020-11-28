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
    // Computing the intersection between the elliptical orbit 'orbit' whose focal is at the origin of the frame,
    // and with the Earth's surface represented by a circle of radius rEarth and positioned at the origin of the frame
    // Both the circle and the ellipsis are in the same plane: plane (XY): +X horizontal right vec & +Y vertical positive
    // The semi-major axis of the ellipsis is along the Y-axis, with its periapsis towards +Y, and its apoapsis towards -Y
    // The intersection is found solving analytically the following system:
    // (Equation.1) ==> (x^2/a^2) + (y+c)^2/b^2 = 1     : Ellipsis of the orbit positioned at (0,-c) in the frame
    // (Equation.2) ==>  x^2 + y^2 = rEarth^2           : Circle representing Earth's surface positioned at (0,0) in the frame

    double a = orbit->a; // Semi-Major axis in meters
    double b = orbit->b; // Semi-Minor axis in meters
    double r = planetRadius; // Earth radius in meters
    double c = orbit->c; // Distance ellipsis center-focal
    double a2 = a*a;
    double a4 = a2*a2;
    double b2 = b*b;
    double c2 = c*c;
    double r2 = r*r;

    double num = a4*(-(b2+c2-r2))+a2*b2*(b2-c2-r2) + 2*sqrt(a4*b2*c2*(a4-a2*(b2-c2+r2)+b2*r2));
    double denom = pow(a2-b2, 2);
    double x = sqrt(num/denom);

    num = sqrt(a4*b2*c2*(a4-a2*(b2-c2+r2)+b2*r2)) - a4*c2;
    denom = a2*c*(a2-b2);
    double y = -num/denom;
    // Solution in meters
    return NewVector3d(x, y, 0);
}

Vector3d *Get_ImpactPoint_in3DWorld(OrbitParams *orbit, Vector3d *impactPoint)
{
    // By default and to be coherent with Unity X-Y-Z axis, an impact point calculated with the method 'Compute_ImpactPoints_inPlane'  will be from the plane (XY) in the 2D method
    // to the (XZ) plane (with orbit's inclination i == 0)
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
    // Impact point in meters
    return impactPoint;
}