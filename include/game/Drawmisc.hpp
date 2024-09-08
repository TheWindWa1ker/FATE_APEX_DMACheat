#pragma once
#include "Font.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"
#include <d3d11.h>
#include <D3DX11tex.h>
#pragma comment(lib, "D3DX11.lib")

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "tools.h"
#include "file.h"
#include "Game_struct.h"

#pragma execution_character_set("utf-8")

int width = 0;
int height = 0;
float seer_dist = 400.0 * 40.0f;
float xp_dist = 600.0f * 40.0f;
bool draw = false;

//开关融合器背景
static bool isBlenderChecked = false; // 用于追踪勾选框的状态

std::chrono::steady_clock::time_point newTime = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point oldTime1 = std::chrono::steady_clock::now();

extern std::vector<Player *> *Players;
extern Camera* GameCamera;
extern Misc* Miscellanous;
extern Level* Map;

DWORD picker_flags = ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreview;

// 获取指定目录下的所有文件名
std::vector<std::string> getFilesInDirectory(const std::string& directory) {
    std::vector<std::string> files;
    for (const auto& entry : std::filesystem::directory_iterator(directory))
        if (entry.is_regular_file())
            files.push_back(entry.path().filename().string());
    return files;
}

void Helpmarker(const char* Text, ImVec4 Color)
{
    ImGui::TextColored(Color, "(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip(Text);
    }
}

//绘制相关
void DrawFontStroke(float size, float x, float y, ImVec4 color, const char* str)
{
    auto draw = ImGui::GetForegroundDrawList();
    draw->AddText(NULL, size, ImVec2(x + 1, y), ImColor(15, 15, 20, 255), str);
    draw->AddText(NULL, size, ImVec2(x - 1, y), ImColor(15, 15, 20, 255), str);
    draw->AddText(NULL, size, ImVec2(x, y + 1), ImColor(15, 15, 20, 255), str);
    draw->AddText(NULL, size, ImVec2(x, y - 1), ImColor(15, 15, 20, 255), str);
    draw->AddText(NULL, size, ImVec2(x, y), ImColor(color), str);
}

void DrawFontStroke(const ImFont* font, float size, ImVec2 pos, ImVec4 color, const char* str)
{
    auto draw = ImGui::GetForegroundDrawList();
    draw->AddText(font, size, ImVec2(pos.x + 1, pos.y), ImColor(15, 15, 20, 255), str);
    draw->AddText(font, size, ImVec2(pos.x - 1, pos.y), ImColor(15, 15, 20, 255), str);
    draw->AddText(font, size, ImVec2(pos.x, pos.y + 1), ImColor(15, 15, 20, 255), str);
    draw->AddText(font, size, ImVec2(pos.x, pos.y - 1), ImColor(15, 15, 20, 255), str);
    draw->AddText(font, size, ImVec2(pos.x, pos.y), ImColor(color), str);
}

void DrawBoxStroke(Vector4D Box, ImColor color, float Thickness) {

    auto draw = ImGui::GetForegroundDrawList();

    draw->AddLine({ Box.x - 1,Box.y - 1 }, { Box.x + Box.w + 1,Box.y - 1 }, ImColor(0, 0, 0, 255), 1);
    draw->AddLine({ Box.x + 1,Box.y + 1 }, { Box.x + Box.w - 1,Box.y + 1 }, ImColor(0, 0, 0, 255), 1);
    draw->AddLine({ Box.x,Box.y }, { Box.x + Box.w,Box.y }, color, Thickness);

    draw->AddLine({ Box.x - 1,Box.y - 1 }, { Box.x - 1,Box.y + Box.z + 1 }, ImColor(0, 0, 0, 255), 1);
    draw->AddLine({ Box.x + 1,Box.y + 1 }, { Box.x + 1,Box.y + Box.z + 1 }, ImColor(0, 0, 0, 255), 1);
    draw->AddLine({ Box.x,Box.y }, { Box.x,Box.y + Box.z }, color, Thickness);

    draw->AddLine({ Box.x + Box.w + 1,Box.y - 1 }, { Box.x + Box.w + 1,Box.y + Box.z + 1 }, ImColor(0, 0, 0, 255), 1);
    draw->AddLine({ Box.x + Box.w - 1,Box.y + 1 }, { Box.x + Box.w - 1,Box.y + Box.z - 1 }, ImColor(0, 0, 0, 255), 1);
    draw->AddLine({ Box.x + Box.w,Box.y }, { Box.x + Box.w,Box.y + Box.z }, color, Thickness);

    draw->AddLine({ Box.x - 1,Box.y + Box.z + 1 }, { Box.x + Box.w + 1,Box.y + Box.z + 1 }, ImColor(0, 0, 0, 255), 1);
    draw->AddLine({ Box.x + 1,Box.y + Box.z - 1 }, { Box.x + Box.w - 1,Box.y + Box.z - 1 }, ImColor(0, 0, 0, 255), 1);
    draw->AddLine({ Box.x,Box.y + Box.z }, { Box.x + Box.w,Box.y + Box.z }, color, Thickness);

}

