// Config.hpp
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "Aimbot.hpp"
#include "Camera.hpp"
#include "Misc.hpp"
#include "Render.hpp"
#include "Glow.hpp"

#include "json.hpp"

#pragma execution_character_set("utf-8")

class Config {
public:
    using json = nlohmann::json;

    static Config& GetInstance() {
        static Config instance;
        return instance;
    }

    Aimbot* AimAssistance = nullptr;
    Camera* GameCamera = nullptr;
    Sense* ESP = nullptr;
    Misc* Miscellanous = nullptr;
    bool initialized = false;

    Config() = default;

    Config(Config const&) = delete;
    void operator=(Config const&) = delete;

    void Initialize(Aimbot* aimAssistance, Camera* gameCamera, Sense* esp, Misc* miscellanous) {
        if (!initialized) {
            AimAssistance = aimAssistance;
            GameCamera = gameCamera;
            ESP = esp;
            Miscellanous = miscellanous;

            LoadConfig(ConfigPath);
            initialized = true;
        }
    }

    void Save(const std::string& filename) {
        std::ofstream configFile(filename);
        if (!configFile.is_open()) {
            std::cerr << "Unable to open the configuration file：" << filename << std::endl;
            return;
        }

        json Config;

        // 保存AimbotConfig
        Config["Aimbot"] = {
                {"AimBotType", AimAssistance->AimBotType},
                {"Enable", AimAssistance->Sticky},
                {"InputModel", AimAssistance->InputModel},
                {"AutoBone", AimAssistance->autobone},
                {"AllBone", AimAssistance->allbone},
                {"AimTeam", AimAssistance->AimTeam},
                {"aim_time", AimAssistance->aim_time},
                {"FOV", AimAssistance->FOV},
                {"ZoomFOV", AimAssistance->ZoomFOV},
                {"Smooth", AimAssistance->Smooth},
                {"ZoomSmooth", AimAssistance->ZoomSmooth},
                {"Flicksmooth", AimAssistance->Flicksmooth},
                {"SkySmooth", AimAssistance->SkySmooth},
                {"MaxSmoothIncrease", AimAssistance->MaxSmoothIncrease},
                {"RecoilCompensation", AimAssistance->RecoilCompensation},
                {"AimBotKey", AimAssistance->AimBotKey},
                {"AimBotKey2", AimAssistance->AimBotKey2},
                {"AimTriggerKey", AimAssistance->AimTriggerKey},
                {"AimFlickKey", AimAssistance->AimFlickKey},
                {"AimHeadKey", AimAssistance->AimHeadKey},
                {"ThrowKey", AimAssistance->ThrowKey},
                {"inputAimKey", AimAssistance->inputAimKey},
                {"inputAimKey2", AimAssistance->inputAimKey2},
                {"inputTriggerKey", AimAssistance->inputTriggerKey},
                {"inputFlickKey", AimAssistance->inputFlickKey},
                {"inputHeadKey", AimAssistance->inputHeadKey},
                {"inputThrowKey", AimAssistance->inputThrowKey},
                {"AimBone", AimAssistance->aimBone}
        };

        // 保存DeviceConfig
        Config["Device"] = {
                {"DeviceType", AimAssistance->KmboxType},
                {"KmboxIP", AimAssistance->KmboxIP},
                {"KmboxPort", AimAssistance->KmboxPort},
                {"KmboxUUID", AimAssistance->KmboxUUID},
                {"KmboxComPort", AimAssistance->KmboxComPort},
                {"BaudRate", AimAssistance->BaudRate},
//                {"LurkerComPort", DeviceC.LurkerComPort},
//                {"MoBoxComPort", DeviceC.MoBoxComPort}
        };

        // 保存GlowConfig
        Config["Glow"] = {
                {"Item_Gold", Item_Gold},
                {"Item_Red", Item_Red},
                {"Item_Purple", Item_Purple},
                {"Item_Blue", Item_Blue},
                {"Item_Grey", Item_Grey},
                {"Item_Weapons", Item_Weapons},
                {"Item_Ammo", Item_Ammo},
                {"ItemGlow", ESP->ItemGlow},
                {"enableGlow", ESP->enableGlow},
                {"checkShield", ESP->checkShield},
                {"insidetype", ESP->insidetype},
                {"outlinetype", ESP->outlinetype},
                {"outlinesize", ESP->outlinesize},
                {"Invisible", {ESP->GlowInvisible[0], ESP->GlowInvisible[1], ESP->GlowInvisible[2]}},
                {"Visible", {ESP->GlowVisible[0], ESP->GlowVisible[1], ESP->GlowVisible[2]}},
                {"Admin", {ESP->GlowAdmin[0], ESP->GlowAdmin[1], ESP->GlowAdmin[2]}},
                {"Knocked", {ESP->GlowKnocked[0], ESP->GlowKnocked[1], ESP->GlowKnocked[2]}},
                {"HighlightScale", ESP->HighlightScale}
        };

        //保存绘制选项
        Config["ESP"] = {
                {"ScreenSizeX", GameCamera->ScreenSizeX},
                {"ScreenSizeY", GameCamera->ScreenSizeY},
                {"GameFOV", GameCamera->FOV},
                {"GlowESPTeam", GlowESPTeam},
                {"box", v.box},
                {"skeleton", v.skeleton},
                {"line", v.line},
                {"distance", v.distance},
                {"healthbar", v.healthbar},
                {"xp", v.xp},
                {"rank", v.rank},
                {"drawinfo", v.drawinfo},
                {"drawteam", v.drawteam},
                {"drawname", v.drawname},
                {"showfov", v.showfov},
                {"PlayerWarning", v.PlayerWarning},
                {"drawdistance", v.drawdistance},
                {"infodistance", v.infodistance},
                {"WarningDistance", v.WarningDistance},
                {"SkeltonThickness", SkeltonThickness},
                {"BoneFreshDistance", BoneFreshDistance},
                {"BarStyle", BarStyle},
                {"textspector", textspector},
                {"guispector", guispector }
        };
        // 保存ColorConfig
        Config["Color"] = {
                {"Bone Invisible", {DRAW.Bone.Invisible[0], DRAW.Bone.Invisible[1], DRAW.Bone.Invisible[2], DRAW.Bone.Invisible[3]}},
                {"Bone Visible", {DRAW.Bone.Visible[0], DRAW.Bone.Visible[1], DRAW.Bone.Visible[2], DRAW.Bone.Visible[3]}},
                {"Box Invisible", {DRAW.Box.Invisible[0], DRAW.Box.Invisible[1], DRAW.Box.Invisible[2], DRAW.Box.Invisible[3]}},
                {"Box Visible", {DRAW.Box.Visible[0], DRAW.Box.Visible[1], DRAW.Box.Visible[2], DRAW.Box.Visible[3]}},
                {"Text EnableRankColor", DRAW.Text.EnableRankColor},
                {"Text size", DRAW.Text.size},
                {"Text Info", {DRAW.Text.Info[0], DRAW.Text.Info[1], DRAW.Text.Info[2], DRAW.Text.Info[3]}}
        };

        Config["Misc"] = {
                {"SuperGlide", Miscellanous->SuperGlide},
                {"isTS", Miscellanous->isTS},
                {"autorbjump", Miscellanous->autorbjump},
                {"fastreload", AimAssistance->fastreload},
                {"autoGrpple", Miscellanous->autoGrpple},
                {"EnableVSync", EnableVSync}
        };

        configFile << Config.dump(4);
        std::cout << "Save the configuration successfully: " << filename << std::endl;
    }

