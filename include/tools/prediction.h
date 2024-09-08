#include "Matrix.hpp"

#define Assert( _exp ) ((void)0)

#define FastSqrt(x)			(sqrt)(x)

#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h

#define M_PI_F		((float)(M_PI))	// Shouldn't collide with anything.
#ifndef RAD2DEG
#define RAD2DEG( x  )  ( (float)(x) * (float)(180.f / M_PI_F) )
#endif

#ifndef DEG2RAD
#define DEG2RAD( x  )  ( (float)(x) * (float)(M_PI_F / 180.f) )
#endif

struct PredictCtx {
    Vector3D StartPos;
    Vector3D TargetPos;
    Vector3D TargetVel;
    float BulletSpeed;
    float BulletGravity;

    Vector2D AimAngles;
};

inline Vector3D ExtrapolatePos(const PredictCtx &Ctx, float Time) {
    return Ctx.TargetPos + (Ctx.TargetVel * Time);
}

inline bool OptimalPitch(const PredictCtx &Ctx, const Vector2D &Dir2D,
                         float *OutPitch) {
    float Vel = Ctx.BulletSpeed, Grav = Ctx.BulletGravity, DirX = Dir2D.x,
            DirY = Dir2D.y;
    float Root = Vel * Vel * Vel * Vel -
                 Grav * (Grav * DirX * DirX + 2.f * DirY * Vel * Vel);
    if (Root >= 0.f) {
        *OutPitch = atanf((Vel * Vel - sqrt(Root)) / (Grav * DirX));
        return true;
    }
    return false;
}

inline bool SolveTrajectory(PredictCtx &Ctx, const Vector3D &ExtrPos,
                            float *TravelTime) {
    Vector3D Dir = ExtrPos - Ctx.StartPos;
    Vector2D Dir2D = {sqrtf(Dir.x * Dir.x + Dir.y * Dir.y), Dir.z};

    float CurPitch;
    if (!OptimalPitch(Ctx, Dir2D, &CurPitch)) {
        return false;
    }

    *TravelTime = Dir2D.x / (cosf(CurPitch) * Ctx.BulletSpeed);
    Ctx.AimAngles.y = atan2f(Dir.y, Dir.x);
    Ctx.AimAngles.x = CurPitch;
    return true;
}

inline bool BulletPredict(PredictCtx &Ctx) {
    float MAX_TIME = 1.f, TIME_STEP = (1.f / 256.f);
    for (float CurrentTime = 0.f; CurrentTime <= MAX_TIME;
         CurrentTime += TIME_STEP) {
        float TravelTime;
        Vector3D ExtrPos = ExtrapolatePos(Ctx, CurrentTime);
        if (!SolveTrajectory(Ctx, ExtrPos, &TravelTime)) {
            return false;
        }

        if (TravelTime < CurrentTime) {
            Ctx.AimAngles = {-RAD2DEG(Ctx.AimAngles.x), RAD2DEG(Ctx.AimAngles.y)};
            return true;
        }
    }
    return false;
}