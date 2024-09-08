#include <iostream>
#include <thread>
#include <unordered_set>
#include "Memory/Memory.h"
#include "game/Level.hpp"
#include "game/Player.hpp"
#include "game/LocalPlayer.hpp"
#include "game/Camera.hpp"
#include "game/Glow.hpp"
#include "game/Aimbot.hpp"
#include "game/Config.hpp"
#include "Spectator.hpp"
#include "game/Misc.hpp"
#include "game/Render.hpp"
#include "Game_struct.h"

#include <curl/curl.h>
#pragma execution_character_set("utf-8")

#include "ReadList.hpp"

// 回调函数，用于从libcurl接收数据
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t realSize = size * nmemb;
    userp->append((char*)contents, realSize);
    return realSize;
}

// 构建请求URL
std::string buildUrl(const std::string& baseUrl, const std::string& auth, const std::string& uid, const std::string& platform) {
    std::ostringstream oss;
    oss << baseUrl << "?auth=" << auth << "&uid=" << uid << "&platform=" << platform;
    return oss.str();
}

// 执行HTTP GET请求并返回响应
std::string httpGetRequest(const std::string& baseUrl, const std::string& auth, const std::string& uid, const std::string& platform) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer; // 用于存储响应数据
    std::string url = buildUrl(baseUrl, auth, uid, platform);

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;

        curl_easy_cleanup(curl);
    }
    return readBuffer; // 返回响应数据
}

std::string APIGetPlayerInfo(long long int ID) {

    std::string baseUrl = "https://api.mozambiquehe.re/bridge";
    std::string auth = "5f0f33fa9156c9301f13016004fe0c28";
    std::string uid = std::to_string(ID).c_str();
    std::string platform = "PC";
    std::string response = httpGetRequest(baseUrl, auth, uid, platform);
    return response;
}

// Game Objects
Level* Map = new Level();
LocalPlayer* Myself = new LocalPlayer();
Camera* GameCamera = new Camera();

// Players
std::vector<Player*>* HumanPlayers = new std::vector<Player*>;
std::vector<Player*>* Dummies = new std::vector<Player*>;
std::vector<Player*>* Players = new std::vector<Player*>;

//  
Sense* ESP = new Sense(Players, GameCamera, Myself);
Aimbot* AimAssist = new Aimbot(Myself, Players, GameCamera);
Spectator* Spectators = new Spectator(Players, Myself);
Misc* Miscellanous = new Misc(Myself);

std::string strstrstr(const std::string& str, const std::string& front, const std::string& rear) {
    if (str.empty() || front.empty() || rear.empty())
        return "";

    size_t frontPos = str.find(front);
    if (frontPos == std::string::npos) {
        // 前缀未找到
        return "ERROR";
    }

    // 调整起始位置，开始搜索后缀
    frontPos += front.length();
    size_t rearPos = str.find(rear, frontPos);
    if (rearPos == std::string::npos) {
        // 后缀未找到
        return "ERROR";
    }

    // 提取并返回两字符串之间的内容
    return str.substr(frontPos, rearPos - frontPos);
}

void ThreadRankLevel() {
    while (Map->IsPlayable) {

        for (int i = 0; i < Players->size(); i++) {
            Player* player = Players->at(i); // 获取当前玩家对象
            // 检查player是否为nullptr
            if (player == nullptr) continue;
            if (player->UserId == 0) continue;
            if (player->PlayerInfo == "")
            {
                player->PlayerInfo = APIGetPlayerInfo(player->UserId);
            }
        }
    }
}