void DrawBox(Vector4D Box, ImColor color, float Thickness) {

    auto draw = ImGui::GetForegroundDrawList();

    draw->AddLine({ Box.x,Box.y }, { Box.x + Box.w,Box.y }, color, Thickness);
    draw->AddLine({ Box.x,Box.y }, { Box.x,Box.y + Box.z }, color, Thickness);
    draw->AddLine({ Box.x + Box.w,Box.y }, { Box.x + Box.w,Box.y + Box.z }, color, Thickness);
    draw->AddLine({ Box.x,Box.y + Box.z }, { Box.x + Box.w,Box.y + Box.z }, color, Thickness);
}

int CalculateEntityDistance(Vector3D LocalCoord, Vector3D EntityCoord) {
    return (int)(sqrt((LocalCoord.x - EntityCoord.x) * (LocalCoord.x - EntityCoord.x) + (LocalCoord.y - EntityCoord.y) * (LocalCoord.y - EntityCoord.y) + (LocalCoord.z - EntityCoord.z) * (LocalCoord.z - EntityCoord.z))) * 0.025;
}

Vector3D RotatePoint(Vector3D EntityPos, Vector3D LocalPlayerPos, int posX, int posY, int sizeX, int sizeY, float angle, float zoom)
{
    float r_1, r_2;
    float x_1, y_1;

    r_1 = -(EntityPos.y - LocalPlayerPos.y);
    r_2 = EntityPos.x - LocalPlayerPos.x;
    float Yaw = 0;

    float yawToRadian = Yaw * (float)(M_PI / 180.0F);
    x_1 = (float)(r_2 * (float)cos((double)(yawToRadian)) - r_1 * sin((double)(yawToRadian))) / 20;
    y_1 = (float)(r_2 * (float)sin((double)(yawToRadian)) + r_1 * cos((double)(yawToRadian))) / 20;

    x_1 *= zoom;
    y_1 *= zoom;

    int sizX = sizeX / 2;
    int sizY = sizeY / 2;

    x_1 += sizX;
    y_1 += sizY;
    x_1 += posX;
    y_1 += posY;
    return Vector3D(x_1, y_1, 0);
}

void DrawArrowAtPosition(float arrow_center_x, float arrow_center_y, ImColor color, float angle)
{
    std::array<ImVec2, 4> points{
            ImVec2(-22.0f, -8.6f), // 左下角
            ImVec2(0.0f, 0.0f),    // 顶点
            ImVec2(-22.0f, 8.6f),  // 右下角
            ImVec2(-18.0f, 0.0f)   // 箭头底部中间点，用于闭合箭头底部
    };

    // 计算顶点相对于底部中间点的偏移量，以使其成为旋转的中心
    ImVec2 center_offset(points[3].x - points[1].x, points[3].y - points[1].y);

    for (auto& point : points)
    {
        // 应用缩放因子
        auto x = (point.x - center_offset.x) * 0.65;
        auto y = (point.y - center_offset.y) * 0.65;

        float angle1 = (360.0 - angle) * (M_PI / 180.0);

        point.x = arrow_center_x + x * cosf(angle1) - y * sinf(angle1);
        point.y = arrow_center_y + x * sinf(angle1) + y * cosf(angle1);
    }

    // 绘制箭头的两个三角形部分
    ImDrawList* draw_list = ImGui::GetForegroundDrawList();
    draw_list->AddTriangleFilled(points[0], points[1], points[3], color); // 左边的三角形
    draw_list->AddTriangleFilled(points[2], points[1], points[3], color); // 右边的三角形
    draw_list->AddQuad(points[0], points[1], points[2], points[3], ImColor(0, 0, 0, 255), 1.5f);
}

