#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <array>

#include "KmboxNet.hpp"
#include "KmboxB.h"
#include "HidTable.hpp"

#include "Player.hpp"
#include "LocalPlayer.hpp"
#include "Offsets.hpp"
#include "Camera.hpp"

#include "Vector2D.hpp"
#include "Vector3D.hpp"
#include "QAngle.hpp"
#include "Resolver.hpp"

#include "Memory.h"
#include "Conversion.hpp"
#include "HitboxType.hpp"

#include "prediction.h"
#include "Vector.h"
#include "skynade.h"

#include "ReadList.hpp"

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif
#include <iostream>
#include <vector>
#include <algorithm>
#pragma execution_character_set("utf-8")

// Conversion

struct Aimbot {

    bool KmboxInitialized = false; // 新增状态变量，用于存储初始化状态

    float aim_time=0.005;
    std::chrono::steady_clock::time_point Aim_old_time = std::chrono::steady_clock::now();

    // 根据算法计算出的最终目标距离
    float FinalDistance = 0;

    // 瞄准时是否“粘附”在目标上，以便更容易跟踪
    bool Sticky = true;
    // 瞄准过程的平滑度，数值越大，瞄准动作越平滑
    float SkySmooth = 1.f;
    float Smooth = 2.0f;
    float ZoomSmooth = 3.f;
    float Flicksmooth = 1.5f;
    // 平滑度的最大递增量，用于动态调整瞄准平滑度
    float MaxSmoothIncrease = 0.20f;
    // 瞄准功能的有效视野范围
    float FOV = 10.0f;
    float ZoomFOV = 6.f;
    // 缩放时的视野比例，用于调整瞄准精度
    float ZoomScale = 1.2;
    // 触发瞄准功能的最小距离
    float MinDistance = 0;
    // 非瞄准（走火）状态下的最大瞄准距离
    float HipfireDistance = 50;
    // 瞄准（开镜）状态下的最大瞄准距离
    float ZoomDistance = 300;
    // 瞄准操作的最小延迟，以毫秒计
    int MinimumDelay = 1;
    // 对后坐力的补偿值
    float RecoilCompensation = 1.0f;

    bool DrawFOV = false;
    //自动化功能
    int aimBone = 2;
    bool autobone = true;
    bool allbone = false;
    std::string AimBotType = "KmBox";
    bool fastreload = true;
    Angles CalculatedAngles = Angles(0, 0, 0);
    Angles SwayAngles = Angles(0, 0, 0);
    Angles SkynadeAngles = Angles(0, 0, 0);
    Angles SmoothedAngles = Angles(0, 0, 0);

    Vector3D SkyPoint{};

    // 瞄准功能激活的按键
    int AimBotKey = 0x01;
    int AimBotKey2 = 0x0;
    int AimTriggerKey = 0x05;
    int AimFlickKey = 0x06;
    int AimHeadKey = VK_SHIFT;
    int ThrowKey = 0x2;
    //input system key
    int inputAimKey = 108;
    int inputAimKey2 = 0;
    int inputTriggerKey = 111;
    int inputFlickKey = 112;
    int inputHeadKey = 79;
    int inputThrowKey = 109;
    bool InputModel = true;
    bool AimTeam = false;

    // 指向代表本地玩家的对象
    LocalPlayer* Myself;
    // 存储所有游戏中玩家对象的指针列表
    std::vector<Player*>* Players;
    // 指向游戏相机对象
    Camera* GameCamera;

    // 当前的目标玩家对象
    Player* CurrentTarget = nullptr;

    //高抛目标
    Player* SKYTargetGet = nullptr;

    // 是否已选择目标
    bool TargetSelected = true;
    bool SKYTargetSelected = false;


    float addx = 0.0f;
    float addy = 0.0f;


    // 上一次瞄准操作的时间
    //std::chrono::milliseconds LastAimTime{ 0 };

    // 构造函数，初始化Aimbot对象
    Aimbot(LocalPlayer* Myself, std::vector<Player*>* GamePlayers, Camera* Cam) {
        this->Myself = Myself;
        this->Players = GamePlayers;
        this->GameCamera = Cam;
    }

    //霰弹枪
    std::array<int, 4> Shotguns = { 104, 96, 97, 88 };
    // 连接到Kmbox设备的配置
    std::string KmboxType = "BPro"; // Kmbox的类型
    char KmboxIP[24] = "192.168.2.188"; // Kmbox的IP地址
    char KmboxPort[10] = "8336"; // Kmbox的端口号
    char KmboxUUID[32] = "24B75054"; // Kmbox的UUID
    int KmboxComPort = 4; // Kmbox的通信端口
    int BaudRate = 115200; //波特率

    // Kmbox通信端口的定义，此部分代码被省略，需要根据实际情况补充_com的定义
    _com comPort;

    void Initialize() {
        KmboxInitialized = false; // 默认设为 false
        if (KmboxType == "Net") {
            std::cout << "[#] 正在初始化 Kmbox Net..." << std::endl;
            MinimumDelay = 1;

            int result = kmNet_init(KmboxIP, KmboxPort, KmboxUUID);

            if (result != 0) {
                std::cout << "[#] Kmbox Net初始化失败." << std::endl;
            }
            else {
                std::cout << "[#] Kmbox Net初始化成功." << std::endl;
                KmboxInitialized = true;
            }
        }
        else if (KmboxType == "BPro") {
            std::cout << "[#] 正在初始化 Kmbox B+ Pro..." << std::endl;
            MinimumDelay = 3;

            if (comPort.open(KmboxComPort, BaudRate)) {
                std::cout << "[#] Kmbox B+ Pro初始化成功." << std::endl;
                KmboxInitialized = true;
            }
            else {
                std::cout << "[#] Kmbox B+ Pro初始化失败." << std::endl;
            }
        }

    }

    void Move(int x, int y) {
        if (KmboxType == "Net") {
            kmNet_mouse_move((short)x, (short)y);
        }
        else if (KmboxType == "BPro") {
            char cmd[1024] = { 0 };
            //模拟手动，过大会导致KM重启
            sprintf_s(cmd, "km.move(%d, %d)\r\n", x, y);
            comPort.write(cmd);
        }
    }

    void writeCommand(char* cmd, const std::string& message) {
        //std::cout << "Sending command: " << cmd << " - " << message << std::endl;
        comPort.write(cmd);
    }