void MiscBaseScatter(Level* map, LocalPlayer* myself, Camera* gameCamera, Sense* esp) {
    // 创建分散读写句柄
    auto handle = mem.CreateScatterHandle();

    //input system 按键处理
    uint64_t buttonAddress = mem.OFF_BASE + OFFSET_INPUT_SYSTEM + 0xb0;
    mem.AddScatterReadRequest(handle, buttonAddress, &mem.InputButton, sizeof(Memory::InputButton));

    // 分散读地图名称
    uint64_t levelAddress = mem.OFF_BASE + OFF_LEVEL;
    mem.AddScatterReadRequest(handle, levelAddress, &map->NameBuffer, sizeof(map->NameBuffer));

    //游戏模式
    uint64_t gameModeAddress = mem.OFF_BASE + OFF_GAMEMODE;
    mem.AddScatterReadRequest(handle, gameModeAddress, &map->gameModePtr, sizeof(uint64_t));

    // 分散读本人地址
    uint64_t localPlayerAddress = mem.OFF_BASE + OFF_LOCAL_PLAYER;
    mem.AddScatterReadRequest(handle, localPlayerAddress, &myself->BasePointer, sizeof(uint64_t));

    // 分散读本人是否处于攻击状态
    uint64_t inAttackAddress = mem.OFF_BASE + OFF_INATTACK;
    mem.AddScatterReadRequest(handle, inAttackAddress, &myself->IsInAttack, sizeof(uint32_t));

    // 游戏相机分散读取请求
    uint64_t cameraRenderPointerAddress = mem.OFF_BASE + OFF_VIEWRENDER;
    mem.AddScatterReadRequest(handle, cameraRenderPointerAddress, &gameCamera->RenderPointer, sizeof(uint64_t));

    // 高亮显示设置指针的分散读取请求
    uint64_t highlightSettingsPointerAddress = mem.OFF_BASE + OFF_GLOW_HIGHLIGHTS;
    mem.AddScatterReadRequest(handle, highlightSettingsPointerAddress, &esp->HighlightSettingsPointer, sizeof(uint64_t));

    if (myself->BasePointer) {
        mem.AddScatterReadRequest(handle, myself->BasePointer + OFFSET_GRAPPLEACTIVED, &Myself->isGrppleActived, sizeof(int));
    }

    // 高亮显示设置指针的分散读取请求
    uint64_t isGRAPPLEAddress = mem.OFF_BASE + OFF_LOCAL_PLAYER + OFFSET_GRAPPLE + OFFSET_GRAPPLEATTACHED;
    mem.AddScatterReadRequest(handle, isGRAPPLEAddress, &Myself->isGRAPPLE, sizeof(uint64_t));

    // 执行分散读取
    mem.ExecuteReadScatter(handle);

    // 关闭分散读取句柄
    mem.CloseScatterHandle(handle);
}

//读取玩家BasePointer
void PlayerBasePointerScatter(std::vector<Player*>& players) {
    // Create scatter handle
    auto handle = mem.CreateScatterHandle();

    for (size_t i = 0; i < players.size(); ++i) {
        int index = players[i]->Index;
        uint64_t address = mem.OFF_BASE + OFF_ENTITY_LIST + ((static_cast<unsigned long long>(index) + 1) << 5);
        mem.AddScatterReadRequest(handle, address, &players[i]->BasePointer, sizeof(uint64_t));
    }

    // Execute the scatter read
    mem.ExecuteReadScatter(handle);

    // Close the scatter handle
    mem.CloseScatterHandle(handle);
}

//读取玩家队标和名字
void ScatterReadTeamAndName(std::vector<Player*>& players) {
    // 创建分散读写句柄
    auto handle = mem.CreateScatterHandle();

    for (size_t i = 0; i < players.size(); ++i) {
        Player* player = players[i];

        // 在添加分散读取请求之前，判断玩家地址指针是否为0
        if (mem.IsValidPointer(player->BasePointer)) {
            // 分散读取名称缓冲区请求
            uint64_t nameBufferAddress = player->BasePointer + OFF_NAME;
            mem.AddScatterReadRequest(handle, nameBufferAddress, player->NameBuffer, sizeof(player->NameBuffer));

            // 分散读取名称缓冲区请求
            uint64_t mp_xAdress = player->BasePointer + OFF_XP;
            mem.AddScatterReadRequest(handle, mp_xAdress, &player->m_xp, sizeof(int));

            // 分散读队伍请求
            uint64_t teamAddress = player->BasePointer + OFF_TEAM_NUMBER;
            mem.AddScatterReadRequest(handle, teamAddress, &player->Team, sizeof(int));

            mem.AddScatterReadRequest(handle, mem.OFF_BASE + OFF_NAME_LIST + (player->Index) * 24, &player->NameList, sizeof(uint64_t));

            mem.AddScatterReadRequest(handle, player->BasePointer + OFFSET_UID, &player->UserId, sizeof(long long int));
        }
    }
    // 执行分散读取
    mem.ExecuteReadScatter(handle);

    // 将NameBuffer转换为Name字段的std::string
    for (size_t i = 0; i < players.size(); ++i) {
        Player* player = players[i];
        //判断玩家地址指针是否为0
        if (player->BasePointer != 0) {
            // 将NameBuffer转换为Name
            mem.AddScatterReadRequest(handle, player->NameList, player->PlayerName, 32);
            player->Name = std::string(player->NameBuffer);
        }
    }
    mem.ExecuteReadScatter(handle);
    // 关闭分散读取句柄
    mem.CloseScatterHandle(handle);
}