void DrawLine(ImVec2 a, ImVec2 b, ImColor color, float width) {
    ImGui::GetWindowDrawList()->AddLine(a, b, color, width);
}

void DrawLine(Vector2D a, Vector2D b, ImColor color, float width) {
    ImVec2 c = ImVec2(a.x, a.y);
    ImVec2 d = ImVec2(b.x, b.y);
    ImGui::GetWindowDrawList()->AddLine(c, d, color, width);
}

void DrawBox(ImColor color, float x, float y, float w, float h) {
    DrawLine(ImVec2(x, y), ImVec2(x + w, y), color, 1.0f);
    DrawLine(ImVec2(x, y), ImVec2(x, y + h), color, 1.0f);
    DrawLine(ImVec2(x + w, y), ImVec2(x + w, y + h), color, 1.0f);
    DrawLine(ImVec2(x, y + h), ImVec2(x + w, y + h), color, 1.0f);
}

void Text(ImVec2 pos, ImColor color, const char *text_begin, const char *text_end, float wrap_width, const ImVec4 *cpu_fine_clip_rect) {
    ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), 14, pos, color, text_begin, text_end, wrap_width,
                                        cpu_fine_clip_rect);
}

void Text(ImVec2 pos, ImColor color, const char *text_begin, const char *text_end, float wrap_width, const ImVec4 *cpu_fine_clip_rect, int font) {
    ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), font, pos, color, text_begin, text_end, wrap_width,
                                        cpu_fine_clip_rect);
}

void String(ImVec2 pos, ImColor color, const char *text) {
    Text(pos, color, text, text + strlen(text), 200, 0);
}

void String(ImVec2 pos, ImColor color, const char *text, int font) {
    Text(pos, color, text, text + strlen(text), 200, 0, font);
}

void RectFilled(float x0, float y0, float x1, float y1, ImColor color, float rounding, int rounding_corners_flags) {
    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x0, y0), ImVec2(x1, y1), color, rounding, rounding_corners_flags);
}

void ProgressBar(float x, float y, float w, float h, int value, int v_max, ImColor barColor) {
    RectFilled(x, y, x + w, y + ((h / float(v_max)) * (float) value), barColor, 0.0f, 0);
}

void HealthBar(Vector4D Head, int health) {
    //1:3

    ImVec2 p1(Head.x - Head.w * 2, Head.y - 12);
    ImVec2 p2(Head.x - Head.w * 2, Head.y - 16);

    ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // 默认颜色 (白色)

    if (health > 100) {
        health = 100;
    }

    // 根据健康值调整颜色
    if (health >= 90) {
        color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);      // 绿色
    }
    else if (health >= 80) {
        color = ImVec4(0.0f, 0.8f, 0.8f, 1.0f);      // 青绿
    }
    else if (health >= 70) {
        color = ImVec4(0.5f, 1.0f, 0.5f, 1.0f);      // 嫩绿
    }
    else if (health >= 60) {
        color = ImVec4(0.8f, 1.0f, 0.0f, 1.0f);      // 嫩黄
    }
    else if (health >= 40) {
        color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);      // 黄色
    }
    else if (health >= 5) {
        color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);      // 红色
    }

    // 绘制外部黑色矩形背景
    ImGui::GetForegroundDrawList()->AddRectFilled(p1, p2, ImColor(0, 0, 0, 100));

    // 根据健康值动态调整血条的宽度
    float healthWidth = ((Head.w * 5) * health) / 100.0f; // 血条当前宽度
    float MaxhealthWidth = Head.w * 5; // 满血血条宽度
    ImVec2 healthBarTopRight(p1.x + healthWidth, p2.y); // 血条右边的点，静态宽度
    ImVec2 MaxhealthBarTopRight(p1.x + MaxhealthWidth, p2.y); // 血条右边的点，动态宽度

    // 绘制内部健康条
    ImVec4 segmentColor = ImVec4 (0.5f, 0.5f, 0.5f, 0.5f);
    ImGui::GetForegroundDrawList()->AddRectFilled(p1, MaxhealthBarTopRight, ImColor(segmentColor));
    ImGui::GetForegroundDrawList()->AddRectFilled(p1, healthBarTopRight, ImColor(color));
}

