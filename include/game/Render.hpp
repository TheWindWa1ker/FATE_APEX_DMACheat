#pragma once
#include "game/LocalPlayer.hpp"
#include "game/Player.hpp"
#include "game/Camera.hpp"
#include "Spectator.hpp"
#include "game/Aimbot.hpp"
#include "game/Glow.hpp"

#include "Auth.hpp"

#define GREEN ImColor(0, 255, 0)
#define RED ImColor(255, 75, 0)
#define BLUE ImColor(51, 153, 255)
#define ORANGE ImColor(255, 165, 0)
#define WHITE ImColor(255, 255, 255)
#define PURPLE ImColor(255, 0 , 255)

#define U8(_S)    (const char*)u8##_S
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_RESET   "\033[0m"
#define COLOR_LYELLOW "\033[1;33m"
#define COLOR_LGREEN  "\033[1;32m"
#define COLOR_LBLUE   "\033[1;34m"

inline std::string ConfigPath = "Config/Default.FATE";

inline void Log(const int& type, const char* format, ...) {
    std::ostringstream oss;

    //oss << COLOR_LYELLOW << "[" << getCurrentTime() << "]";

    if (type == 1) {
        oss << COLOR_LGREEN << "[+] ";
    }
    else if (type == 2) {
        oss << COLOR_RED << "[X] ";
    }
    else {
        oss << COLOR_LYELLOW << "[-] ";
    }

    va_list args;
            va_start(args, format);
    char buffer[256];
    std::vsnprintf(buffer, sizeof(buffer), format, args);

            va_end(args);

    oss << buffer;
    oss << COLOR_RESET;

    std::cout << oss.str() << std::endl;
}
void AuthRender();

void Render(LocalPlayer* Myself, std::vector<Player*>* Players, Camera* GameCamera, Spectator* Spectators, Aimbot* AimAssist, Sense* ESP);

struct Colors
{
    struct
    {
        ImColor InVisible;
        ImColor Visible;
    } Box;

    struct
    {
        ImColor InVisible;
        ImColor Visible;
    } Bone;

};
inline Colors PlayerColors;

typedef struct visuals {
    bool box = true;
    bool skeleton = false;
    bool line = false;
    bool distance = true;
    bool healthbar = true;
    bool xp = true;
    bool rank = true;
    bool drawinfo = true;
    bool drawteam = true;
    bool drawname = true;
    bool showfov = false;
    bool PlayerWarning = true;
    float drawdistance = 300.0f;
    float infodistance = 100.0f;
    float WarningDistance = 300.f;
} visuals;
inline visuals v;

struct DRAW_Misc
{
    static inline ImVec4 FloatToImColor(const float Color[4]) {
        return {Color[0], Color[1], Color[2], Color[3]};
    }

    struct
    {
        float Invisible[4] = { 1, 0, 0, 1 };
        float Visible[4] = { 0, 1, 0, 1 };
    } Bone;

    struct
    {
        float Invisible[4] = { 1, 0, 0, 1 };
        float Visible[4] = { 0, 1, 0, 1 };
    } Box;

    struct
    {
        bool EnableRankColor = false;
        float size = 15.f;
        float Info[4] = { 0.1, 1, 0.3, 1 };
    } Text;
};
inline DRAW_Misc DRAW;

//////////////////////////////////定义区
//        static widget test{};
inline char buffer[999] = "";
inline bool show_calculator{};
inline int distance{13};
inline float hp{82};
inline std::string result{};
inline int menutap{1};
inline ImVec4 clear_color = ImVec4{0, 0, 0, 0};
inline bool ShowMenu = true;
//////////////////////////////////定义区结束

//垂直同步
inline bool keyboardstate = false;
inline bool EnableVSync = true;
inline bool PlayersPopulated = false;
inline bool RenderEsp = true;
inline int BarStyle = 1;
inline float SkeltonThickness = 1.f;

inline float BoneFreshDistance = 100.f;

// 根据击中盒类型获取对应的骨骼索引
inline int GetBone(HitboxType HitBox,uint64_t ModelPointer) {
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
inline Vector3D GetBonePOS(HitboxType HitBox,uint64_t ModelPointer, Vector3D LocalOrigin,uint64_t BonePointer) {
    // 初始化一个偏移量为0的三维VectorQuantity
    Vector3D Offset = Vector3D(0.0f, 0.0f, 0.0f);

    // 调用GetBoneFromHitbox函数获取对应的骨骼索引
    int Bone = GetBone(HitBox, ModelPointer);
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

//全局声明
static RECT menuRect;

static LONG nv_default = WS_POPUP | WS_CLIPSIBLINGS;
static LONG nv_default_in_game = nv_default | WS_DISABLED;
static LONG nv_edit = nv_default_in_game | WS_VISIBLE;

static LONG nv_ex_default = WS_EX_TOOLWINDOW;
static LONG nv_ex_edit = nv_ex_default | WS_EX_LAYERED | WS_EX_TRANSPARENT;
static LONG nv_ex_edit_menu = nv_ex_default | WS_EX_TRANSPARENT;

//放入菜单循环
inline void UpdateWindowTransparency(HWND hwnd)
{
    //static bool lastIsMenuRegion = false;
    POINT mousePos;
    GetCursorPos(&mousePos);
    ScreenToClient(hwnd, &mousePos);

    //bool isMenuRegion = (mousePos.x >= menuRect.left && mousePos.x <= menuRect.right &&
    //	mousePos.y >= menuRect.top && mousePos.y <= menuRect.bottom);

    if (mousePos.x >= menuRect.left && mousePos.x <= menuRect.right &&
        mousePos.y >= menuRect.top && mousePos.y <= menuRect.bottom) {
        // 鼠标在菜单区域内，不穿透
        SetWindowLong(hwnd, GWL_EXSTYLE, nv_ex_edit_menu);
    }
    else {
        // 鼠标在菜单区域外，穿透
        SetWindowLong(hwnd, GWL_EXSTYLE, nv_ex_edit);
    }

    // 重新设置窗口置顶
    //SetWindowPos(GameData.Config.Overlay.hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    //lastIsMenuRegion = isMenuRegion;
}