#pragma once

#include <cstdint>
#include <array>

typedef struct {
    float x;
    float y;
    float z;
    float w;
} vec4_t;


typedef struct {
    float x;
    float y;
    float z;
} vec3_t;

typedef struct {
    float x;
    float y;
    vec3_t hit;
} TargetInfo;

extern "C" {

     //将角度转换为屏幕坐标 比如你需要绘制高抛雷的点就得调用这个
    vec4_t angles_to_screen(float local_camera_origin_x, float local_camera_origin_y,
        float local_camera_origin_z, float skynade_x,
        float skynade_y, float skynade_z);

    vec4_t skynade_angle(uint32_t weapon_id, uint32_t weapon_mod_bitfield,
        float weapon_projectile_scale,
        float weapon_projectile_speed,
        float local_view_origin_x, float local_view_origin_y, float local_view_origin_z, 
        float target_x,float target_y, float target_z,
        bool thermite, bool frag, bool arc,
        float target_off, float frag_off, float arc_off);

    //预测
    TargetInfo linear_predict(bool spine, float weapon_projectile_grav,
        float weapon_projectile_speed, uint32_t weapon_idx,
        float local_x, float local_y, float local_z,
        float origin_x, float origin_y, float origin_z,
        float bone_head_x, float bone_head_y, float bone_head_z,
        float bone_body_x, float bone_body_y, float bone_body_z,
        float vel_x, float vel_y,float vel_z);


}



