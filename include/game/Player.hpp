#pragma once
#include <string>
#include "Offsets.hpp"
#include "LocalPlayer.hpp"
#include "Level.hpp"
#include "Memory/Memory.h"
#include "kmbox/HitboxType.hpp"
#include "tools/Vector2D.hpp"
#include "tools/Vector3D.hpp"
#include "Matrix.hpp"

#include "ReadList.hpp"

#include <fstream>
#include <unordered_set>

inline bool textspector = false;
inline bool guispector = true;

struct Player {
    LocalPlayer* Myself;
    Level* Map;

    int Index;

    int m_xp;

    uint64_t BasePointer = 0;

    uint64_t ModelPointer = 0;

    uint64_t BonePointer = 0;

    uint64_t UidPointer = 0;

    uint64_t NameList = 0;

    char* PlayerName = new char[32 + 1];
    char NameBuffer[8] = { 0 };
    std::string Name;
    int Team;

    int GlowEnable;
    int GlowThroughWall;
    int HighlightID;

    bool IsDead;
    bool IsKnocked;

    Vector3D LocalOrigin;
    Vector3D AbsoluteVelocity;

    int health;
    int shield;
    int max_shield;
    //char* PlayerName = new char[32 + 1];

    float ViewYaw;

    bool Admin;

    long long int UserId;
    std::string PlayerInfo = "";
    std::string PlayerLevel = "";
    std::string PlayerRank = "";

    int LastTimeAimedAt;
    int LastTimeAimedAtPrevious;
    float OldVisTime;
    bool IsAimedAt;

    uint64_t LastVisibleCheckTime = 0;
    float WorldTime;
    float LastVisibleTime;
    int LastTimeVisiblePrevious = 0;
    bool LastVisibleState = false;
    int VisCheckCount = 0;
    const int VisCheckThreshold = 10;
    bool IsVisible;

    bool IsLocal;
    bool IsAlly;
    bool IsHostile;
    bool IsWhite;
    bool IsBlack;
    bool IsAdmin;

    float DistanceToLocalPlayer;
    float Distance2DToLocalPlayer;

    bool IsLockedOn;

//    Vector3D AbsOrgin(bool Update = false) {
//        if (!BasePointer) return Vector3D{ 0.f, 0.f, 0.f };
//
//        return AbsoluteVelocity;
//    }
//
//    int BoneByHitBox(int HitBox)
//    {
//        //get model ptr
//        uint64_t Model = ModelPointer;
//        if (!Model) return -1;
//
//        //get studio hdr
//        uint64_t StudioHdr = mem.Read<uint64_t>(ModelPointer + 0x8, true);
//        if (!StudioHdr) return -1;
//
//        // 读取击中盒缓存信息
//        auto HitboxCache = mem.Read<uint16_t>(StudioHdr + 0x34, true);
//        //get hitbox array
//        uint64_t HitBoxsArray = StudioHdr + ((uint16_t)(HitboxCache & 0xFFFE) << (4 * (HitboxCache & 1)));
//        if (!HitBoxsArray) return -1;
//
//        //get bone index
//        int Bone = mem.Read<int>(HitBoxsArray + mem.Read<int>(HitBoxsArray + 8) + (HitBox * 0x2C));
//
//        return ((Bone < 0) || (Bone > 255)) ? -1 : Bone;
//    }
//
//    Vector3D HitBoxPos(int HitBox, bool UpdateBones)
//    {
//        //get bones base data
//        DWORD64 Bones = Myself;
//        if (!Bones) return Vector3D ();
//
//        Vector3D BoneOff;
//        int Bone = BoneByHitBox(HitBox);
//        if (Bone == -1) return Vector3D ();
//
//        Matrix3x4 BoneMatrix = mem.Read<Matrix3x4>(Bones + (Bone * sizeof(Matrix3x4)));
//        BoneOff = { BoneMatrix._14, BoneMatrix._24, BoneMatrix._34 };
//
//        Vector3D Pos = AbsOrgin(UpdateBones) + BoneOff;
//        return Pos;
//    }

    uint64_t Valid = 0;

    Player(int PlayerIndex, LocalPlayer* Me, Level* Map) {
        this->Index = PlayerIndex;
        this->Myself = Me;
        this->Map = Map;
    }

    uint64_t GetMilliseconds()
    {
        LARGE_INTEGER PerformanceFrequency;
        QueryPerformanceFrequency(&PerformanceFrequency);
        LARGE_INTEGER CurrentPerformanceCount;
        QueryPerformanceCounter(&CurrentPerformanceCount);
        return (CurrentPerformanceCount.QuadPart/*microseconds*/ * 1000/*milliseconds*/) / PerformanceFrequency.QuadPart/*microseconds*/;
    }

    //读取观战信息
    bool IsSpectating() {
        if (!IsDead)
            return false;
        uint64_t SpectatorList = mem.Read<uint64_t>(mem.OFF_BASE + OFF_OBSERVER_LIST);
        int PlayerData = mem.Read<int>(BasePointer + 0x38);
        int SpecIndex = mem.Read<int>(SpectatorList + PlayerData * 8 + OFF_OBSERVER_ARRAY);//0x0964
        uint64_t SpectatorAddr = mem.Read<uint64_t>(mem.OFF_BASE + OFF_ENTITY_LIST + ((SpecIndex & 0xFFFF) << 5));
        if (SpectatorAddr == Myself->BasePointer)
            return true;
        return false;
    }

    bool VisCheck() {
        bool Visibility = false;
        if (LastVisibleTime > OldVisTime || LastVisibleTime < 0.f && OldVisTime > 0.f) {
            Visibility = true;
        }
        OldVisTime = LastVisibleTime;
        return Visibility;
    }

