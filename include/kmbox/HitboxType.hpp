#pragma once

enum class HitboxType {
    Head = 0,
    Neck = 1,
    UpperChest = 2,
    Chest = 3,
    Hip = 4,

    UpperSpine = 5,    // 上脊椎
    MidSpine = 6,      // 中脊椎
    LowerSpine = 7,    // 下脊椎
    PropGun = 8,       // 枪支道具
    NeckA = 9,         // 颈部A
    NeckB = 10,        // 颈部B
    NeckC = 11,        // 颈部C
    HeadB = 12,        // 头部B
    LeftClavicle = 13, // 左锁骨
    LeftShoulder = 14, // 左肩膀
    LeftShoulderTwist = 15, // 左肩扭转
    LeftShoulderMid = 16,   // 左肩中部
    LeftElbowB = 17,        // 左肘部B
    LeftElbow = 18,         // 左肘部
    LeftForearm = 19,       // 左前臂
    LeftWrist = 20,         // 左腕部
    LeftPropHand = 21,      // 左手持道具
    LeftThumbA = 22,        // 左拇指A
    LeftThumbB = 23,        // 左拇指B
};

class HitBoxManager
{
private:

public:

    Vector2D ScreenHeadBone;
    Vector2D ScreenNeckBone;
    Vector2D ScreenChestBone;
    Vector2D ScreenWaistBone;
    Vector2D ScreenBotmBone;

    Vector2D ScreenLeftshoulderBone;
    Vector2D ScreenLeftelbowBone;
    Vector2D ScreenLeftHandBone;
    Vector2D ScreenRightshoulderBone;
    Vector2D ScreenRightelbowBone;
    Vector2D ScreenRightHandBone;

    Vector2D ScreenLeftThighsBone;
    Vector2D ScreenLeftkneesBone;
    Vector2D ScreenLeftlegBone;
    Vector2D ScreenRightThighsBone;
    Vector2D ScreenRightkneesBone;
    Vector2D ScreenRightlegBone;
};