void ShildBar(Vector4D Head, int ShieldHeal, int ShieldHealthMax) {
    //2倍- Head.w数量加上方框的一个Head.w等于血条或护甲条长度

    ImVec2 p1(Head.x - Head.w * 2, Head.y - 18);
    ImVec2 p2(Head.x - Head.w * 2, Head.y - 22);

    int numSegments = ShieldHealthMax / 25; // 总共的格子数，基于ShieldHealthMax动态计算
    float gapWidth = (Head.w * 5) / 20; // 每个格子之间的间隔宽度
    float totalGapWidth = gapWidth * (numSegments - 1); // 总间隔宽度
    float segmentWidth = ((Head.w * 5) - totalGapWidth) / numSegments; // 每个格子的宽度，考虑间隔

    ImVec4 color; // 根据 ShieldHealthMax 调整颜色

    // 根据 ShieldHealthMax 调整颜色
    if (ShieldHealthMax == 50) {
        color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // 白色
    }
    else if (ShieldHealthMax == 75) {
        color = ImColor(0, 161, 255, 255); // 蓝色
    }
    else if (ShieldHealthMax == 100) {
        color = ImColor(178, 58, 247, 255); // 紫色
    }
    else if (ShieldHealthMax == 125) {
        color = ImVec4(2.0f, 0.0f, 0.0f, 1.0f); // 红色
    }
    else {
        color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // 默认颜色 (白色)
    }

    // 保证健康值不超过 ShieldHealthMax
    if (ShieldHeal > ShieldHealthMax) {
        ShieldHeal = ShieldHealthMax;
    }

    // 绘制外部黑色矩形背景
    ImGui::GetForegroundDrawList()->AddRectFilled(p1, p2, ImColor(0, 0, 0, 100));

    //// 根据 ShieldHeal 调整内部血条的宽度
    //float healthWidth = (Head.w * ShieldHeal) / (float)ShieldHealthMax; // 血条当前宽度的计算公式
    //ImVec2 healthBarTopRight(p1.x + Head.w, p2.y);

    //// 绘制内部血条
    //ImGui::GetForegroundDrawList()->AddRectFilled(p1, healthBarTopRight, ImColor(color));
    // 计算并绘制每个格子
    for (int i = 0; i < numSegments; ++i) {
        ImVec2 segmentTopLeft(p1.x + i * (segmentWidth + gapWidth), p1.y);
        ImVec2 segmentBottomRight(segmentTopLeft.x + segmentWidth, p2.y);

        // 如果当前格子代表的护甲值在ShieldHeal内，就绘制颜色，否则绘制灰色
        ImVec4 segmentColor = ((i * 25) < ShieldHeal) ? color : ImVec4(0.5f, 0.5f, 0.5f, 0.5f);
        ImGui::GetForegroundDrawList()->AddRectFilled(segmentTopLeft, segmentBottomRight, ImColor(segmentColor));
    }
}

//seer esp
void DrawQuadFilled(ImVec2 p1, ImVec2 p2, ImVec2 p3, ImVec2 p4, ImColor color) {
    ImGui::GetWindowDrawList()->AddQuadFilled(p1, p2, p3, p4, color);
}

void DrawHexagon(const ImVec2 &p1, const ImVec2 &p2, const ImVec2 &p3, const ImVec2 &p4, const ImVec2 &p5, const ImVec2 &p6, ImU32 col, float thickness) {
    ImGui::GetWindowDrawList()->AddHexagon(p1, p2, p3, p4, p5, p6, col, thickness);
}

void DrawHexagonFilled(const ImVec2 &p1, const ImVec2 &p2, const ImVec2 &p3, const ImVec2 &p4, const ImVec2 &p5, const ImVec2 &p6, ImU32 col) {
    ImGui::GetWindowDrawList()->AddHexagonFilled(p1, p2, p3, p4, p5, p6, col);
}