    void LoadConfig(const std::string& filename) {
        std::ifstream configFile(filename);
        if (!configFile.is_open()) {
            std::cerr << "Failed to open config file: " << filename << std::endl;
            return;
        }

        json configJson;
        try {
            configFile >> configJson;

            // 加载AimbotConfig
            json aimbotConfig = configJson.at("Aimbot");
            AimAssistance->AimBotType = aimbotConfig.at("AimBotType").get<std::string>();
            AimAssistance->Sticky = aimbotConfig.at("Enable").get<bool>();
            AimAssistance->InputModel = aimbotConfig.at("InputModel").get<bool>();
            AimAssistance->autobone = aimbotConfig.at("AutoBone").get<bool>();
            AimAssistance->allbone = aimbotConfig.at("AllBone").get<bool>();
            AimAssistance->AimTeam = aimbotConfig.at("AimTeam").get<bool>();
            AimAssistance->aim_time = aimbotConfig.at("aim_time").get<float>();
            AimAssistance->FOV = aimbotConfig.at("FOV").get<float>();
            AimAssistance->ZoomFOV = aimbotConfig.at("ZoomFOV").get<float>();
            AimAssistance->Smooth = aimbotConfig.at("Smooth").get<float>();
            AimAssistance->ZoomSmooth = aimbotConfig.at("ZoomSmooth").get<float>();
            AimAssistance->Flicksmooth = aimbotConfig.at("Flicksmooth").get<float>();
            AimAssistance->SkySmooth = aimbotConfig.at("SkySmooth").get<float>();
            AimAssistance->MaxSmoothIncrease = aimbotConfig.at("MaxSmoothIncrease").get<float>();
            AimAssistance->RecoilCompensation = aimbotConfig.at("RecoilCompensation").get<float>();
            AimAssistance->AimBotKey = aimbotConfig.at("AimBotKey").get<int>();
            AimAssistance->AimBotKey2 = aimbotConfig.at("AimBotKey2").get<int>();
            AimAssistance->AimTriggerKey = aimbotConfig.at("AimTriggerKey").get<int>();
            AimAssistance->AimFlickKey = aimbotConfig.at("AimFlickKey").get<int>();
            AimAssistance->AimHeadKey = aimbotConfig.at("AimHeadKey").get<int>();
            AimAssistance->ThrowKey = aimbotConfig.at("ThrowKey").get<int>();
            AimAssistance->inputAimKey = aimbotConfig.at("inputAimKey").get<int>();
            AimAssistance->inputAimKey2 = aimbotConfig.at("inputAimKey2").get<int>();
            AimAssistance->inputTriggerKey = aimbotConfig.at("inputTriggerKey").get<int>();
            AimAssistance->inputFlickKey = aimbotConfig.at("inputFlickKey").get<int>();
            AimAssistance->inputHeadKey = aimbotConfig.at("inputHeadKey").get<int>();
            AimAssistance->inputThrowKey = aimbotConfig.at("inputThrowKey").get<int>();
            AimAssistance->aimBone = aimbotConfig.at("AimBone").get<int>();

            // 加载DeviceConfig
            json deviceConfig = configJson.at("Device");
            AimAssistance->KmboxType = deviceConfig.at("DeviceType").get<std::string>();
            std::strncpy(AimAssistance->KmboxIP, deviceConfig.at("KmboxIP").get<std::string>().c_str(), sizeof(AimAssistance->KmboxIP) - 1);
            AimAssistance->KmboxIP[sizeof(AimAssistance->KmboxIP) - 1] = '\0';
            std::strncpy(AimAssistance->KmboxPort, deviceConfig.at("KmboxPort").get<std::string>().c_str(), sizeof(AimAssistance->KmboxPort) - 1);
            AimAssistance->KmboxPort[sizeof(AimAssistance->KmboxPort) - 1] = '\0';
            std::strncpy(AimAssistance->KmboxUUID, deviceConfig.at("KmboxUUID").get<std::string>().c_str(), sizeof(AimAssistance->KmboxUUID) - 1);
            AimAssistance->KmboxUUID[sizeof(AimAssistance->KmboxUUID) - 1] = '\0';
            AimAssistance->KmboxComPort = deviceConfig.at("KmboxComPort").get<int>();
            AimAssistance->BaudRate = deviceConfig.at("BaudRate").get<int>();

            // 加载GlowConfig
            json glowConfig = configJson.at("Glow");
            Item_Gold = glowConfig.at("Item_Gold").get<bool>();
            Item_Red = glowConfig.at("Item_Red").get<bool>();
            Item_Purple = glowConfig.at("Item_Purple").get<bool>();
            Item_Blue = glowConfig.at("Item_Blue").get<bool>();
            Item_Grey = glowConfig.at("Item_Grey").get<bool>();
            Item_Weapons = glowConfig.at("Item_Weapons").get<bool>();
            Item_Ammo = glowConfig.at("Item_Ammo").get<bool>();
            ESP->ItemGlow = glowConfig.at("ItemGlow").get<bool>();
            ESP->enableGlow = glowConfig.at("enableGlow").get<bool>();
            ESP->checkShield = glowConfig.at("checkShield").get<bool>();
            ESP->insidetype = glowConfig.at("insidetype").get<int>();
            ESP->outlinetype = glowConfig.at("outlinetype").get<int>();
            ESP->outlinesize = glowConfig.at("outlinesize").get<int>();
            for (size_t i = 0; i < 3; ++i) {
                ESP->GlowInvisible[i] = glowConfig.at("Invisible")[i].get<float>();
                ESP->GlowVisible[i] = glowConfig.at("Visible")[i].get<float>();
                ESP->GlowAdmin[i] = glowConfig.at("Admin")[i].get<float>();
                ESP->GlowKnocked[i] = glowConfig.at("Knocked")[i].get<float>();
            }
            ESP->HighlightScale = glowConfig.at("HighlightScale").get<float>();

            // 加载ESPConfig
            json espConfig = configJson.at("ESP");
            GameCamera->ScreenSizeX = espConfig.at("ScreenSizeX").get<float>();
            GameCamera->ScreenSizeY = espConfig.at("ScreenSizeY").get<float>();
            GameCamera->FOV = espConfig.at("GameFOV").get<float>();
            GlowESPTeam = espConfig.at("GlowESPTeam").get<bool>();
            v.box = espConfig.at("box").get<bool>();
            v.skeleton = espConfig.at("skeleton").get<bool>();
            v.line = espConfig.at("line").get<bool>();
            v.distance = espConfig.at("distance").get<bool>();
            v.healthbar = espConfig.at("healthbar").get<bool>();
            v.xp = espConfig.at("xp").get<bool>();
            v.rank = espConfig.at("rank").get<bool>();
            v.drawinfo = espConfig.at("drawinfo").get<bool>();
            v.drawteam = espConfig.at("drawteam").get<bool>();
            v.drawname = espConfig.at("drawname").get<bool>();
            v.showfov = espConfig.at("showfov").get<bool>();
            v.PlayerWarning = espConfig.at("PlayerWarning").get<bool>();
            v.drawdistance = espConfig.at("drawdistance").get<float>();
            v.infodistance = espConfig.at("infodistance").get<float>();
            v.WarningDistance = espConfig.at("WarningDistance").get<float>();
            SkeltonThickness = espConfig.at("SkeltonThickness").get<float>();
            BoneFreshDistance = espConfig.at("BoneFreshDistance").get<float>();
            BarStyle = espConfig.at("BarStyle").get<int>();
            textspector = espConfig.at("textspector").get<bool>();
            guispector = espConfig.at("guispector").get<bool>();

            // 加载ColorConfig
            json colorConfig = configJson.at("Color");
            for (size_t i = 0; i < 4; ++i) {
                DRAW.Bone.Invisible[i] = colorConfig.at("Bone Invisible")[i].get<float>();
                DRAW.Bone.Visible[i] = colorConfig.at("Bone Visible")[i].get<float>();
                DRAW.Box.Invisible[i] = colorConfig.at("Box Invisible")[i].get<float>();
                DRAW.Box.Visible[i] = colorConfig.at("Box Visible")[i].get<float>();
                DRAW.Text.Info[i] = colorConfig.at("Text Info")[i].get<float>();
            }
            DRAW.Text.EnableRankColor = colorConfig.at("Text EnableRankColor").get<bool>();
            DRAW.Text.size = colorConfig.at("Text size").get<float>();

            // 加载MiscConfig
            json miscConfig = configJson.at("Misc");
            Miscellanous->SuperGlide = miscConfig.at("SuperGlide").get<bool>();
            Miscellanous->isTS = miscConfig.at("isTS").get<bool>();
            Miscellanous->autorbjump = miscConfig.at("autorbjump").get<bool>();
            AimAssistance->fastreload = miscConfig.at("fastreload").get<bool>();
            Miscellanous->autoGrpple = miscConfig.at("autoGrpple").get<bool>();
            EnableVSync = miscConfig.at("EnableVSync").get<bool>();

            std::cout << "Load the configuration successfully: " << filename << std::endl;
        }
        catch (json::parse_error& e) {
            std::cerr << "JSON Parse Error: " << e.what() << std::endl;
        }
        catch (json::out_of_range& e) {
            std::cerr << "JSON Out of Range Error: " << e.what() << std::endl;
        }
        catch (std::exception& e) {
            std::cerr << "General Error: " << e.what() << std::endl;
        }
    }

private:

};