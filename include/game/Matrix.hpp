#pragma once
#include "tools/Vector2D.hpp"
#include "tools/Vector3D.hpp"
#include "tools/Vector4D.hpp"

struct Matrix3x4 {
public:
	float matrix[3][4];

	Vector3D GetPosition() const {
		return Vector3D(matrix[0][3], matrix[1][3], matrix[2][3]);
	}

	Vector3D GetPosition2() const {
		return Vector3D(matrix[0][3], matrix[1][3], matrix[2][3]);
	}
};

struct ViewMatrix {
public:
	float matrix[4][4];

	Vector3D Transform(const Vector3D vector) const {
		Vector3D transformed;

		transformed.x = vector.y * matrix[0][1] + vector.x * matrix[0][0] + vector.z * matrix[0][2] + matrix[0][3];
	    transformed.y = vector.y * matrix[1][1] + vector.x * matrix[1][0] + vector.z * matrix[1][2] + matrix[1][3];
	    transformed.z = vector.y * matrix[3][1] + vector.x * matrix[3][0] + vector.z * matrix[3][2] + matrix[3][3];

		return transformed;
	}

    Vector4D Transform4D(const Vector3D vector,int px,int py) const {

        Vector4D Box; // 存储变换后的VectorQuantity。
        float camera = matrix[3][0] * vector.x + matrix[3][1] * vector.y + matrix[3][2] * vector.z + matrix[3][3];

        float r_x = px + (matrix[0][0] * vector.x + matrix[0][1] * vector.y + matrix[0][2] * vector.z + matrix[0][3]) / camera * px;
        float r_y = py - (matrix[1][0] * vector.x + matrix[1][1] * vector.y + matrix[1][2] * (vector.z + 80) + matrix[1][3]) / camera * py;
        float r_w = py - (matrix[1][0] * vector.x + matrix[1][1] * vector.y + matrix[1][2] * (vector.z + 160) + matrix[1][3]) / camera * py;

        Box.x = (r_x - (r_y - r_w) / 4);
        Box.y = r_y;
        Box.w = (r_y - r_w) / 2;
        Box.z = r_y - r_w;

        return Box;
    }
};