void ScatterReadPlayerValidity(std::vector <Player*>& players) {
	// Create scatter handle
	auto handle = mem.CreateScatterHandle();

	for (size_t i = 0; i < players.size(); ++i) {
		Player* player = players[i];

		// Verify that the BasePointer is not 0 before adding scatter read requests
		if (mem.IsValidPointer(player->BasePointer)) {
			// Scatter read request for IsPlayer
			uint64_t validAddress = player->BasePointer;
			mem.AddScatterReadRequest(handle, validAddress, &player->Valid, sizeof(uint64_t));
		}
	}

	// Execute the scatter read
	mem.ExecuteReadScatter(handle);

	// Close the scatter handle
	mem.CloseScatterHandle(handle);

    for (int i = 0; i < Players->size(); i++) {
        Player* p = Players->at(i);
        p->ValidCheck();
    }
}

void ScatterReadPlayerAttributes(std::vector<Player*>& players) {
    // 创建散射控制柄
    auto handle = mem.CreateScatterHandle();

    for (size_t i = 0; i < players.size(); ++i) {
        //std::cout << "players.size()->" << players.size() << std::endl;
        Player* player = players[i];
        //检查玩家以及训练场
        if (mem.IsValidPointer(player->BasePointer) && !player->IsPlayer() && !Map->IsFiringRange) {
            player->BasePointer = 0;
            continue;
        }

        // 在读之前，验证BasePointer是否为0
        if (mem.IsValidPointer(player->BasePointer)) {
            if (player->IsPlayer()) {
                // 分散读取是否死亡
                uint64_t isDeadAddress = player->BasePointer + OFF_LIFE_STATE;
                mem.AddScatterReadRequest(handle, isDeadAddress, &player->IsDead, sizeof(bool));

                // 分散读取是否倒地
                uint64_t isKnockedAddress = player->BasePointer + OFF_BLEEDOUT_STATE;
                mem.AddScatterReadRequest(handle, isKnockedAddress, &player->IsKnocked, sizeof(bool));

                // 分散读取VectorQuantity
//                uint64_t absoluteVelocityAddress = player->BasePointer + OFF_ABSVELOCITY;
//                mem.AddScatterReadRequest(handle, absoluteVelocityAddress, &player->AbsoluteVelocity, sizeof(Vector3D));

                // 分散读取上色所需
                uint64_t glowEnableAddress = player->BasePointer + OFF_GLOW_ENABLE;
                uint64_t glowThroughWallAddress = player->BasePointer + OFF_GLOW_THROUGH_WALL;
                uint64_t highlightIDAddress = player->BasePointer + OFF_GLOW_HIGHLIGHT_ID + 1;
                mem.AddScatterReadRequest(handle, glowEnableAddress, &player->GlowEnable, sizeof(int));
                mem.AddScatterReadRequest(handle, glowThroughWallAddress, &player->GlowThroughWall, sizeof(int));
                mem.AddScatterReadRequest(handle, highlightIDAddress, &player->HighlightID, sizeof(int));

                // 分散读取上次看到时间与上次消失时间
                uint64_t lastTimeAimedAtAddress = player->BasePointer + OFF_LAST_AIMEDAT_TIME;
                uint64_t lastVisibleTimeAddress = player->BasePointer + OFF_LAST_VISIBLE_TIME;
                uint64_t worldTimeAddress = Myself->BasePointer + OFF_TIME_BASE;
                mem.AddScatterReadRequest(handle, lastTimeAimedAtAddress, &player->LastTimeAimedAt, sizeof(int));
                mem.AddScatterReadRequest(handle, lastVisibleTimeAddress, &player->LastVisibleTime, sizeof(float));
                mem.AddScatterReadRequest(handle, worldTimeAddress, &player->WorldTime, sizeof(float));

                // 分散读取玩家Yaw
                uint64_t viewYawAddress = player->BasePointer + OFF_YAW;
                mem.AddScatterReadRequest(handle, viewYawAddress, &player->ViewYaw, sizeof(float));

            }
            else {
                player->IsDead = false;
                player->IsKnocked = false;
            }
            //如果是虚拟玩家并且在训练场  或者玩家是真实玩家
            if (player->IsDummy() && Map->IsFiringRange || player->IsPlayer()) {
                // 分散读取玩家的绝对coord
                uint64_t localOriginAddress = player->BasePointer + OFF_LOCAL_ORIGIN;
                mem.AddScatterReadRequest(handle, localOriginAddress, &player->LocalOrigin, sizeof(Vector3D));

                // Scatter read request for AbsoluteVelocity
                uint64_t absoluteVelocityAddress = player->BasePointer + OFF_ABSVELOCITY;
                mem.AddScatterReadRequest(handle, absoluteVelocityAddress, &player->AbsoluteVelocity, sizeof(Vector3D));

                // 分散读取玩家的血量值
                uint64_t healthAddress = player->BasePointer + OFF_HEALTH;
                mem.AddScatterReadRequest(handle, healthAddress, &player->health, sizeof(int));

                // 分散读取玩家的护盾
                uint64_t ShieldAddress = player->BasePointer + OFF_SHIELD;
                mem.AddScatterReadRequest(handle, ShieldAddress, &player->shield, sizeof(int));

                // 分散读取玩家的最大护盾
                uint64_t MAXShieldAddress = player->BasePointer + OFF_MAXSHIELD;
                mem.AddScatterReadRequest(handle, MAXShieldAddress, &player->max_shield, sizeof(int));

                // 分散读取玩家的模型指针
                uint64_t modelPointerAddress = player->BasePointer + OFF_STUDIOHDR;
                mem.AddScatterReadRequest(handle, modelPointerAddress, &player->ModelPointer, sizeof(uint64_t));

                // 分散读取玩家的骨骼指针
                uint64_t bonePointerAddress = player->BasePointer + OFF_BONES;
                mem.AddScatterReadRequest(handle, bonePointerAddress, &player->BonePointer, sizeof(uint64_t));
            }
        }
    }
    mem.ExecuteReadScatter(handle);
    // 关闭句柄
    mem.CloseScatterHandle(handle);
}