    void TwoGunBug()
    {
        if (mem.isPressed(99) || mem.GetKeyboard()->IsKeyDown(VK_F8)) {
            if (KmboxType == "Net") {
                //bug1
                kmNet_keydown(KEY_1_EXCLAMATION_MARK);
                Sleep(125);
                kmNet_keyup(KEY_1_EXCLAMATION_MARK);
                Sleep(580);
                kmNet_keydown(KEY_Z);
                Sleep(62);
                kmNet_keydown(KEY_2_AT);
                Sleep(47);
                kmNet_keyup(KEY_Z);
                Sleep(47);
                kmNet_keyup(KEY_2_AT);
                Sleep(65);
                //bug2
                kmNet_keydown(KEY_1_EXCLAMATION_MARK);
                Sleep(10);
                kmNet_keyup(KEY_1_EXCLAMATION_MARK);
                Sleep(260);
                kmNet_keydown(KEY_Z);
                Sleep(75);
                kmNet_keyup(KEY_Z);
                Sleep(10);
                kmNet_keydown(KEY_2_AT);
                Sleep(50);
                kmNet_keyup(KEY_2_AT);
                Sleep(30);
                kmNet_keydown(KEY_1_EXCLAMATION_MARK);
                Sleep(30);
                kmNet_keyup(KEY_1_EXCLAMATION_MARK);
                Sleep(30);
                kmNet_keydown(KEY_2_AT);
                Sleep(30);
                kmNet_keyup(KEY_2_AT);
            } else if (KmboxType == "BPro") {
                char cmd[1024] = {0};
                // 按下1然后保持125毫秒
                sprintf_s(cmd, "km.press(%d, %d)\r\n", 30, 125);
                writeCommand(cmd, "Pressing '1' for 125ms");
                std::this_thread::sleep_for(std::chrono::milliseconds(580));
                // 按下Z保持50毫秒
                sprintf_s(cmd, "km.press(%d, %d)\r\n", 29, 50);
                writeCommand(cmd, "Pressing 'Z' for 50ms");
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                // 按下2不添加保持
                sprintf_s(cmd, "km.press(%d, %d)\r\n", 31, 100);
                writeCommand(cmd, "Pressing '2' for 100ms");
                std::this_thread::sleep_for(std::chrono::milliseconds(65));
                // 按下1保持10毫秒
                sprintf_s(cmd, "km.press(%d, %d)\r\n", 30, 10);
                writeCommand(cmd, "Pressing '1' for 10ms");
                std::this_thread::sleep_for(std::chrono::milliseconds(260));
                // 按下Z保持50毫秒
                sprintf_s(cmd, "km.press(%d, %d)\r\n", 29, 75);
                writeCommand(cmd, "Pressing 'Z' for 75ms");
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                // 按下2保持50毫秒
                sprintf_s(cmd, "km.press(%d, %d)\r\n", 31, 50);
                writeCommand(cmd, "Pressing '2' for 50ms");
                std::this_thread::sleep_for(std::chrono::milliseconds(30));
                // 按下1保持30毫秒
                sprintf_s(cmd, "km.press(%d, %d)\r\n", 30, 30);
                writeCommand(cmd, "Pressing '1' for 30ms");
                std::this_thread::sleep_for(std::chrono::milliseconds(30));
                // 按下2不添加保持
                sprintf_s(cmd, "km.press(%d)\r\n", 31);
                writeCommand(cmd, "Pressing '2'");
            }
        }
    }

    void LeftClick() {
        if (AimBotType == "Memory") {
            mem.Write<int>(mem.OFF_BASE + OFF_INATTACK + 0x8, 5);
            Sleep((int)Utils::RandomRange((float)MinimumDelay, 10));
            mem.Write<int>(mem.OFF_BASE + OFF_INATTACK + 0x8, 4);
        }
        else
        {
            if (KmboxType == "Net") {
                kmNet_mouse_left(1);
                Sleep((int) Utils::RandomRange((float)MinimumDelay, 10));
                kmNet_mouse_left(0);
            } else if (KmboxType == "BPro") {
                char cmd[1024] = {0};
                sprintf_s(cmd, "km.click(0)\r\n");
                Sleep((int) Utils::RandomRange((float)MinimumDelay, 10));
                comPort.write(cmd);
            }
        }
    }

