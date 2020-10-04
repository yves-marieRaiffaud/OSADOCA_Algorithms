#ifndef IMPACT_RESTITUTION
#define IMPACT_RESTITUTION

typedef struct Vector3d Vector3d;
typedef struct Quaterniond Quaterniond;
typedef struct OrbitParams OrbitParams;

OrbitParams *Orbit_From_RV(Vector3d *radial, Vector3d *velocity, double mu);
Vector3d *Get_ImpactPoint_in3DWorld(OrbitParams *orbit, Vector3d *impactPoint);
Vector3d *Compute_ImpactPoints_inPlane(OrbitParams *orbit, double planetRadius);

#endif