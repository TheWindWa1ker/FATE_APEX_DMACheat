#define IMGUI_DEFINE_MATH_OPERATORS
#pragma once
#include <iostream>
#include <vector>
#include "Player.hpp"
#include "LocalPlayer.hpp"
#include "Offsets.hpp"
#include "game/Camera.hpp"
#include "GlowMode.hpp"
#include "tools/Spectator.hpp"

#include "Memory/Memory.h"
#include <array>

#include "imgui.h"

#include "ReadList.hpp"

inline ImColor knocked = ImColor(17, 189, 3, 255); // 玩家颜色
inline ImColor whitebar = ImColor(255, 255, 255, 255); // 玩家颜色
inline ImColor bluebar = ImColor(0, 100, 255, 255); // 玩家颜色
inline ImColor purplebar = ImColor(226, 0, 255, 255); // 玩家颜色
inline ImColor redbar = ImColor(255, 0, 0, 255); // 玩家颜色
inline ImColor nonbar = ImColor(255, 165, 0, 255); // 玩家颜色

inline bool Item_Gold = false;
inline bool Item_Red = false;
inline bool Item_Purple = false;
inline bool Item_Blue = false;
inline bool Item_Grey = false;
inline bool Item_Weapons = false;
inline bool Item_Ammo = false;

inline bool GlowESPTeam = false;

const std::vector<uint8_t> ItemHighlightID = { 15, 42, 47, 54, 65, 9, 58 }; // Gold, Red, Purple, Blue, Grey, Weapons, Ammo

struct Sense {
    // Variables
    Camera* GameCamera; // 指向游戏中使用的摄像机对象，用于管理视角和渲染设置。
    LocalPlayer* Myself; // 指向代表游戏中本地玩家的对象，用于获取和设置本地玩家的状态信息。
    std::vector<Player*>* Players; // 存储游戏中所有玩家的指针的VectorQuantity，用于访问和操作游戏中的其他玩家对象。
    int TotalSpectators = 0; // 观察者的总数，用于跟踪当前观察本地玩家的人数。
    std::vector<std::string> Spectators; // 存储当前观察本地玩家的所有观察者的名称。
    uint64_t HighlightSettingsPointer; // 高亮设置的内存地址指针，可能用于修改或读取高亮相关设置。

    // Colors
    float InvisibleGlowColor[3] = { 0, 1, 0 }; // 当玩家不可见时使用的发光颜色，这里是绿色。
    float VisibleGlowColor[3] = { 1, 0, 0 }; // 当玩家可见时使用的发光颜色，这里是红色。

    // Checks
    std::vector<int> HighlightIDInts = { 872415232 }; // 存储用于某些高亮功能检查的整数值，具体用途依赖于游戏逻辑。

    // 构造函数，用于初始化Sense对象时设置玩家列表、游戏摄像机和本地玩家对象的引用。
    Sense(std::vector<Player*>* Players, Camera* GameCamera, LocalPlayer* Myself) {
        this->Players = Players; // 设置玩家列表指针。
        this->GameCamera = GameCamera; // 设置游戏摄像机指针。
        this->Myself = Myself; // 设置本地玩家对象指针。
    }

    float DrawDistance = 300.0f;
    bool enableGlow = false;
    bool checkShield = false;
    bool ItemGlow = false;

    int insidetype = 101;
    int outlinetype = 125;
    int outlinesize = 64;

    // 初始化函数，当前为空，提供了一个可用于将来扩展功能的地方。
    void Initialize() {
        // 目前没有初始化逻辑，但可以在未来添加。
    }
    float HighlightScale = 30;

    float GlowInvisible[3] = {1, 0.1, 0};
    float GlowVisible[4] = {0, 1, 0.2};
    float GlowAdmin[4] = {1, 0, 0};
    float GlowKnocked[4] = {0.1, 1, 0.1};