    enum : int
    {
        mp_weapon_rspn101 = 0,
        mp_weapon_sentinel = 1,
        mp_weapon_bow = 2,
        mp_weapon_titan_sword = 3,
        mp_ability_emote_projector = 4,
        mp_ability_generic_offhand = 5,
        mp_weapon_dragon_lmg = 6,
        npc_weapon_hemlok = 7,
        npc_weapon_thermite_grenade = 8,
        npc_weapon_lstar = 9,
        npc_weapon_spider_jungle = 10,
        mp_ability_gibraltar_shield = 11,
        mp_weapon_defensive_bombardment_weapon = 12,
        mp_weapon_creeping_bombardment_weapon = 13,
        mp_weapon_mounted_turret_weapon = 14,
        mp_weapon_mounted_turret_placeable = 15,
        mp_weapon_cluster_bomb = 16,
        mp_weapon_mortar_ring_missle = 17,
        mp_ability_valk_jets = 18,
        mp_ability_heartbeat_sensor = 19,
        mp_weapon_ash_dataknife = 20,
        mp_ability_rolling_thunder_piece = 21,
        mp_weapon_revive_shield = 22,
        mp_ability_companion_launch = 23,
        mp_weapon_vantage_recall = 24,
        mp_ability_reinforce = 25,
        mp_ability_conduit_shield_mines_line = 26,
        mp_ability_transport_portal_datapad = 27,
        weapon_cubemap = 28,
        npc_weapon_energy_shotgun = 29,
        mp_ability_item_spawner = 30,
        mp_ability_panic_button = 31,
        mp_weapon_grenade_bangalore = 32,
        mp_weapon_grenade_creeping_bombardment = 33,
        mp_ability_area_sonar_scan = 34,
        mp_ability_hunt_mode = 35,
        mp_weapon_dirty_bomb = 36,
        mp_weapon_grenade_gas = 37,
        mp_weapon_bubble_bunker = 38,
        mp_weapon_grenade_defensive_bombardment = 39,
        mp_weapon_deployable_medic = 40,
        mp_ability_care_package = 41,
        mp_ability_holopilot = 42,
        mp_ability_mirage_ultimate = 43,
        mp_ability_octane_stim = 44,
        mp_weapon_jump_pad = 45,
        mp_ability_grapple = 46,
        mp_weapon_zipline = 47,
        mp_ability_phase_walk = 48,
        mp_weapon_phase_tunnel = 49,
        mp_weapon_tesla_trap = 50,
        mp_weapon_trophy_defense_system = 51,
        mp_ability_crypto_drone = 52,
        mp_ability_crypto_drone_emp = 53,
        mp_ability_revenant_shadow_pounce_free = 54,
        mp_ability_shadow_form = 55,
        mp_ability_translocation = 56,
        mp_ability_black_market = 57,
        mp_weapon_cover_wall = 58,
        mp_weapon_mobile_hmg = 59,
        mp_ability_horizon_tac_space_elevator = 60,
        mp_ability_horizon_black_hole = 61,
        mp_weapon_riot_drill = 62,
        mp_ability_wrecking_ball = 63,
        mp_ability_valk_cluster_missile = 64,
        mp_ability_valk_skyward = 65,
        mp_ability_conduit_arc_flash = 66,
        mp_ability_conduit_shield_mines = 67,
        mp_weapon_arc_bolt = 68,
        mp_weapon_phase_breach = 69,
        mp_ability_debuff_zone = 70,
        mp_ability_portable_auto_loader = 71,
        mp_weapon_cluster_bomb_launcher = 72,
        mp_weapon_mortar_ring = 73,
        mp_ability_sonic_blast = 74,
        mp_ability_echo_locator = 75,
        mp_ability_shield_throw = 76,
        mp_ability_armored_leap = 77,
        mp_ability_companion_launch_entry = 78,
        mp_ability_sniper_ult = 79,
        mp_ability_spike_strip = 80,
        mp_weapon_ferro_wall = 81,
        mp_ability_phase_door = 82,
        mp_ability_transport_portal = 83,
        mp_weapon_alternator_smg = 84,
        mp_weapon_autopistol = 85,
        mp_weapon_autopistol_crate = 86,
        mp_weapon_defender = 87,
        mp_weapon_esaw = 88,
        mp_weapon_esaw_crate = 89,
        mp_weapon_dmr = 90,
        mp_weapon_energy_ar = 91,
        mp_weapon_shotgun = 92,
        mp_weapon_shotgun_crate = 93,
        mp_weapon_vinson = 94,
        mp_weapon_g2 = 95,
        mp_weapon_hemlok = 96,
        mp_weapon_hemlok_crate = 97,
        mp_weapon_sniper = 98,
        mp_weapon_lstar = 99,
        mp_weapon_lstar_crate = 100,
        mp_weapon_mastiff = 101,
        mp_weapon_shotgun_pistol = 102,
        mp_weapon_shotgun_pistol_light = 103,
        mp_weapon_shotgun_pistol_energy = 104,
        mp_weapon_shotgun_pistol_sniper = 105,
        mp_weapon_shotgun_pistol_heavy = 106,
        mp_weapon_pdw = 107,
        mp_weapon_pdw_crate = 108,
        mp_weapon_energy_shotgun = 109,
        mp_weapon_r97 = 110,
        mp_weapon_r97_crate = 111,
        mp_weapon_semipistol = 112,
        mp_weapon_lmg = 113,
        mp_weapon_doubletake = 114,
        mp_weapon_wingman = 115,
        mp_weapon_wingman_crate = 116,
        mp_weapon_volt_smg = 117,
        mp_weapon_3030 = 118,
        mp_weapon_car = 119,
        mp_weapon_nemesis = 120,
        mp_weapon_melee_survival = 121,
        melee_pilot_emptyhanded = 122,
        mp_weapon_wraith_kunai_primary = 123,
        melee_wraith_kunai = 124,
        mp_weapon_bangalore_heirloom_primary = 125,
        melee_bangalore_heirloom = 126,
        mp_weapon_bloodhound_axe_primary = 127,
        melee_bloodhound_axe = 128,
        mp_weapon_caustic_hammer_primary = 129,
        melee_caustic_hammer = 130,
        mp_weapon_gibraltar_club_primary = 131,
        melee_gibraltar_club = 132,
        mp_weapon_lifeline_baton_primary = 133,
        melee_lifeline_baton = 134,
        mp_weapon_mirage_statue_primary = 135,
        melee_mirage_statue = 136,
        mp_weapon_octane_knife_primary = 137,
        melee_octane_knife = 138,
        mp_weapon_pathfinder_gloves_primary = 139,
        melee_pathfinder_gloves = 140,
        mp_weapon_wattson_gadget_primary = 141,
        melee_wattson_gadget = 142,
        mp_weapon_crypto_heirloom_primary = 143,
        melee_crypto_heirloom = 144,
        mp_weapon_revenant_scythe_primary = 145,
        melee_revenant_scythe = 146,
        mp_weapon_loba_heirloom_primary = 147,
        melee_loba_heirloom = 148,
        mp_weapon_rampart_wrench_primary = 149,
        melee_rampart_wrench = 150,
        mp_weapon_horizon_heirloom_primary = 151,
        melee_horizon_heirloom = 152,
        mp_weapon_valkyrie_spear_primary = 153,
        melee_valkyrie_spear = 154,
        mp_weapon_ash_heirloom_primary = 155,
        melee_ash_heirloom = 156,
        mp_weapon_seer_heirloom_primary = 157,
        melee_seer_heirloom = 158,
        mp_weapon_wraith_kunai_rt01_primary = 159,
        melee_wraith_kunai_rt01 = 160,
        mp_weapon_fuse_heirloom_primary = 161,
        melee_fuse_heirloom = 162,
        mp_weapon_artifact_dagger_primary = 163,
        melee_artifact_dagger = 164,
        mp_weapon_revenant_scythe_primary_rt01 = 165,
        melee_revenant_scythe_rt01 = 166,
        mp_weapon_artifact_sword_primary = 167,
        melee_artifact_sword = 168,
        mp_weapon_octane_knife_rt01_primary = 169,
        melee_octane_knife_rt01 = 170,
        mp_weapon_crypto_heirloom_rt01_primary = 171,
        melee_crypto_heirloom_rt01 = 172,
        mp_weapon_bangalore_heirloom_rt01_primary = 173,
        melee_bangalore_heirloom_rt01 = 174,
        mp_weapon_incap_shield = 175,
        mp_weapon_throwingknife = 176,
        mp_weapon_thermite_grenade = 177,
        mp_weapon_frag_grenade = 178,
        mp_weapon_grenade_emp = 179,
        mp_ability_mobile_respawn_beacon = 180,
        mp_ability_void_ring = 181,
        mp_ability_redeploy_balloon = 182,
        mp_ability_copycat_kit_bloodhound_sonar = 183,
        mp_ability_copycat_kit_horizon_elevator = 184,
        mp_ability_copycat_kit_mirage_holopilot = 185,
        mp_ability_copycat_kit_wraith_phase_walk = 186,
        mp_ability_copycat_kit_catalyst_wall = 187,
        mp_ability_copycat_kit_octane_pad = 188,
        mp_ability_copycat_kit_ash_phase_blade = 189,
        mp_ability_copycat_kit_caustic_grenade_gas = 190,
        mp_weapon_grenade_rev_shell = 191,
        mp_ability_consumable = 192,
        npc_weapon_nessie = 193,
    };

