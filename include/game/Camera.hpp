#pragma once
#include "Offsets.hpp"
#include "LocalPlayer.hpp"
#include "Player.hpp"
#include "Offsets.hpp"
#include "Memory/Memory.h"
#include "tools/Vector4D.hpp"
#include "tools/Vector3D.hpp"
#include "tools/Vector2D.hpp"
#include "Matrix.hpp"

struct Camera {
    Vector3D LocalCamera;
    float ScreenSizeX, ScreenSizeY;
    Vector2D ScreenSize = Vector2D(ScreenSizeX, ScreenSizeY);
    Vector2D ScreenCenter = Vector2D(ScreenSize.x / 2, ScreenSize.y / 2);
    float FOV;
    ViewMatrix GameViewMatrix;
    uint64_t RenderPointer;

    void Initialize() {
        ScreenSize = Vector2D(ScreenSizeX, ScreenSizeY);
        ScreenCenter = Vector2D(ScreenSize.x / 2, ScreenSize.y / 2);
    }

    const Vector2D& GetResolution() {
        return ScreenSize;
    }

    const Vector2D& GetCenter() {
		return ScreenCenter;
	}

    void Update() {

        if (ScreenCenter.x / 2 != ScreenSize.x / 2 || ScreenCenter.x / 2 != ScreenSize.y / 2)
        {
            ScreenCenter = Vector2D(ScreenSize.x / 2, ScreenSize.y / 2);
        }

        uint64_t MatrixPtr = mem.Read<uint64_t>(RenderPointer + OFF_VIEWMATRIX, true);
        GameViewMatrix = mem.Read<ViewMatrix>(MatrixPtr);
    }

    bool WorldToScreen(Vector3D WorldPosition, Vector2D& ScreenPosition) const {
        Vector3D transformed = GameViewMatrix.Transform(WorldPosition);
        if (transformed.z < 0.001) {
            return false;
        }

        transformed.x *= 1.0 / transformed.z;
        transformed.y *= 1.0 / transformed.z;

        if (transformed.x < -1.05f || transformed.x > 1.05f || transformed.y < -1.05f || transformed.y > 1.05f) {
			return false;
		}

        ScreenPosition = Vector2D(ScreenSize.x / 2.0f + transformed.x * (ScreenSize.x / 2.0f), ScreenSize.y / 2.0f - transformed.y * (ScreenSize.y / 2.0f));
        return true;
    }

    bool WorldToScreen4D(Vector3D WorldPosition, Vector4D& ScreenPosition) {

        ScreenPosition = GameViewMatrix.Transform4D(WorldPosition, ScreenCenter.x, ScreenCenter.y);
        if (ScreenPosition.w < 0.001)
        {
            return false;
        }
        return true;
    }
};