void DrawHealth(float x, float y, int shield, int max_shield, int armorType, int health) {

    int bg_offset = 3;
    int bar_width = 105; //158
    // 4steps...2*3=6
    // 38*4=152 152+6 = 158
    // 5steps...2*4=8
    // 30*5=150 150+8 = 158
    float max_health = 100.0f;//100
    float shield_step = 25.0f; //25

    float shield_25 = 19.4f; //30
    int steps = 5;


    ImVec2 bg1(x - bar_width / 2 - bg_offset, y);
    ImVec2 bg2(bg1.x - 10, bg1.y - 16);
    ImVec2 bg3(bg2.x + 5, bg2.y - 7);
    ImVec2 bg4(bg3.x + bar_width + bg_offset, bg3.y);
    ImVec2 bg5(bg4.x + 11, bg4.y + 18);
    ImVec2 bg6(x + bar_width / 2 + bg_offset, y);
    DrawHexagonFilled(bg1, bg2, bg3, bg4, bg5, bg6, ImColor(0, 0, 0, 120));


    ImVec2 h1(bg1.x + 3, bg1.y - 4);
    ImVec2 h2(h1.x - 5, h1.y - 8);
    ImVec2 h3(h2.x + (float) health / max_health * bar_width, h2.y);
    ImVec2 h4(h1.x + (float) health / max_health * bar_width, h1.y);
    ImVec2 h3m(h2.x + bar_width, h2.y);
    ImVec2 h4m(h1.x + bar_width, h1.y);
    DrawQuadFilled(h1, h2, h3m, h4m, ImColor(10, 10, 30, 60));
    DrawQuadFilled(h1, h2, h3, h4, WHITE);


    ImColor shieldCracked(97, 97, 97);
    ImColor shieldCrackedDark(67, 67, 67);

    ImColor shieldCol;
    ImColor shieldColDark; //not used, but the real seer q has shadow inside
    if (max_shield == 50) { //white
        shieldCol = ImColor(247, 247, 247);
        shieldColDark = ImColor(164, 164, 164);
    } else if (max_shield == 75) { //blue
        shieldCol = ImColor(39, 178, 255);
        shieldColDark = ImColor(27, 120, 210);
    } else if (max_shield == 100) { //purple
        shieldCol = ImColor(206, 59, 255);
        shieldColDark = ImColor(136, 36, 220);
    } else if (max_shield == 100) { //gold
        shieldCol = ImColor(255, 255, 79);
        shieldColDark = ImColor(218, 175, 49);
    } else if (max_shield == 125) { //red
        shieldCol = ImColor(219, 2, 2);
        shieldColDark = ImColor(219, 2, 2);
    } else {
        shieldCol = ImColor(247, 247, 247);
        shieldColDark = ImColor(164, 164, 164);
    }
    int shield_tmp = shield;
    int shield1 = 0;
    int shield2 = 0;
    int shield3 = 0;
    int shield4 = 0;
    int shield5 = 0;
    if (shield_tmp > 25) {
        shield1 = 25;
        shield_tmp -= 25;
        if (shield_tmp > 25) {
            shield2 = 25;
            shield_tmp -= 25;
            if (shield_tmp > 25) {
                shield3 = 25;
                shield_tmp -= 25;
                if (shield_tmp > 25) {
                    shield4 = 25;
                    shield_tmp -= 25;
                    shield5 = shield_tmp;
                } else {
                    shield4 = shield_tmp;
                }
            } else {
                shield3 = shield_tmp;
            }
        } else {
            shield2 = shield_tmp;
        }
    } else {
        shield1 = shield_tmp;
    }
    ImVec2 s1(h2.x - 1, h2.y - 2);
    ImVec2 s2(s1.x - 3, s1.y - 5);
    ImVec2 s3(s2.x + shield1 / shield_step * shield_25, s2.y);
    ImVec2 s4(s1.x + shield1 / shield_step * shield_25, s1.y);
    ImVec2 s3m(s2.x + shield_25, s2.y);
    ImVec2 s4m(s1.x + shield_25, s1.y);

    ImVec2 ss1(s4m.x + 2, s1.y);
    ImVec2 ss2(s3m.x + 2, s2.y);
    ImVec2 ss3(ss2.x + shield2 / shield_step * shield_25, s2.y);
    ImVec2 ss4(ss1.x + shield2 / shield_step * shield_25, s1.y);
    ImVec2 ss3m(ss2.x + shield_25, s2.y);
    ImVec2 ss4m(ss1.x + shield_25, s1.y);

    ImVec2 sss1(ss4m.x + 2, s1.y);
    ImVec2 sss2(ss3m.x + 2, s2.y);
    ImVec2 sss3(sss2.x + shield3 / shield_step * shield_25, s2.y);
    ImVec2 sss4(sss1.x + shield3 / shield_step * shield_25, s1.y);
    ImVec2 sss3m(sss2.x + shield_25, s2.y);
    ImVec2 sss4m(sss1.x + shield_25, s1.y);

    ImVec2 ssss1(sss4m.x + 2, s1.y);
    ImVec2 ssss2(sss3m.x + 2, s2.y);
    ImVec2 ssss3(ssss2.x + shield4 / shield_step * shield_25, s2.y);
    ImVec2 ssss4(ssss1.x + shield4 / shield_step * shield_25, s1.y);
    ImVec2 ssss3m(ssss2.x + shield_25, s2.y);
    ImVec2 ssss4m(ssss1.x + shield_25, s1.y);

    ImVec2 sssss1(ssss4m.x + 2, s1.y);
    ImVec2 sssss2(ssss3m.x + 2, s2.y);
    ImVec2 sssss3(sssss2.x + shield5 / shield_step * shield_25, s2.y);
    ImVec2 sssss4(sssss1.x + shield5 / shield_step * shield_25, s1.y);
    ImVec2 sssss3m(sssss2.x + shield_25, s2.y);
    ImVec2 sssss4m(sssss1.x + shield_25, s1.y);
    if (max_shield == 50) {
        if (shield <= 25) {
            if (shield < 25) {
                DrawQuadFilled(s1, s2, s3m, s4m, shieldCracked);
                DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
            }
            if (shield != 0)
                DrawQuadFilled(s1, s2, s3, s4, shieldCol);

        } else if (shield <= 50) {
            DrawQuadFilled(s1, s2, s3, s4, shieldCol);
            if (shield != 50) {
                DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
            }
            if (shield != 0)
                DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
        }
    } else if (max_shield == 75) {
        if (shield <= 25) {
            if (shield < 25) {
                DrawQuadFilled(s1, s2, s3m, s4m, shieldCracked);
                DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
                DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
            }
            if (shield != 0)
                DrawQuadFilled(s1, s2, s3, s4, shieldCol);

        } else if (shield <= 50) {
            DrawQuadFilled(s1, s2, s3, s4, shieldCol);
            if (shield < 50) {
                DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
                DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
            }
            if (shield != 0)
                DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
        } else if (shield <= 75) {
            DrawQuadFilled(s1, s2, s3, s4, shieldCol);
            DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
            if (shield < 75) {
                DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
            }
            if (shield != 0)
                DrawQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
        }
    } else if (max_shield == 100) {
        if (shield <= 25) {
            if (shield < 25) {
                DrawQuadFilled(s1, s2, s3m, s4m, shieldCracked);
                DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
                DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
                DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
            }
            if (shield != 0)
                DrawQuadFilled(s1, s2, s3, s4, shieldCol);

        } else if (shield <= 50) {
            DrawQuadFilled(s1, s2, s3, s4, shieldCol);
            if (shield < 50) {
                DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
                DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
                DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
            }
            if (shield != 0)
                DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
        } else if (shield <= 75) {
            DrawQuadFilled(s1, s2, s3, s4, shieldCol);
            DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
            if (shield < 75) {
                DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
                DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
            }
            if (shield != 0)
                DrawQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
        } else if (shield <= 100) {
            DrawQuadFilled(s1, s2, s3, s4, shieldCol);
            DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
            DrawQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
            if (shield < 100) {
                DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
            }
            if (shield != 0)
                DrawQuadFilled(ssss1, ssss2, ssss3, ssss4, shieldCol);
        }
    } else if (max_shield == 125) {
        if (shield <= 25) {
            if (shield < 25) {
                DrawQuadFilled(s1, s2, s3m, s4m, shieldCracked);
                DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
                DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
                DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
                DrawQuadFilled(sssss1, sssss2, sssss3m, sssss4m, shieldCracked);
            }
            if (shield != 0)
                DrawQuadFilled(s1, s2, s3, s4, shieldCol);

        } else if (shield <= 50) {
            DrawQuadFilled(s1, s2, s3, s4, shieldCol);
            if (shield < 50) {
                DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
                DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
                DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
                DrawQuadFilled(sssss1, sssss2, sssss3m, sssss4m, shieldCracked);
            }
            if (shield != 0)
                DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
        } else if (shield <= 75) {
            DrawQuadFilled(s1, s2, s3, s4, shieldCol);
            DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
            if (shield < 75) {
                DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
                DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
                DrawQuadFilled(sssss1, sssss2, sssss3m, sssss4m, shieldCracked);
            }
            if (shield != 0)
                DrawQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
        } else if (shield <= 100) {
            DrawQuadFilled(s1, s2, s3, s4, shieldCol);
            DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
            DrawQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
            if (shield < 100) {
                DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
                DrawQuadFilled(sssss1, sssss2, sssss3m, sssss4m, shieldCracked);
            }
            if (shield != 0)
                DrawQuadFilled(ssss1, ssss2, ssss3, ssss4, shieldCol);
        } else if (shield <= 125) {
            DrawQuadFilled(s1, s2, s3, s4, shieldCol);
            DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
            DrawQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
            DrawQuadFilled(ssss1, ssss2, ssss3, ssss4, shieldCol);
            if (shield < 125) {
                DrawQuadFilled(sssss1, sssss2, sssss3m, sssss4m, shieldCracked);
            }
            if (shield != 0)
                DrawQuadFilled(sssss1, sssss2, sssss3, sssss4, shieldCol);
        }
    }
}

