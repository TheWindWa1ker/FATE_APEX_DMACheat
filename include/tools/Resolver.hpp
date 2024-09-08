#pragma once
#include <cmath>
#include "Vector3D.hpp"
#include "tools/QAngle.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif


class Resolver {
public:
    static QAngle CalculateAngle(Vector3D from, Vector3D to) {
        Vector3D newDirection = to.Subtract(from);
        newDirection.Normalize();
        
        float pitch = -asinf(newDirection.z) * (180 / M_PI);
        float yaw = atan2f(newDirection.y, newDirection.x) * (180 / M_PI);

        return QAngle(pitch, yaw);
    }

    static double solveTimeOfFlight(double v, double g, Vector3D v_t,
                                    Vector3D s_t,
                                    Vector3D s_w,
                                    Vector3D v_w,
                                    double initial_guess = 0.01, double tolerance = 1e-3, int max_iterations = 1000)
    {
        auto equation = [&](double t)
        {
            return
                    std::pow(v_t.x - v_w.x + (s_t.x - s_w.x) / t, 2) +
                    std::pow(v_t.y - v_w.y + (s_t.y - s_w.y) / t, 2) +
                    std::pow(0.5 * g * t + v_t.z - v_w.z + (s_t.z - s_w.z) / t, 2) - std::pow(v, 2);
        };

        auto derivative = [&](double t)
        {
            double term1 = v_t.x - v_w.x + (s_t.x - s_w.x) / t;
            double term2 = v_t.y - v_w.y + (s_t.y - s_w.y) / t;
            double term3 = 0.5 * g * t + v_t.z - v_w.z + (s_t.z - s_w.z) / t;
            double term1_derivative = -1.0 * (s_t.x - s_w.x) / std::pow(t, 2);
            double term2_derivative = -1.0 * (s_t.y - s_w.y) / std::pow(t, 2);
            double term3_derivative = 0.5 * g - 1.0 * (s_t.z - s_w.z) / std::pow(t, 2);
            return 2.0 * (term1 * term1_derivative + term2 * term2_derivative + term3 * term3_derivative);
        };

        // Print z of the target position
        //std::cout << "Target z: " << s_t.z << std::endl;

        // Print velocity of target
        //std::cout << "Target velocity: " << v_t.Magnitude() << std::endl;

        double t = initial_guess;
        for (int i = 0; i < max_iterations; ++i)
        {
            double f_value = equation(t);
            double f_derivative_value = derivative(t);

            if (std::abs(f_value) < tolerance)
            {
                break;
            }
            double new_t = t - f_value / f_derivative_value;

            if (new_t < 0.0)
            {
                new_t = t / 2.0;
            }
            t = new_t;
        }
        if (t < 0.0 || t > 2.5)
            t = 0.01;
        return t;
    }

    static Vector3D GetTargetPosition(const Vector3D& targetPosition, Vector3D targetVelocity, float time) {
        return targetPosition.Add((targetVelocity.Multiply(time)));
    }

    static float GetTimeToTarget(Vector3D startPosition, Vector3D endPosition, float bulletSpeed) {
        float distance = (endPosition.Subtract(startPosition)).Magnitude();
        return distance / bulletSpeed;
    }

    static float GetBasicBulletDrop(Vector3D startPosition, Vector3D endPosition, float bulletSpeed, float bulletDropRate) {
        float time = GetTimeToTarget(startPosition, endPosition, bulletSpeed);
        bulletDropRate *= 500.0f;
        return 0.5f * bulletDropRate * time * time;
    }

    static Vector3D GetTargetPosition(Vector3D startPosition, Vector3D endPosition, Vector3D targetVelocity, float bulletSpeed) {
        float time = GetTimeToTarget(startPosition, endPosition, bulletSpeed);
        return GetTargetPosition(endPosition, targetVelocity, time);
    }

    static float GetTimeToTarget(const Vector3D start, const Vector3D end, const float speed, const float gravity) {
        float horizontalDistance = start.Distance2D(end);
        float heightDifference = end.z - start.z;
        return (speed * sqrtf(2 * sinf(M_PI / 4) / gravity)) + (sqrtf(2 * sinf(M_PI / 4) * (sinf(M_PI / 4) - 2 * heightDifference) / gravity));
    }
};