    bool UseFastReload() {
        if(Myself->WeaponIndex == mp_weapon_sentinel ||
           Myself->WeaponIndex == mp_weapon_defender ||
           Myself->WeaponIndex == mp_weapon_dmr ||
           Myself->WeaponIndex == mp_weapon_shotgun ||
           Myself->WeaponIndex == mp_weapon_sniper ||
           Myself->WeaponIndex == mp_weapon_mastiff ||
           Myself->WeaponIndex == mp_weapon_shotgun_pistol ||
           Myself->WeaponIndex == mp_weapon_energy_shotgun ||
           Myself->WeaponIndex == mp_weapon_doubletake ||
           Myself->WeaponIndex == mp_weapon_wingman ||
           Myself->WeaponIndex == mp_weapon_3030 )
        {
            return false;
        }
        return true;
    };

    void Update_TacticalReload() {
        //std::cout << " Weap:" << Myself->WeaponIndex << " ammo" << Myself->Ammo << std::endl;
        if (fastreload) {
            if (!Myself->IsCombatReady()) { return; }
            bool isTacticalReload = false;
            if (!isTacticalReload && UseFastReload()) {
                if (Myself->Ammo == 1) {
                    //Reload();
                    isTacticalReload = true;
                    mem.Write<int>(mem.OFF_BASE + OFF_INATTACK + 0x8, 4);//停止开枪
                    mem.Write<int>(mem.OFF_BASE + OFFSET_IN_RELOAD + 0x8, 5);//换弹
                    isTacticalReload = false;
                    mem.Write<int>(mem.OFF_BASE + OFFSET_IN_RELOAD + 0x8, 4);

//                    auto handle = mem.CreateScatterHandle();
//                    int a = 1, b = 4;
//
//                    mem.AddScatterWriteRequest(handle, mem.OFF_BASE + OFF_INATTACK + 0x8, &b, sizeof(int));//停止开枪
//                    mem.AddScatterWriteRequest(handle, mem.OFF_BASE + OFFSET_IN_RELOAD + 0x8, &a, sizeof(int));//换弹
//
//                    mem.ExecuteWriteScatter(handle);
//                    mem.CloseScatterHandle(handle);
                }
            }
        }
    }

    void Update_Aimbot()
    {
        if (Myself->IsZooming)
            FinalDistance = ZoomDistance;
        else
            FinalDistance = HipfireDistance;

        if (mem.GetKeyboard()->IsKeyDown(AimTriggerKey) && !InputModel) { return; }
        if (mem.isPressed(inputTriggerKey) && InputModel) { return; }

        if (!Myself->IsCombatReady()) {
            CurrentTarget = nullptr;
            return;
        }

//        if (!mem.GetKeyboard()->IsKeyDown(AimBotKey) && !mem.GetKeyboard()->IsKeyDown(AimBotKey2) && !mem.GetKeyboard()->IsKeyDown(AimFlickKey) && !Myself->IsInAttack) {
        if (!mem.GetKeyboard()->IsKeyDown(AimBotKey) && !mem.GetKeyboard()->IsKeyDown(AimBotKey2) && !mem.GetKeyboard()->IsKeyDown(AimFlickKey) && !InputModel) {
            ReleaseTarget();
            return;
        }

        if (!mem.isPressed(inputAimKey) && !mem.isPressed(inputAimKey2) && !mem.isPressed(inputFlickKey) && InputModel) {
            ReleaseTarget();
            return;
        }

        if (Myself->IsHoldingGrenade) {
            ReleaseTarget();
            return;
        }

        if (!Sticky || !IsValidTarget(CurrentTarget)) {
            Player* Target = FindBestTarget();
            if (!IsValidTarget(Target)) {
                ReleaseTarget();
                return;
            }

            CurrentTarget = Target;
            CurrentTarget->IsLockedOn = true;
            TargetSelected = true;
        }
        if (!Myself->IsHoldingGrenade) {
            std::chrono::steady_clock::time_point Aim_new_time = std::chrono::steady_clock::now();
            std::chrono::duration<double> Date_time = Aim_new_time - Aim_old_time;
            if (TargetSelected && CurrentTarget) {
                {
                    if (AimBotType == "Memory") {
                        if (Date_time.count() < aim_time)
                        {
                            return;
                        }
                        StartAiming(CurrentTarget);
                    } else {
                        if (Date_time.count() < aim_time)
                        {
                            return;
                        }
                        StartAiming(CurrentTarget);
                    }
                }
            }
            Aim_new_time = std::chrono::steady_clock::now();
            Aim_old_time = Aim_new_time;
        }
    }

