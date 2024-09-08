#ifndef APEX_GAME_STRUCT_H
#define APEX_GAME_STRUCT_H
#include "Vector3D.hpp"

struct Infoarray {
    Vector3D coord;
    Vector3D VectorQuantity;
    uint64_t PtrAddress;
    uint64_t ModelAddress;
    uint64_t BoneAddress;
    std::string Name;
    float Yaw;
    float Distance;
    float Distance2D;
    int Index;
    int Team;
    int Health;
    int Sheld;
    int MaxSheld;
    bool Dead;
    bool Knocked;
    bool Visible;
    bool LocalPlayer;
    bool Teammates;
    bool Enemy;
    bool Lock;
    bool Admin;
    bool White;
    bool Black;
    unsigned long ID;
    int PlayerLV;
    std::string PlayerLevel;
    std::string LevelPrestige;
    std::string PlayerRank;
    std::vector<Vector3D> PlayerBone;
};

inline std::vector<Infoarray> Temporaryarray;
inline std::vector<Infoarray> Globalarray;

#endif //APEX_GAME_STRUCT_H