inline int CalculateDistance(Vector3D LocalCoord, Vector3D EntityCoord) {
    return (int)(sqrt((LocalCoord.x - EntityCoord.x) * (LocalCoord.x - EntityCoord.x) + (LocalCoord.y - EntityCoord.y) * (LocalCoord.y - EntityCoord.y) + (LocalCoord.z - EntityCoord.z) * (LocalCoord.z - EntityCoord.z))) * 0.025;
}

inline int GetPlayerLevel(int m_xp)
{
    //int m_xp = mem.Read<int>(playerEntity + OFF_XP); //m_xp
    if (m_xp < 0) return 0;
    if (m_xp < 100) return 1;
    if (m_xp < 2750) return 2;
    if (m_xp < 6650) return 3;
    if (m_xp < 11400) return 4;
    if (m_xp < 17000) return 5;
    if (m_xp < 23350) return 6;
    if (m_xp < 30450) return 7;
    if (m_xp < 38300) return 8;
    if (m_xp < 46450) return 9;
    if (m_xp < 55050) return 10;
    if (m_xp < 64100) return 11;
    if (m_xp < 73600) return 12;
    if (m_xp < 83550) return 13;
    if (m_xp < 93950) return 14;
    if (m_xp < 104800) return 15;
    if (m_xp < 116100) return 16;
    if (m_xp < 127850) return 17;
    if (m_xp < 140050) return 18;
    if (m_xp < 152400) return 19;
    if (m_xp < 164900) return 20;
    if (m_xp < 177550) return 21;
    if (m_xp < 190350) return 22;
    if (m_xp < 203300) return 23;
    if (m_xp < 216400) return 24;
    if (m_xp < 229650) return 25;
    if (m_xp < 243050) return 26;
    if (m_xp < 256600) return 27;
    if (m_xp < 270300) return 28;
    if (m_xp < 284150) return 29;
    if (m_xp < 298150) return 30;
    if (m_xp < 312300) return 31;
    if (m_xp < 326600) return 32;
    if (m_xp < 341050) return 33;
    if (m_xp < 355650) return 34;
    if (m_xp < 370400) return 35;
    if (m_xp < 385300) return 36;
    if (m_xp < 400350) return 37;
    if (m_xp < 415550) return 38;
    if (m_xp < 430900) return 39;
    if (m_xp < 446400) return 40;
    if (m_xp < 462050) return 41;
    if (m_xp < 477850) return 42;
    if (m_xp < 493800) return 43;
    if (m_xp < 509900) return 44;
    if (m_xp < 526150) return 45;
    if (m_xp < 542550) return 46;
    if (m_xp < 559100) return 47;
    if (m_xp < 575800) return 48;
    if (m_xp < 592650) return 49;
    if (m_xp < 609650) return 50;
    if (m_xp < 626800) return 51;
    if (m_xp < 644100) return 52;
    if (m_xp < 661550) return 53;
    if (m_xp < 679150) return 54;
    if (m_xp < 696900) return 55;
    if (m_xp < 714800) return 56;
    return static_cast<int>(floor((m_xp - 714800 + 1) / 18000) + 57);
}