    void ValidCheck() {
        if (Valid) {
            if (Valid > 0x7FFFFFFEFFFF || Valid < 0x7FF000000000) {
				BasePointer = 0;
				Valid = 0;
			}
        }
    }

    bool is_teammate() const
    {
        if (LvMap::m_mixtape && Myself->Squad == -1) {
            return (Team & 1) == (Myself->Team & 1);
        }
        return Team == Myself->Team;
    }

    void Read() {
        if (!mem.IsValidPointer(BasePointer)) { BasePointer = 0; return; }
        if (!IsPlayer() && !IsDummy()) { BasePointer = 0; return; }

        IsAimedAt = LastTimeAimedAtPrevious < LastTimeAimedAt;
        LastTimeAimedAtPrevious = LastTimeAimedAt;

        IsVisible = IsDummy() || IsAimedAt || VisCheck();    

        if (Myself->IsValid()) {
            IsLocal = Myself->BasePointer == BasePointer;
            IsAlly = is_teammate();
            IsHostile = !IsAlly;
            IsWhite = RList.IsWhite(UserId);
            IsBlack = RList.IsBlack(UserId);
            IsAdmin = RList.IsAdmin(UserId);
            DistanceToLocalPlayer = Myself->LocalOrigin.Distance(LocalOrigin);
            Distance2DToLocalPlayer = Myself->LocalOrigin.To2D().Distance(LocalOrigin.To2D());
            
        }
    }

    bool IsValid() {
        return mem.IsValidPointer(BasePointer) && !IsDead && health > 0 && (IsPlayer() || IsDummy() && Map->IsFiringRange);
    }

    bool IsCombatReady() {
        if (!IsValid())return false;
        if (IsDummy()) return true;
        if (IsDead) return false;
        if (IsKnocked) return false;
        return true;
    }

    bool IsPlayer() {
        return Name == "player";
    }

    bool IsDummy() {
        return Team == 97;
    }

    // 根据击中盒类型获取对应的骨骼索引
    int GetBoneFromHitbox(HitboxType HitBox) const {
        // 首先检查模型指针是否有效
        if (!mem.IsValidPointer(ModelPointer))
            return -1; // 如果模型指针无效，返回-1

        // 读取StudioHDR，这是模型数据的一个部分，包含了重要的模型信息
        uint64_t StudioHDR = mem.Read<uint64_t>(ModelPointer + 0x8, true);
        // 检查StudioHDR指针的有效性
        if (!mem.IsValidPointer(StudioHDR + 0x34))
            return -1; // 如果无效，返回-1

        // 读取击中盒缓存信息
        auto HitboxCache = mem.Read<uint16_t>(StudioHDR + 0x34, true);
        // 通过缓存信息计算出击中盒数组的地址
        auto HitboxArray = StudioHDR + ((uint16_t)(HitboxCache & 0xFFFE) << (4 * (HitboxCache & 1)));
        // 检查击中盒数组指针的有效性
        if (!mem.IsValidPointer(HitboxArray + 0x4))
            return -1; // 如果无效，返回-1

        // 读取索引缓存信息
        auto IndexCache = mem.Read<uint16_t>(HitboxArray + 0x4, true);
        // 通过索引缓存信息计算出击中盒索引的地址
        auto HitboxIndex = ((uint16_t)(IndexCache & 0xFFFE) << (4 * (IndexCache & 1)));
        // 根据给定的HitBox类型计算出对应骨骼的指针
        auto BonePtr = HitboxIndex + HitboxArray + (static_cast<int>(HitBox) * 0x20);
        // 检查骨骼指针的有效性
        if (!mem.IsValidPointer(BonePtr))
            return -1; // 如果无效，返回-1

        // 读取并返回骨骼索引
        return mem.Read<uint16_t>(BonePtr, true);
    }

    // 根据给定的击中盒类型获取对应骨骼的位置
    Vector3D GetBonePosition(HitboxType HitBox) const {
        // 初始化一个偏移量为0的三维VectorQuantity
        Vector3D Offset = Vector3D(0.0f, 0.0f, 0.0f);

        // 调用GetBoneFromHitbox函数获取对应的骨骼索引
        int Bone = GetBoneFromHitbox(HitBox);
        // 如果骨骼索引无效（即不在0到255之间），则返回当前位置加上偏移量
        if (Bone < 0 || Bone > 255)
            return LocalOrigin.Add(Offset);

        // 创建一个临时的骨骼指针变量，并根据骨骼索引计算实际的骨骼数据指针
        uint64_t TempBonePointer = BonePointer; // 创建一个临时非const变量
        TempBonePointer += (Bone * sizeof(Matrix3x4)); // 根据骨骼索引计算偏移
        // 检查计算得到的骨骼指针是否有效
        if (!mem.IsValidPointer(TempBonePointer))
            return LocalOrigin.Add(Offset); // 如果无效，返回当前位置加上偏移量

        // 从内存中读取骨骼矩阵
        Matrix3x4 BoneMatrix = mem.Read<Matrix3x4>(TempBonePointer);
        // 从骨骼矩阵中获取骨骼的位置
        Vector3D BonePosition = BoneMatrix.GetPosition();

        // 检查获取到的骨骼位置是否有效
        if (!BonePosition.IsValid())
            return LocalOrigin.Add(Vector3D(0, 0, 0)); // 如果无效，返回当前位置加上0偏移量

        // 将骨骼位置与本地原点相加，得到世界coord下的骨骼位置
        BonePosition += LocalOrigin;
        return BonePosition; // 返回计算得到的骨骼位置
    }
};