std::string process_model_name(char model_name[]) {
    if (strstr(model_name, "dummie")) {
        return "人机";
    } else if (strstr(model_name, "bangalore")) {
        return "班加罗尔";
    } else if (strstr(model_name, "fuse")) {
        return "暴雷";
    } else if (strstr(model_name, "ash")) {
        return "艾许";
    } else if (strstr(model_name, "madmaggie")) {
        return "疯马吉";
    } else if (strstr(model_name, "ballistic")) {
        return "弹道";
    } else if (strstr(model_name, "pathfinder")) {
        return "探路者";
    } else if (strstr(model_name, "octane")) {
        return "动力小子";
    } else if (strstr(model_name, "stim")) {
        return "动力小子";
    } else if (strstr(model_name, "revenant")) {
        return "亡灵";
    } else if (strstr(model_name, "nova")) {
        return "地平线";
    } else if (strstr(model_name, "horizon")) {
        return "地平线";
    } else if (strstr(model_name, "valkyrie")) {
        return "瓦尔基里";
    } else if (strstr(model_name, "bloodhound")) {
        return "寻血猎犬";
    } else if (strstr(model_name, "crypto")) {
        return "密客";
    } else if (strstr(model_name, "seer")) {
        return "希尔";
    } else if (strstr(model_name, "wraith")) {
        return "恶灵";
    } else if (strstr(model_name, "vantage")) {
        return "万蒂奇";
    } else if (strstr(model_name, "gibraltar")) {
        return "直布罗陀";
    } else if (strstr(model_name, "lifeline")) {
        return "命脉";
    } else if (strstr(model_name, "loba")) {
        return "罗芭";
    } else if (strstr(model_name, "holo")) {
        return "幻象";
    } else if (strstr(model_name, "mirage")) {
        return "幻象";
    } else if (strstr(model_name, "newcastle")) {
        return "纽卡斯尔";
    } else if (strstr(model_name, "conduit")) {
        return "导线管";
    } else if (strstr(model_name, "caustic")) {
        return "侵蚀";
    } else if (strstr(model_name, "wattson")) {
        return "沃特森";
    } else if (strstr(model_name, "rampart")) {
        return "兰伯特";
    } else if (strstr(model_name, "catalyst")) {
        return "催化剂";
    } else return model_name;
}