void UpdateCore() {
    try {
        while (true) {
            // Initial Misc Reads //
            MiscBaseScatter(Map, Myself, GameCamera, ESP);

            // Map Checking //
            Map->Read();
            if (!Map->IsPlayable) {
                Players->clear();
                PlayersPopulated = false;
                continue;
            }

            // Local Player Update //
            //读本人的武器之类的信息
            Myself->Read();

            if (!Myself->IsValid()) {
                Players->clear();
                PlayersPopulated = false;
                continue;
            }

            //判断自己的位置是否有效
            if (!Myself->ValidPosition()) {
				Players->clear();
                PlayersPopulated = false;
				continue;
			}

            // Populate Players //
            // 假人修复
            if (Map->IsFiringRange) {
                static auto lastExecTime = std::chrono::steady_clock::now() - std::chrono::seconds(5); // Subtract to ensure it runs the first time
                // Check if 5 seconds have passed since the last execution
                auto currentTime = std::chrono::steady_clock::now();
                if (currentTime - lastExecTime >= std::chrono::seconds(5)) {
                    PlayersPopulated = false;
                    Players->clear();
                    lastExecTime = currentTime;
                }
            }

            if (!PlayersPopulated) {
                // Call the scatter function to read BasePointers, Team and Name for all players
                if (Map->IsFiringRange) {
                    PlayerBasePointerScatter(*Dummies);
                    ScatterReadTeamAndName(*Dummies);
                    PlayerBasePointerScatter(*HumanPlayers);
                    ScatterReadTeamAndName(*HumanPlayers);
                }
                else {
                    PlayerBasePointerScatter(*HumanPlayers);
                    ScatterReadTeamAndName(*HumanPlayers);
                }

                if (Map->IsFiringRange) {
                    for (int i = 0; i < Dummies->size(); i++) {
                        Player* p = Dummies->at(i);
                        if (p->BasePointer != 0 && (p->IsPlayer() || p->IsDummy()))
                            Players->push_back(p);
                    }
                    for (int i = 0; i < HumanPlayers->size(); i++) {
                        Player* p = HumanPlayers->at(i);
                        if (p->BasePointer != 0 && (p->IsPlayer() || p->IsDummy()))
                            Players->push_back(p);
                    }
                }
                else {
                    for (int i = 0; i < HumanPlayers->size(); i++) {
                        Player* p = HumanPlayers->at(i);
                        if (p->BasePointer != 0 && (p->IsPlayer() || p->IsDummy()))
                            Players->push_back(p);
                    }
                }
                std::cout << "PLAYER INFO READY" << std::endl;
                PlayersPopulated = true;
            }

            // Updates //
            // Update Player Validity
            ScatterReadPlayerValidity(*Players);

            // Update Player Attributes
            ScatterReadPlayerAttributes(*Players);

            // Update Camera
            GameCamera->Update();
            GameCamera->LocalCamera = Myself->CameraPosition;

            // 清空Temporaryarray，准备存放新的数据
            Temporaryarray.clear();
            // Update Players
            for (int i = 0; i < Players->size(); i++) {
                Player* p = Players->at(i); // 获取当前玩家对象
                p->Read(); // 读取当前玩家的信息

                Infoarray Temporaryinfo; // 创建一个新的Temporaryinfo对象，用于存放玩家数据

//                float distance = CalculateDistance(GameCamera->LocalCamera, p->LocalOrigin);

//                if (v.skeleton && distance < BoneFreshDistance) {
//                    Temporaryinfo.BoneHead = p->GetBonePosition(HitboxType::Head);
//                    Temporaryinfo.BoneNeck = p->GetBonePosition(HitboxType::Neck);
//                    Temporaryinfo.BoneUpperChest = p->GetBonePosition(HitboxType::UpperChest);
//                    Temporaryinfo.BoneChest = p->GetBonePosition(HitboxType::Chest);
//                    Temporaryinfo.BoneHip = p->GetBonePosition(HitboxType::Hip);
//                    Temporaryinfo.BoneMidSpine = p->GetBonePosition(HitboxType::MidSpine);
//                    Temporaryinfo.BoneLowerSpine = p->GetBonePosition(HitboxType::LowerSpine);
//                    Temporaryinfo.BonePropGun = p->GetBonePosition(HitboxType::PropGun);
//                    Temporaryinfo.BoneNeckA = p->GetBonePosition(HitboxType::NeckA);
//                    Temporaryinfo.BoneNeckB = p->GetBonePosition(HitboxType::NeckB);
//                    Temporaryinfo.BoneNeckC = p->GetBonePosition(HitboxType::NeckC);
//                    Temporaryinfo.BoneHeadB = p->GetBonePosition(HitboxType::HeadB);
//                    Temporaryinfo.BoneLeftClavicle = p->GetBonePosition(HitboxType::LeftClavicle);
//                    Temporaryinfo.BoneLeftShoulder = p->GetBonePosition(HitboxType::LeftShoulder);
//                    Temporaryinfo.BoneLeftShoulderMid = p->GetBonePosition(HitboxType::LeftShoulderMid);
//                    Temporaryinfo.BoneLeftElbowB = p->GetBonePosition(HitboxType::LeftElbowB);
//                    Temporaryinfo.BoneLeftElbow = p->GetBonePosition(HitboxType::LeftElbow);
//                }

                if (v.skeleton && CalculateDistance(GameCamera->LocalCamera, p->LocalOrigin) < BoneFreshDistance) {
                Temporaryinfo.BoneAddress = p->BonePointer; //骨骼地址
                Temporaryinfo.ModelAddress = p->ModelPointer; //模型地址
                }

                Temporaryinfo.coord = p->LocalOrigin; // 将玩家的本地coord赋值给Temporaryinfo

                Temporaryinfo.VectorQuantity = p->AbsoluteVelocity;
                Temporaryinfo.PtrAddress = p->BasePointer;
                Temporaryinfo.Name = p->PlayerName;
                Temporaryinfo.Yaw = p->ViewYaw;
                Temporaryinfo.Distance = p->DistanceToLocalPlayer;
                Temporaryinfo.Distance2D = p->Distance2DToLocalPlayer;
                Temporaryinfo.Index = p->Index;
                Temporaryinfo.Team = p->Team;
                Temporaryinfo.Health = p->health;
                Temporaryinfo.Sheld = p->shield;
                Temporaryinfo.MaxSheld = p->max_shield;
                Temporaryinfo.Dead = p->IsDead;
                Temporaryinfo.Knocked = p->IsKnocked;
                Temporaryinfo.Visible = p->IsVisible;
                Temporaryinfo.LocalPlayer = p->IsLocal;
                Temporaryinfo.Teammates = p->IsAlly;
                Temporaryinfo.Enemy = p->IsHostile;
                Temporaryinfo.Lock = p->IsLockedOn;
                Temporaryinfo.ID = p->UserId;
                Temporaryinfo.Knocked = p->IsKnocked;
                //Temporaryinfo.PlayerLV = GetPlayerLevel(Temporaryinfo.PtrAddress);
                Temporaryinfo.PlayerLV = GetPlayerLevel(p->m_xp);
                //std::cout << "m_xp" << p->m_xp << std::endl;
//                std::cout << "LastVisibleTime" << p->LastVisibleTime << std::endl;
//                std::cout << "WorldTime" << p->WorldTime << std::endl;

//                Temporaryinfo.PlayerLevel = strstrstr(p->PlayerInfo, "\"level\"\:", "\,\"");
//                Temporaryinfo.LevelPrestige = strstrstr(p->PlayerInfo, "\"levelPrestige\"\:", "\,\"");

//                if (Temporaryinfo.PlayerLevel != "")
//                {
//                    Temporaryinfo.PlayerLevel += " ";
//                }

                Temporaryinfo.PlayerRank = strstrstr(p->PlayerInfo, "\"rankName\"\:\"", "\"\,\"");

                if (Temporaryinfo.PlayerRank == "Rookie")
                {
                    Temporaryinfo.PlayerRank = "[菜鸟]";
                    Temporaryinfo.PlayerRank += strstrstr(p->PlayerInfo, "\"rankDiv\"\:", "\,\"");
                }
                if (Temporaryinfo.PlayerRank == "Bronze")
                {
                    Temporaryinfo.PlayerRank = "[青铜]";
                    Temporaryinfo.PlayerRank += strstrstr(p->PlayerInfo, "\"rankDiv\"\:", "\,\"");
                }
                if (Temporaryinfo.PlayerRank == "Silver")
                {
                    Temporaryinfo.PlayerRank = "[白银]";
                    Temporaryinfo.PlayerRank += strstrstr(p->PlayerInfo, "\"rankDiv\"\:", "\,\"");
                }
                if (Temporaryinfo.PlayerRank == "Gold")
                {
                    Temporaryinfo.PlayerRank = "[黄金]";
                    Temporaryinfo.PlayerRank += strstrstr(p->PlayerInfo, "\"rankDiv\"\:", "\,\"");
                }
                if (Temporaryinfo.PlayerRank == "Platinum")
                {
                    Temporaryinfo.PlayerRank = "[铂金]";
                    Temporaryinfo.PlayerRank += strstrstr(p->PlayerInfo, "\"rankDiv\"\:", "\,\"");
                }
                if (Temporaryinfo.PlayerRank == "Diamond")
                {
                    Temporaryinfo.PlayerRank = "[钻石]";
                    Temporaryinfo.PlayerRank += strstrstr(p->PlayerInfo, "\"rankDiv\"\:", "\,\"");
                }

                if (Temporaryinfo.PlayerRank == "Apex Predator")
                {
                    Temporaryinfo.PlayerRank = "[猎杀]#";
                    Temporaryinfo.PlayerRank += strstrstr(p->PlayerInfo, "\"ladderPosPlatform\"\:", "\,\"");
                }
                if (Temporaryinfo.PlayerRank == "Master")
                {
                    Temporaryinfo.PlayerRank = "[大师]#";
                    Temporaryinfo.PlayerRank += strstrstr(p->PlayerInfo, "\"rankScore\"\:", "\,\"");
                    Temporaryinfo.PlayerRank += "分";
                }
                Temporaryinfo.Admin = RList.IsAdmin(p->UserId);
                Temporaryinfo.White = RList.IsWhite(p->UserId);
                Temporaryinfo.Black = RList.IsBlack(p->UserId);

//                Temporaryinfo.Admin = p->IsAdmin;
//                Temporaryinfo.White = p->IsWhite;
//                Temporaryinfo.Black = p->IsBlack;

                Temporaryarray.push_back(Temporaryinfo);// 将Temporaryinfo添加到Temporaryarray中
            }
            Globalarray = Temporaryarray;
            // Update ESP
            ESP->Update();

            // Update AimAssist
            AimAssist->Update_Aimbot();
//            AimAssist->Update_Aimbot(AimAssist->AimBotKey2);

            AimAssist->Update_Triggerbot();
            AimAssist->Update_TacticalReload();
            AimAssist->TwoGunBug();

            AimAssist->SkyNadeRun();

            Spectators->SpectatorsUpdate();

            Miscellanous->Update();
        }
    }
    catch (const std::exception& ex) {
        std::cout << "Error: " << ex.what() << std::endl;
        return;
    }
}