    void StartAiming(Player* Target) {
        auto TargetPosition = CalculatePredictedPosition(Target->GetBonePosition(static_cast<HitboxType>(GetBestBone(Target))), Target->AbsoluteVelocity, Myself->WeaponProjectileSpeed, Myself->WeaponProjectileScale);
        Vector2D ScreenPosition;

        if (GameCamera->WorldToScreen(TargetPosition, ScreenPosition)) {
            auto Center = GameCamera->GetCenter();
            Vector2D RelativePosition = { ScreenPosition.x - Center.x, ScreenPosition.y - Center.y };
            float baseSmoothing = RList.IsBlack(Target->UserId) ? 1 : (((mem.GetKeyboard()->IsKeyDown(AimFlickKey) && !InputModel) || (mem.isPressed(inputFlickKey) && InputModel)) ? Flicksmooth : Myself->IsZooming ? ZoomSmooth : Smooth);
            const float baseFOV = ((mem.GetKeyboard()->IsKeyDown(0x2) && !InputModel) || (mem.isPressed(109) && InputModel)) ? ZoomFOV : FOV;
            const float maxPercentageIncrease = RList.IsBlack(Target->UserId) ? 0.f : MaxSmoothIncrease;
            const float maxIncrease = baseSmoothing * maxPercentageIncrease;

            //baseSmoothing += Utils::RandomRange(-0.05f, 0.05f);

            auto distance = (float)CalculateDistanceFromCrosshair(TargetPosition);
            float dynamicSmoothing = CalculateDynamicSmoothing(distance, baseSmoothing, baseFOV, maxIncrease);

            Vector2D step = CalculateStep(RelativePosition, dynamicSmoothing);
            {
                if (std::abs(step.x) >= 1.0f || std::abs(step.y) >= 1.0f) {
                    if (AimBotType == "KmBox") {
                        Move((int)step.x, (int)step.y);
                    } else {
                        mem.Write<SVector>(Myself->BasePointer + OFFSET_VIEWANGLES, SmoothedAngles);
                    }
                }
            }
        }

        if ((mem.GetKeyboard()->IsKeyDown(AimFlickKey) && !InputModel) || (mem.isPressed(inputFlickKey) && InputModel)) {
            AimAtBones(Target);
        }
    }

    float GetAimFOV(Player* Target) {
        Angles BreathAngle = mem.Read<Angles>(Myself->BasePointer + OFFSET_BREATH_ANGLES);
        Vector3D LocalCamera = Myself->CameraPosition;
        Vector3D EntityPOS = Target->LocalOrigin;
        Angles Angle = Math::CalcAngle(LocalCamera, EntityPOS);
        return Math::GetFov(BreathAngle, Angle);
    }

    static float CalculateDynamicSmoothing(float distance, float baseSmoothing, float baseFOV, float maxIncrease) {
        if (distance > baseFOV) return baseSmoothing;
        //float scale = 1.0f - (distance / baseFOV);
        return std::max(baseSmoothing + maxIncrease, baseSmoothing);
    }

    [[nodiscard]] Vector2D CalculateStep(const Vector2D& RelativePosition, float dynamicSmoothing) const {
        bool isAttacking = Myself->IsInAttack || (mem.GetKeyboard()->IsKeyDown(VK_LBUTTON) && !InputModel) || (mem.isPressed(108) && InputModel);
        return {
                RelativePosition.x / dynamicSmoothing,
                RelativePosition.y / (isAttacking ? dynamicSmoothing * RecoilCompensation : dynamicSmoothing)
        };
    }

    void AimAtBones(Player* Target) {
        std::vector<int> bones = { 0, 3 };
        for (int boneIndex : bones) {
            auto bonePosition = CalculatePredictedPosition(Target->GetBonePosition(static_cast<HitboxType>(boneIndex)), Target->AbsoluteVelocity, Myself->WeaponProjectileSpeed, Myself->WeaponProjectileScale);
            auto boxDimensions = GetBoxDimensionsForBone(boneIndex);
            auto corners = CalculateBoxCorners(bonePosition, boxDimensions);

            if (IsTargetWithinCrosshair(corners)) {
                LeftClick();
                break;
            }
        }
    }

    static std::vector<Vector3D> CalculateBoxCorners(const Vector3D& position, const Vector3D& dimensions) {
        return {
                {position.x + dimensions.x, position.y + dimensions.y, position.z + dimensions.z},
                {position.x - dimensions.x, position.y - dimensions.y, position.z - dimensions.z},
                {position.x + dimensions.x, position.y - dimensions.y, position.z + dimensions.z},
                {position.x - dimensions.x, position.y + dimensions.y, position.z - dimensions.z},
        };
    }

    static Vector3D GetBoxDimensionsForBone(int boneIndex) {
        if (boneIndex == 0) return { 5.0f, 5.0f, 5.0f }; // Head
        if (boneIndex == 3) return { 8.0f, 8.0f, 12.0f }; // Body
        return { 0.0f, 0.0f, 0.0f }; // Default, should not happen
    }

    [[nodiscard]] bool IsTargetWithinCrosshair(const std::vector<Vector3D>& corners) const {
        float minX = FLT_MAX, maxX = -FLT_MAX, minY = FLT_MAX, maxY = -FLT_MAX;
        Vector2D screenPos;

        for (const auto& corner : corners) {
            if (GameCamera->WorldToScreen(corner, screenPos)) {
                minX = std::min(minX, screenPos.x);
                maxX = std::max(maxX, screenPos.x);
                minY = std::min(minY, screenPos.y);
                maxY = std::max(maxY, screenPos.y);
            }
        }

        auto Center = GameCamera->GetCenter();
        return Center.x >= (minX - 1) && Center.x <= (maxX + 1) &&
               Center.y >= (minY - 1.5) && Center.y <= (maxY + 1.5);
    }

    void Update_Triggerbot() {
        if (Myself->IsZooming)
            FinalDistance = ZoomDistance;
        else FinalDistance = HipfireDistance;

        if (!Myself->IsCombatReady()) { return; }

        if (!mem.GetKeyboard()->IsKeyDown(AimTriggerKey) && !InputModel) { return; }
        if (!mem.isPressed(inputTriggerKey) && InputModel) { return; }

        if (Myself->IsHoldingGrenade) { return; }

        Player* Target = CurrentTarget;
        if (Sticky) {
            if (!IsValidTarget(Target)) {
                if (TargetSelected) {
                    return;
                }

                Target = FindBestTarget();
                if (!IsValidTarget(Target)) {
                    ReleaseTarget();
                    return;
                }

                CurrentTarget = Target;
                CurrentTarget->IsLockedOn = true;
                TargetSelected = true;
            }
        }
        else {
            Target = FindBestTarget();
            if (!IsValidTarget(Target)) {
                ReleaseTarget();
                return;
            }

            CurrentTarget = Target;
            CurrentTarget->IsLockedOn = true;
            TargetSelected = true;
        }

        std::chrono::steady_clock::time_point Aim_new_time = std::chrono::steady_clock::now();
        std::chrono::duration<double> Date_time = Aim_new_time - Aim_old_time;
        if (TargetSelected && CurrentTarget) {
            if (Date_time.count() < aim_time)
            {
                return;
            }
            StartTrigger(CurrentTarget);
            return;
        }
        Aim_new_time = std::chrono::steady_clock::now();
        Aim_old_time = Aim_new_time;
    }

