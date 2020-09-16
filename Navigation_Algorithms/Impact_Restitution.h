#ifndef IMPACT_RESTITUTION
#define IMPACT_RESTITUTION

typedef struct Vector3d Vector3d;
typedef struct Quaterniond Quaterniond;
typedef struct Orbit_Shape Orbit_Shape;

Orbit_Shape *Orbit_From_RV(Vector3d *radial, Vector3d *velocity, double mu);

#endif