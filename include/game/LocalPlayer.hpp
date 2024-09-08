#pragma once
#include <iostream>
#include "Offsets.hpp"
#include "Memory/Memory.h"
#include "tools/Vector2D.hpp"
#include "tools/Vector3D.hpp"
#include <chrono>
#include <thread>

struct LocalPlayer {
    uint64_t BasePointer;

    uint64_t WeaponEntity;

    int OffHandWeaponID;
    int Flags;

    bool IsDead;
    bool IsInAttack;
    bool IsKnocked;
    bool IsZooming;

    bool continueSglideLoop; // 控制循环是否继续的标志

    int Team;
    int Squad;
    Vector3D LocalOrigin;
    Vector3D AbsoluteVelocity;
    Vector3D CameraPosition;
    Vector3D view_offset;

    std::string PlayerInfo = "";
    Vector2D ViewAngles;
    Vector2D PunchAngles;
    float ViewYaw;

    int isGrppleActived = 0;
    int isGRAPPLE = 0;

    int weapon_mod_bitfield;
    int WeaponIndex;
    float WeaponProjectileSpeed;
    float WeaponProjectileScale;
    bool IsHoldingGrenade;
    bool IsReloading;
    int Ammo;

    float ZoomFOV;
    float TargetZoomFOV;

    float TimeBase;
    float TraversalTimeStart;
    float TraversalTimeProgress;

    void ResetPointer() {
        BasePointer = 0;
    }

    bool ValidPosition() {
		if (LocalOrigin.x != 0.0f && LocalOrigin.y != 0.0f && LocalOrigin.z != 0.0f) {
            return true;
		} else {
			return false;
		}
	}