    void StartTrigger(Player* Target) {
        std::vector<int> bones = { 0, 3 };

        for (int boneIndex : bones) {
            Vector3D bonePosition = CalculatePredictedPosition(Target->GetBonePosition(static_cast<HitboxType>(boneIndex)), Target->AbsoluteVelocity, Myself->WeaponProjectileSpeed, Myself->WeaponProjectileScale);

            static float boxWidth, boxDepth, boxHeight;

            if (boneIndex == 0) { // Head
                boxWidth = boxDepth = boxHeight = 0; // Assuming the head is roughly a cube
            }
            else if (boneIndex == 3) { // Body
                boxWidth = 3.0; // X
                boxDepth = 3.0; // Y
                boxHeight = 5.0; // Z
            }

            // Calculate corner points of the box around the bone in world space
            std::vector<Vector3D> corners = {
                    {bonePosition.x + boxWidth, bonePosition.y + boxDepth, bonePosition.z + boxHeight},
                    {bonePosition.x - boxWidth, bonePosition.y - boxDepth, bonePosition.z - boxHeight},
                    {bonePosition.x + boxWidth, bonePosition.y - boxDepth, bonePosition.z + boxHeight},
                    {bonePosition.x - boxWidth, bonePosition.y + boxDepth, bonePosition.z - boxHeight},
            };

            float minX = FLT_MAX, maxX = -FLT_MAX, minY = FLT_MAX, maxY = -FLT_MAX;

            for (const auto& corner : corners) {
                Vector2D screenPos;
                if (GameCamera->WorldToScreen(corner, screenPos)) {
                    minX = (std::min)(minX, screenPos.x);
                    maxX = (std::max)(maxX, screenPos.x);
                    minY = (std::min)(minY, screenPos.y);
                    maxY = (std::max)(maxY, screenPos.y);
                }
            }

            Vector2D Center = GameCamera->GetCenter();
            if (Center.x >= minX && Center.x <= maxX && Center.y >= minY && Center.y <= maxY) {
                LeftClick();
                break;
            }
        }
    }

    bool IsValidTarget(Player* target) const {
        if (target == nullptr ||
            !target->IsValid() ||
            !target->IsCombatReady() ||
            !target->IsVisible ||
//            !target->IsHostile ||
            (!target->IsHostile && !AimTeam) ||
            RList.IsWhite(target->UserId) ||
            target->Distance2DToLocalPlayer < Conversion::ToGameUnits(MinDistance) ||
            target->Distance2DToLocalPlayer > Conversion::ToGameUnits(FinalDistance))
            return false;
        return true;
    }

    [[nodiscard]] double CalculateDistanceFromCrosshair(Vector3D TargetPosition) const {
        Vector3D CameraPosition = Myself->CameraPosition;
        QAngle CurrentAngle = QAngle(Myself->ViewAngles.x, Myself->ViewAngles.y).NormalizeAngles();

        if (CameraPosition.Distance(TargetPosition) <= 0.0001f)
            return -1;

        QAngle TargetAngle = Resolver::CalculateAngle(CameraPosition, TargetPosition);
        if (!TargetAngle.isValid())
            return -1;

        return CurrentAngle.distanceTo(TargetAngle);
    }

    void ReleaseTarget() {
        if (CurrentTarget != nullptr && CurrentTarget->IsValid())
            CurrentTarget->IsLockedOn = false;

        TargetSelected = false;
        CurrentTarget = nullptr;
    }

    [[nodiscard]] float GetFOVScale() const {
        if (Myself->IsValid()) {
            if (Myself->IsZooming) {
                if (Myself->TargetZoomFOV > 1.0 && Myself->TargetZoomFOV < 90.0) {
                    return tanf(DEG2RAD(Myself->TargetZoomFOV) * (0.64285714285));
                }
            }
        }
        return 1.0;
    }

    int GetBestBone(Player* Target) const {
        float NearestDistance = 999;
        int NearestBone = 1;
        if ((mem.GetKeyboard()->IsKeyDown(AimHeadKey) && !InputModel) || (mem.isPressed(inputHeadKey) && InputModel))
        {
            return NearestBone = 0;
        }
        else if (autobone)
        {
            for (int i = 0; i < 4; i++) {
                auto Bone = static_cast<HitboxType>(i);
                double DistanceFromCrosshair = CalculateDistanceFromCrosshair(Target->GetBonePosition(Bone));
                if (DistanceFromCrosshair < NearestDistance) {
                    NearestBone = i;
                    NearestDistance = (float)DistanceFromCrosshair;
                }
            }
            return NearestBone;
        }
        else if (allbone)
        {
            for (int i = 0; i <= 19; i++) {
                auto Bone = static_cast<HitboxType>(i);
                double DistanceFromCrosshair = CalculateDistanceFromCrosshair(Target->GetBonePosition(Bone));
                if (DistanceFromCrosshair < NearestDistance) {
                    NearestBone = i;
                    NearestDistance = (float)DistanceFromCrosshair;
                }
            }
            return NearestBone;
        }
        else
        {
            return NearestBone = RList.IsBlack(Target->UserId) ? 0 : aimBone;
        }
        return NearestBone = RList.IsBlack(Target->UserId) ? 0 : aimBone;
    }