    void setCustomGlow(Player* Target, int enable, int wall, bool isVisible)
    {
        // 如果目标玩家的发光已被禁用且不可穿墙，且没有高亮ID，则直接返回，不执行任何操作
        if (Target->GlowEnable == 0 && Target->GlowThroughWall == 0 && Target->HighlightID == 0) {
            return;
        }
        uint64_t basePointer = Target->BasePointer; // 获取目标玩家的基础指针

        int settingIndex = 0; // 默认设置索引

        std::array<float, 3> glowColorRGB = { 0, 0, 0 }; // 默认发光颜色为黑色

        if (Target->IsKnocked) {
            settingIndex = 61;
            glowColorRGB = {GlowKnocked[0] * HighlightScale,GlowKnocked[1] * HighlightScale,GlowKnocked[2] * HighlightScale};
        }
        else if (Target->IsVisible) {

            settingIndex = 62;
            glowColorRGB = {GlowVisible[0] * HighlightScale,GlowVisible[1] * HighlightScale,GlowVisible[2] * HighlightScale};
        }
        else {
            if (checkShield)
            {
                if (Target->shield + Target->health <= 100) { // Orange
                    settingIndex = 63;
                    glowColorRGB = { nonbar.Value.x, nonbar.Value.y, nonbar.Value.z };
                }
                else if (Target->shield + Target->health <= 150) { // white
                    settingIndex = 64;
                    glowColorRGB = { whitebar.Value.x, whitebar.Value.y, whitebar.Value.z };
                }
                else if (Target->shield + Target->health <= 175) { // blue
                    settingIndex = 66;
                    glowColorRGB = { bluebar.Value.x, bluebar.Value.y, bluebar.Value.z };
                }
                else if (Target->shield + Target->health <= 200) { // purple
                    settingIndex = 67;
                    glowColorRGB = { purplebar.Value.x, purplebar.Value.y, purplebar.Value.z };
                }
                else if (Target->shield + Target->health <= 225) { // red
                    settingIndex = 68;
                    glowColorRGB = { redbar.Value.x, redbar.Value.y, redbar.Value.z };
                }
                else {
                    settingIndex = 69;
                    glowColorRGB = { 255.0 / 2, 255.0 / 2, 255.0 / 2 };
                }
            }
            else
            {
                settingIndex = 69;
                glowColorRGB = {GlowInvisible[0] * HighlightScale, GlowInvisible[1] * HighlightScale, GlowInvisible[2] * HighlightScale};
            }
        }

        if (RList.IsAdmin(Target->UserId))
        {
            settingIndex = 60;
            glowColorRGB = {GlowAdmin[0] * HighlightScale,GlowAdmin[1] * HighlightScale,GlowAdmin[2] * HighlightScale};
        }

        // 如果目标玩家当前的发光启用状态与参数不同，更新发光启用状态
        if (Target->GlowEnable != enable) {
            uint64_t glowEnableAddress = basePointer + OFF_GLOW_ENABLE;
            mem.Write<int>(glowEnableAddress, enable);
        }

        // 如果目标玩家当前的穿墙发光状态与参数不同，更新穿墙发光状态
        if (Target->GlowThroughWall != wall) {
            uint64_t glowThroughWallAddress = basePointer + OFF_GLOW_THROUGH_WALL;
            mem.Write<int>(glowThroughWallAddress, wall);
        }

        // 更新目标玩家的高亮ID
        uint64_t highlightIdAddress = basePointer + OFF_GLOW_HIGHLIGHT_ID;
        mem.Write<int>(highlightIdAddress, int(settingIndex));

        // 创建Scatter写入句柄
        auto handle = mem.CreateScatterHandle();

        // 设置高亮功能的配置位
        std::array<unsigned char, 4> highlightFunctionBits = {
                insidetype,   // 内部功能配置
                outlinetype, // 轮廓功能配置
                outlinesize,  // 轮廓半径配置
                64   // 可见性与其他状态配置
        };

        mem.AddScatterWriteRequest(handle, HighlightSettingsPointer + OFF_GLOW_HIGHLIGHT_TYPE_SIZE * settingIndex + 0x0, &highlightFunctionBits, sizeof(highlightFunctionBits));
        mem.AddScatterWriteRequest(handle, HighlightSettingsPointer + OFF_GLOW_HIGHLIGHT_TYPE_SIZE * settingIndex + 0x4, &glowColorRGB, sizeof(glowColorRGB));

        mem.ExecuteWriteScatter(handle);
        mem.CloseScatterHandle(handle);
    }

    Vector2D DummyVector = { 0, 0 }; // 用作临时存储转换后的屏幕坐标

    void Update() {
        // 如果本地玩家已经死亡，不执行任何操作
        if (Myself->IsDead) return;

        if (ItemGlow) {
            uint64_t highlightSettingsPtr = HighlightSettingsPointer;
            if (mem.IsValidPointer(highlightSettingsPtr)) {
                uint64_t highlightSize = OFF_GLOW_HIGHLIGHT_TYPE_SIZE;
                const GlowMode newGlowMode = { 137,138,35,127 };
//                for (int highlightId = MinimumItemRarity; highlightId < 60; highlightId++)
//                {
//                    const GlowMode oldGlowMode = mem.Read<GlowMode>(highlightSettingsPtr + (highlightSize * highlightId) + 0, true);
//                    if (newGlowMode != oldGlowMode) {
//                        mem.Write<GlowMode>(highlightSettingsPtr + (highlightSize * highlightId) + 0, newGlowMode);
//                    }
//                }
                const std::vector<bool> itemFlags = { Item_Gold, Item_Red, Item_Purple, Item_Blue, Item_Grey, Item_Weapons, Item_Ammo };

                for (size_t i = 0; i < ItemHighlightID.size(); ++i)
                {
                    if (itemFlags[i])
                    {
                        uint8_t highlightId = ItemHighlightID[i];
                        const GlowMode oldGlowMode = mem.Read<GlowMode>(highlightSettingsPtr + (highlightSize * highlightId) + 0, true);
                        if (newGlowMode != oldGlowMode)
                        {
                            mem.Write<GlowMode>(highlightSettingsPtr + (highlightSize * highlightId) + 0, newGlowMode);
                        }
                    }
                }
            }

        }
        // 遍历所有玩家对象
        for (int i = 0; i < Players->size(); i++) {
            Player* Target = Players->at(i); // 获取当前遍历到的玩家对象

            // 如果目标玩家无效、是虚拟目标、是本地玩家或不是敌对玩家，则跳过当前循环
            if (!Target->IsValid()) continue;
            if (Target->IsDummy()) continue;
            if (Target->IsLocal) continue;
            if (!Target->IsHostile && !GlowESPTeam) continue;

            // 尝试将目标玩家的位置（Z轴上调整了30单位，可能是为了更好地在屏幕上显示）转换为屏幕coord
            if (GameCamera->WorldToScreen(Target->LocalOrigin.ModifyZ(30), DummyVector)) {
                // 如果转换成功，对目标玩家应用自定义发光效果
                // 这里发光启用，穿墙发光启用，根据玩家是否可见设置发光颜色
                if (enableGlow)
                {
                    setCustomGlow(Target, 1, 1, Target->IsVisible);
                }
            }
        }
    }
};