    void Read() {
        if (!mem.IsValidPointer(BasePointer)) return;

        auto handle = mem.CreateScatterHandle();


        mem.AddScatterReadRequest(handle, BasePointer + OFFSET_VIEW_OFFSET, &view_offset, sizeof(Vector3D));

        // 是否死亡
        uint64_t isDeadAddress = BasePointer + OFF_LIFE_STATE;
        mem.AddScatterReadRequest(handle, isDeadAddress, &IsDead, sizeof(bool));

        // 是否被击倒
        uint64_t isKnockedAddress = BasePointer + OFF_BLEEDOUT_STATE;
        mem.AddScatterReadRequest(handle, isKnockedAddress, &IsKnocked, sizeof(bool));

        // Scatter read request for IsZooming
        uint64_t isZoomingAddress = BasePointer + OFF_ZOOMING;
        mem.AddScatterReadRequest(handle, isZoomingAddress, &IsZooming, sizeof(bool));

        // 队标
        uint64_t teamAddress = BasePointer + OFF_TEAM_NUMBER;
		mem.AddScatterReadRequest(handle, teamAddress, &Team, sizeof(int));

        uint64_t squadAddress = BasePointer + OFF_SQUAD_ID;
        mem.AddScatterReadRequest(handle, squadAddress, &Squad, sizeof(int));

        // 坐标
        uint64_t localOriginAddress = BasePointer + OFF_LOCAL_ORIGIN;
		mem.AddScatterReadRequest(handle, localOriginAddress, &LocalOrigin, sizeof(Vector3D));

        // 相机指针
        uint64_t cameraPositionAddress = BasePointer + OFF_CAMERAORIGIN;
		mem.AddScatterReadRequest(handle, cameraPositionAddress, &CameraPosition, sizeof(Vector3D));

        // 视角朝向（应该
        uint64_t viewAnglesAddress = BasePointer + OFF_VIEW_ANGLES;
		mem.AddScatterReadRequest(handle, viewAnglesAddress, &ViewAngles, sizeof(Vector2D));

        // ViewYaw
        uint64_t viewYawAddress = BasePointer + OFF_YAW;
        mem.AddScatterReadRequest(handle, viewYawAddress, &ViewYaw, sizeof(float));

        // 武器句柄
        uint64_t WeaponHandle;
        uint64_t weaponHandleAddress = BasePointer + OFFSET_ACTIVE_WEAPON;
		mem.AddScatterReadRequest(handle, weaponHandleAddress, &WeaponHandle, sizeof(uint64_t));

        // 武器id
        uint64_t offhandWeaponIDAddress = BasePointer + OFF_OFFHAND_WEAPON;
		mem.AddScatterReadRequest(handle, offhandWeaponIDAddress, &OffHandWeaponID, sizeof(int));

        // 滞空状态
        uint64_t FlagsAddress = BasePointer + OFF_OFFHAND_WEAPON;
        mem.AddScatterReadRequest(handle, FlagsAddress, &Flags, sizeof(int));

        // Execute the scatter read
        mem.ExecuteReadScatter(handle);

        // Close the scatter handle
        mem.CloseScatterHandle(handle);
        //如果没有死亡 并且没有被击倒 并且武器句柄存在
        if (!IsDead && !IsKnocked && WeaponHandle) {
            uint64_t WeaponHandleMasked = WeaponHandle & 0xffff;
            WeaponEntity = mem.Read<uint64_t>(mem.OFF_BASE + OFF_ENTITY_LIST + (WeaponHandleMasked << 5), true);

            //std::cout << OffHandWeaponID << std::endl;
            if (OffHandWeaponID == -251 || OffHandWeaponID == -63483) {
                IsHoldingGrenade = true;
            } else {
                IsHoldingGrenade = false;
            }

            auto handle = mem.CreateScatterHandle();

            // FOV缩放
            uint64_t zoomFOVAddress = WeaponEntity + OFF_CURRENTZOOMFOV;
            mem.AddScatterReadRequest(handle, zoomFOVAddress, &ZoomFOV, sizeof(float));

            // 目标FOV缩放
            uint64_t targetZoomFOVAddress = WeaponEntity + OFF_TARGETZOOMFOV;
            mem.AddScatterReadRequest(handle, targetZoomFOVAddress, &TargetZoomFOV, sizeof(float));

            // 武器索引
            uint64_t weaponIndexAddress = WeaponEntity + OFF_WEAPON_INDEX;
            mem.AddScatterReadRequest(handle, weaponIndexAddress, &WeaponIndex, sizeof(int));

            uint64_t mod_bitfieldAddress = WeaponEntity + OFFSET_WEAPON_BITFIELD;
            mem.AddScatterReadRequest(handle, mod_bitfieldAddress, &weapon_mod_bitfield, sizeof(int));

            // 武器射击速度
            uint64_t weaponProjectileSpeedAddress = WeaponEntity + OFF_PROJECTILESPEED;
            mem.AddScatterReadRequest(handle, weaponProjectileSpeedAddress, &WeaponProjectileSpeed, sizeof(float));

            // 子弹下坠
            uint64_t weaponProjectileScaleAddress = WeaponEntity + OFF_PROJECTILESCALE;
            mem.AddScatterReadRequest(handle, weaponProjectileScaleAddress, &WeaponProjectileScale, sizeof(float));

            //绝对速度
            uint64_t playerAbsoluteVelocityAddress = BasePointer + OFF_ABSVELOCITY;
            mem.AddScatterReadRequest(handle, playerAbsoluteVelocityAddress, &AbsoluteVelocity, sizeof(Vector3D));

            // 武器弹药
            uint64_t weaponAmmoAddress = WeaponEntity + OFF_WEAPON_AMMO;
            mem.AddScatterReadRequest(handle, weaponAmmoAddress, &Ammo, sizeof(int));

            // 重新加载
            uint64_t isReloadingAddress = WeaponEntity + OFF_RELOADING;
            mem.AddScatterReadRequest(handle, isReloadingAddress, &IsReloading, sizeof(bool));

            // Execute the scatter read
            mem.ExecuteReadScatter(handle);
            //std::cout << "WeaponID:" << WeaponIndex << std::endl;

            // Close the scatter handle
            mem.CloseScatterHandle(handle);
        }
    }

    bool IsValid() {
        return mem.IsValidPointer(BasePointer);
    }

    bool IsCombatReady() {
        if (!mem.IsValidPointer(BasePointer)) return false;
        if (IsDead) return false;
        if (IsKnocked) return false;
        return true;
    }
};