    Player* FindBestTarget() {
        Player* NearestTarget = nullptr;
        float BestDistance = std::numeric_limits<float>::max(); // 使用标准的最大值
        float BestFOV = std::min(FOV, FOV * (GetFOVScale() * ZoomScale));
        bool Shotgun = std::find(Shotguns.begin(), Shotguns.end(), Myself->WeaponIndex) != Shotguns.end();
        Vector3D CameraPosition = Myself->CameraPosition;

        for (Player* p : *Players) { // 使用范围基于的for循环，更简洁
            if (!IsValidTarget(p)) continue;

            if (p->DistanceToLocalPlayer >= Conversion::ToGameUnits(ZoomDistance)) continue; // 提前继续，减少嵌套

            Vector3D TargetPosition = p->GetBonePosition(static_cast<HitboxType>(GetBestBone(p)));
            float Distance = CameraPosition.Distance(TargetPosition);
            auto CurrentFOV = (float)CalculateDistanceFromCrosshair(TargetPosition);

            // 对于霰弹枪，如果目标在5米内，放宽FOV限制
            if (Shotgun && Conversion::ToMeters(Distance) < 5.0f) {
                CurrentFOV /= 10; // 降低FOV要求，而不是增加BestFOV
            }

            // 检查是否是更近的目标且在FOV范围内
            if (CurrentFOV <= BestFOV && Distance < BestDistance) {
                NearestTarget = p;
                BestDistance = Distance;
                BestFOV = CurrentFOV; // 更新最佳FOV，以便更紧密地匹配目标
            }
            //BestFOV = GetAimFOV(p);
        }
        return NearestTarget;
    }

    //预判
    //float CalculateBulletSpeedScale(float smoothingValue) {
    //    return 1.00f - (smoothingValue - 1) * 0.10f;
    //}
    Vector3D CalculatePredictedPosition(Vector3D targetPosition, Vector3D targetVelocity, float BulletSpeed, float BulletGrav) {
        Vector3D playerPosition = Myself->CameraPosition;
        //float bulletSpeedScale = CalculateBulletSpeedScale(Smooth);

        // 特定武器速度调整
        if (Myself->WeaponIndex == 2) { // Bocek
            BulletSpeed = 20000.0f;
        }

        PredictCtx Ctx;
        Ctx.StartPos = playerPosition;
        Ctx.TargetPos = targetPosition;
        Ctx.BulletSpeed = (float)(BulletSpeed - (BulletSpeed*0.08));
        Ctx.BulletGravity = (float)(BulletGrav + (BulletGrav*0.05));

        // 计算初始距离和时间
        float initialDistance = playerPosition.Distance(targetPosition);
        float initialBulletTravelTime = initialDistance / Ctx.BulletSpeed;

        // 根据距离调整速度
        //float distanceScale = std::min(initialDistance / 100.0f, 3.0f); // 将距离转换为米，并限制最大缩放
        Vector3D scaledVelocity = Vector3D(targetVelocity.x, targetVelocity.y, targetVelocity.z);
        //Ctx.TargetVel = Vector3D(targetVelocity.x, targetVelocity.y + (targetVelocity.x * targetVelocity.x + targetVelocity.y * targetVelocity.y + targetVelocity.z * targetVelocity.z)/144, targetVelocity.z);

        // 初始位置预测
        Vector3D initialFuturePosition = targetPosition + scaledVelocity * initialBulletTravelTime;

        // 精细化的时间和位置预测
        float refinedDistance = playerPosition.Distance(initialFuturePosition);
        float refinedBulletTravelTime = refinedDistance / Ctx.BulletSpeed;
        Vector3D finalPredictedPosition = targetPosition + scaledVelocity * refinedBulletTravelTime;

        // 弹道下落预测
        float drop = Resolver::GetBasicBulletDrop(playerPosition, finalPredictedPosition, BulletSpeed, BulletGrav);
        finalPredictedPosition.z += drop;

        CalculatedAngles = Angles{Ctx.AimAngles.x, Ctx.AimAngles.y, 0.f};
        Ctx.TargetPos = finalPredictedPosition;
        if (CalculatedAngles == Angles(0, 0, 0))
            CalculatedAngles = Math::CalcAngle(playerPosition, targetPosition);
        SwayAngles = mem.Read<Angles>(Myself->BasePointer + OFFSET_BREATH_ANGLES);

        Vector3D target_origin = targetPosition;

        /*Vector3D view_offset = mem.Read<Vector3D>(Myself->BasePointer + OFFSET_VIEW_OFFSET);//用分散读取替代
        Vector3D view_origin = Myself->CameraPosition + view_offset;*/
        auto ViewAngles = mem.Read<Angles>(Myself->BasePointer + OFFSET_VIEWANGLES);


//        CalculatedAngles-=SwayAngles-ViewAngles;

        Math::NormalizeAngles(CalculatedAngles);
        Angles Delta = CalculatedAngles - ViewAngles;
        Math::NormalizeAngles(Delta);

        /*if (Myself->IsHoldingGrenade) {
            vector2d_t skynade_angles =
                    skynade_angle(Myself->WeaponIndex, Myself->weapon_mod_bitfield, BulletGrav,
                                  BulletSpeed, view_origin.x, view_origin.y, view_origin.z,
                                  target_origin.x, target_origin.y, target_origin.z);
            const float PIS_IN_180 = 57.2957795130823208767981548141051703f;
            Angles TargetAngles = Angles(-skynade_angles.x * PIS_IN_180, skynade_angles.y * PIS_IN_180, 0);
            Delta = TargetAngles - ViewAngles;

            SkynadeAngles = ViewAngles + Delta / 20;
            SmoothedAngles = SkynadeAngles;
            finalPredictedPosition = {SkynadeAngles.x,SkynadeAngles.y,SkynadeAngles.z};
        }*/

        float baseSmoothing = ((mem.GetKeyboard()->IsKeyDown(AimFlickKey) && !InputModel) || (mem.isPressed(inputFlickKey) && InputModel)) ? Flicksmooth : Myself->IsZooming ? ZoomSmooth : Smooth;
            SmoothedAngles = ViewAngles + Delta / baseSmoothing;

        return finalPredictedPosition;
    }

    bool SKYValid(Player* target) {
        if (target == nullptr ||
            !target->IsValid() ||
            !target->IsCombatReady() ||
//            /*!config.自瞄队友 &&*/ !target->IsHostile
            (!target->IsHostile && !Aimbot::AimTeam))
            return false;
        return true;
    }