//void DataRefresh() {
//    while (true) {
//        VMMDLL_ConfigSet(mem.vHandle, VMMDLL_OPT_REFRESH_FREQ_MEM, 1);//写入 - 刷新内存缓存（不包括 TLB）
//        VMMDLL_ConfigSet(mem.vHandle, VMMDLL_OPT_REFRESH_FREQ_TLB, 1);//写入 - 刷新页表（TLB）缓存
//        Sleep(1000);
//        }
//}

void SetConsoleStyle()
{
    SetConsoleOutputCP(CP_UTF8);

    // 创建一个新的控制台窗口
    AllocConsole();
    HWND consoleWindow = GetConsoleWindow();

    SetConsoleTitle("FATE Console");

    // 设置控制台字体
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = 0;   // 字体宽度
    cfi.dwFontSize.Y = 16;  // 字体高度
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    wcscpy_s(cfi.FaceName, L"Small Fonts"); // 设置字体为微软雅黑

    // 应用字体设置
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    if (!(dwMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING))
    {
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
    }
}

int main(int argc, char* argv[])
{
    SetConsoleStyle();

//  std::cout << "-----------------------------" << std::endl;
//	std::cout << "          FATE APEX          " << std::endl;
//	std::cout << "-----------------------------" << std::endl;
    std::string FATE = R"(
 ********      **      **********   ********         ******    **      **   ********       **      **********
/**/////      ****    /////**///   /**/////         **////**  /**     /**  /**/////       ****    /////**///
/**          **//**       /**      /**             **    //   /**     /**  /**           **//**       /**
/*******    **  //**      /**      /*******       /**         /**********  /*******     **  //**      /**
/**////    **********     /**      /**////        /**         /**//////**  /**////     **********     /**
/**       /**//////**     /**      /**            //**    **  /**     /**  /**        /**//////**     /**
/**       /**     /**     /**      /********       //******   /**     /**  /********  /**     /**     /**
//        //      //      //       ////////         //////    //      //   ////////   //      //      //
)";
    std::string SpiderLogo = R"(
 ███████████   █████████   ███████████ ██████████
░░███░░░░░░█  ███░░░░░███ ░█░░░███░░░█░░███░░░░░█
 ░███   █ ░  ░███    ░███ ░   ░███  ░  ░███  █ ░
 ░███████    ░███████████     ░███     ░██████
 ░███░░░█    ░███░░░░░███     ░███     ░███░░█
 ░███  ░     ░███    ░███     ░███     ░███ ░   █ ░█▀█░█▀█░█▀▀░█░█
 █████       █████   █████    █████    ██████████ ░█▀█░█▀▀░█▀▀░▄▀▄
░░░░░       ░░░░░   ░░░░░    ░░░░░    ░░░░░░░░░░  ░▀░▀░▀░░░▀▀▀░▀░▀
)";
    // 获取控制台句柄
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // 设置文本颜色为绿色，背景颜色为紫色
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);

    // 打印字符串
    std::cout << SpiderLogo << std::endl;

    // 重置颜色为默认值
    SetConsoleTextAttribute(hConsole, 7);

    FATEName = std::filesystem::path(argv[0]).filename().string();

    GetBulletin();

    while (!LoginSuccess && !IsLatestVersion)
    {
        AuthRender();
    }
    //std::thread renderThread(Render, Myself, Players, GameCamera, Spectators, AimAssist, ESP);

    VMMDLL_ConfigSet(mem.vHandle, VMMDLL_OPT_REFRESH_ALL, 1);

	if (!mem.Init("r5apex.exe", true, false))
	{
		std::cout << "[!] DMA初始化失败" << std::endl;
        std::cout << "[!] 请检查是否进入游戏和DMA板子是否连接正常" << std::endl;
        std::cout << "[#] 按回车继续...";
        std::cin.get();
	}
    std::cout << "[#] DMA READY" << std::endl;

    if (!mem.GetKeyboard()->InitKeyboard()) 
    {
        std::cout << "[!] 键盘监听失败，请在菜单中使用Input模式" << std::endl;
        std::cout << "[#] 按回车继续...";
        std::cin.get();
        keyboardstate = false;
    }
    else
    {
        std::cout << "[#] KEYBOARD READY" << std::endl;
        keyboardstate = true;
    }

    try {
        for (int i = 0; i < 100; i++)
            HumanPlayers->push_back(new Player(i, Myself, Map));

        for (int i = 0; i < 15000; i++)
            Dummies->push_back(new Player(i, Myself, Map));

        std::cout << "-----------------------------" << std::endl;

        RList.loadLists();//Read Lists

        Config::GetInstance().Initialize(AimAssist, GameCamera, ESP, Miscellanous);
        std::cout << "[#] 配置文件[config.ini]加载成功" << std::endl;
        std::cout << "-----------------------------" << std::endl;
        GameCamera->Initialize();
        ESP->Initialize();
        AimAssist->Initialize();
        std::cout << "-----------------------------" << std::endl;
        std::cout << "[#] CORE READY" << std::endl;
        std::cout << "-----------------------------" << std::endl;

        // Threads
        std::thread coreThread(UpdateCore);
        std::thread renderThread(Render, Myself, Players, GameCamera, Spectators, AimAssist, ESP);
        std::thread rankThread(ThreadRankLevel);

        coreThread.join();
        //renderThread.join();
        //rankThread.join();
        //dateThread.join();
    }
    catch (...) {}

    std::cout << "[#] PRESS ENTER TO EXIT...";
    std::cin.get();  // Wait for user to press Enter
    return 0;
}

//static auto dump() -> void
//{
//    auto v0 = mem.OFF_BASE + 0x7542B60;
//    auto v1 = *reinterpret_cast<std::uintptr_t*>(v0);
//    auto v2 = *reinterpret_cast<std::uintptr_t*>(*reinterpret_cast<std::uintptr_t*>(v1) + 0x18);
//    auto v3 = *reinterpret_cast<std::uintptr_t*>(*reinterpret_cast<std::uintptr_t*>(v1) + 0x58);
//
//    auto size = reinterpret_cast<std::size_t (*)(std::uintptr_t)>(v2)(v1);
//    auto stream = std::ofstream("weapons.h");
//    if (!stream.good()) return;
//    stream << "enum : int\n";
//    stream << "{\n";
//
//    for (auto i = size_t(0); i < size; i++) stream << "    " << reinterpret_cast<const char* (*)(std::uintptr_t, std::size_t)>(v3)(v1, i) << " = " << std::dec << i << "\n";
//
//    stream << "};\n";
//    stream.flush();
//    stream.close();
//}