    void SkyNadeRun(){
        bool enable = ((mem.GetKeyboard()->IsKeyDown(ThrowKey) && !InputModel) || (mem.isPressed(inputThrowKey) && InputModel)) && Myself->IsHoldingGrenade;
        if (enable && Myself->IsCombatReady())
        {
            SkyGET();
            return;
        }

        SKYTargetSelected = false;
        SKYTargetGet = nullptr;
    }

    void SkyGET() {
        Player* Target = SKYTargetGet;

        if (!SKYValid(Target)) {

            if (SKYTargetSelected) {
                return;
            }

            Target = SKYTarget();

            if (!SKYValid(Target)) {
                SKYTargetSelected = false;
                SKYTargetGet = nullptr;
                return;
            }
            SKYTargetGet = Target;
            SKYTargetSelected = true;
        }


        if (SKYTargetSelected&& SKYTargetGet) {
            SKYAIM(SKYTargetGet);
            return;
        }

    }



    void SKYAIM(Player* Target) {
        float dx, dy;
        Vector3D view_origin = Myself->LocalOrigin + Myself->view_offset;

        //printf("%d \n", Myself->WeaponIndex);
        vector2d_t skynade_angles =
                skynade_angle(Myself->WeaponIndex, Myself->weapon_mod_bitfield, Myself->WeaponProjectileScale ,
                              Myself->WeaponProjectileSpeed, view_origin.x, view_origin.y, view_origin.z,
                              Target->LocalOrigin.x, Target->LocalOrigin.y, Target->LocalOrigin.z);


        const float PIS_IN_180 = 57.2957795130823208767981548141051703f;
      
        if (skynade_angles.x == 0) return;
        


        Angles TargetAngles = Angles(-skynade_angles.x * PIS_IN_180, skynade_angles.y * PIS_IN_180, 0);
        if (std::abs(TargetAngles.y - Myself->ViewAngles.y) > 180) {
            if (TargetAngles.y > 0) {
                TargetAngles.y -= 360;
            } else {
                TargetAngles.y += 360;
            }
        }
        QAngle Delta = {TargetAngles.x - Myself->ViewAngles.x, TargetAngles.y - Myself->ViewAngles.y};
        Vector2D step = Vector2D(Delta.x * 2 / SkySmooth, Delta.y * 2 / SkySmooth);
        SkyPoint = Vector3D(-Delta.y,Delta.x, 0);

        //printf("%f %f\n", SkyPoint.x, SkyPoint.y);

        dx = -step.y;
        dy = step.x;

        dx += addx;
        dy += addy;
        const int mdx = static_cast<int>(dx);
        const int mdy = static_cast<int>(dy);
        addx = dx - static_cast<float>(mdx);
        addy = dy - static_cast<float>(mdy);
        if (mdx != 0 || mdy != 0) {
            std::chrono::steady_clock::time_point Aim_new_time = std::chrono::steady_clock::now();
            std::chrono::duration<double> Date_time = Aim_new_time - Aim_old_time;

            if (Date_time.count() < aim_time)
            {
                return;
            }

            Move(mdx, mdy);

            Aim_new_time = std::chrono::steady_clock::now();
            Aim_old_time = Aim_new_time;
        }


    }

    [[nodiscard]] Player * SKYTarget() {
        Player* NearestTarget = nullptr;
        float BestFOV = 150;
        float LastPov = 9999;
        //float LastPov2 = 9999;
        for (Player* p : *Players){
            if (!SKYValid(p)) continue;
            if (p->DistanceToLocalPlayer >= Conversion::ToGameUnits(150)) continue; // 提前继续，减少嵌套

            Vector3D TargetPosition = p->GetBonePosition(HitboxType::UpperChest);
            Vector2D pos2D/*, grdpos2d*/;
            GameCamera->WorldToScreen(TargetPosition, pos2D);
            float aimDis2d = pos2D.Distance(GameCamera->ScreenCenter);//计算骨骼点距离到屏幕中心的距离

            /*Vector3D GrdPosition = Getnade(p);//该功能是绘制高抛的点
            if (GrdPosition.x != 0)
                GameCamera->WorldToScreen(GrdPosition, grdpos2d);
            float grdDis2d = grdpos2d.Distance(GameCamera->ScreenCenter);*/

            //if (grdpos2d.x == 0) {
                if (aimDis2d > BestFOV) continue;
                if (aimDis2d > LastPov) continue;
                NearestTarget = p;
                LastPov = aimDis2d;

          //  }
            /*else
            {
                if (aimDis2d > BestFOV && grdDis2d > 50.0f) continue;
                if (aimDis2d > LastPov && grdDis2d > LastPov2) continue;
                NearestTarget = p;
                LastPov = grdDis2d;
            }*/


        }
        return NearestTarget;
    }

    static float to_degrees(float a)
    {
        const float PIS_IN_180 = 57.2957795130823208767981548141051703f;
        return a * PIS_IN_180;
    }

//    Vector3D GetSkynade3D(Player* target)
//    {
//        float a_off = 投掷物.arc ? 投掷物.arc_off : 投掷物.lowarc_off;//电弧星
//        float t_off = 投掷物.thermite ? 投掷物.thermite_off : 投掷物.lowthermite_off;//铝热剂
//        float f_off = 投掷物.frag ? 投掷物.frag_off : 投掷物.lowfrag_off;//破片
//
//        Vector3D view_origin = Myself->LocalOrigin + Myself->view_offset;
//
//        vector2d_t skynade_angles =
//                skynade_angle(Myself->WeaponIndex, Myself->weapon_mod_bitfield, Myself->WeaponProjectileScale,
//                              Myself->WeaponProjectileSpeed, view_origin.x, view_origin.y, view_origin.z,
//                              target->LocalOrigin.x, target->LocalOrigin.y, target->LocalOrigin.z, 投掷物.thermite, 投掷物.frag, 投掷物.arc, t_off, f_off, a_off);
//
//        if (skynade_angles.x == 0) return Vector3D(0, 0, 0);
//        Angles skynade_angles_to_screen = angles_to_screen(Myself->CameraPosition.x, Myself->CameraPosition.y, Myself->CameraPosition.z,
//                                                           -to_degrees(skynade_angles.x), to_degrees(skynade_angles.y), 0.0);
//        return Vector3D(skynade_angles_to_screen.x, skynade_angles_to_screen.y, skynade_angles_to_screen.z);
//    }

};