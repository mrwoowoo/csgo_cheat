#pragma once
#include <cstddef>
#include "XorStr.h"
#include "Overlay.h"
#include "csgo.hpp"
#include "Memory.h"
#define _USE_MATH_DEFINES
#include <math.h>

#define xor _XOR_

struct Vec2
{
    float x, y;
};

struct Vec3
{
    float x, y, z;
    Vec3 operator+ (Vec3 v)
    {
        Vec3 r;
        r.x = x + v.x;
        r.y = y + v.y;
        r.z = z + v.z;
        return r;
    }
    Vec3 operator- (Vec3 v)
    {
        Vec3 r;
        r.x = x - v.x;
        r.y = y - v.y;
        r.z = z - v.z;
        return r;
    }
    Vec3 operator* (float v)
    {
        Vec3 r;
        r.x = x * v;
        r.y = y * v;
        r.z = z * v;
        return r;
    }
    Vec3 to_angle()
    {
        Vec3 angles;
        if (y == 0.0f && x == 0.0f)
        {
            angles.x = (z > 0.0f) ? 270.0f : 90.0f;
            angles.y = 0.0f;
        }
        else
        {
            angles.x = atan2(-z, sqrtf(x * x + y * y)) * -180.f / M_PI;
            angles.y = atan2(y, x) * 180.f / M_PI;

            if (angles.y > 90)
                angles.y -= 180;
            else if (angles.y < 90)
                angles.y += 180;
            else if (angles.y == 90)
                angles.y = 0;
        }

        angles.z = 0.0f;

        return angles;
    }
    void Clamp()
    {
        if (x > 180)
            x -= 360;
        else if (x < -180)
            x += 360;

        if (y > 180)
            y -= 360;
        else if (y < -180)
            y += 360;

        if (x < -89)
            x = -89;

        if (x > 89)
            x = 89;

        while (y < -180.0f)
            y += 360.0f;

        while (y > 180.0f)
            y -= 360.0f;

        z = 0;
    }
};

struct Vec4
{
    float x, y, z, w;
};


class Hack
{
public:
	int Run();
    uintptr_t GetEntity(uintptr_t id);
    uintptr_t GetLocal();
    Vec3 GetPos(uintptr_t id);
    Vec3 GetBonePos(uintptr_t id, int Bone);
    bool IsDormant(uintptr_t id);
    int GetHealth(uintptr_t id);
    int GetTeam(uintptr_t id);
    int GetFlag(uintptr_t id);
    int GetCrossId(uintptr_t id);
    void SetViewAngle(Vec3 angle);
    Vec3 GetViewAngle();
    Vec3 GetCamera(uintptr_t id);
    bool WorldToScreen(Vec3 pos, Vec2& screen);
    void ShowMenu();
    static void TopMost(HWND hWnd);
    static void TriggerBot(Hack* hack);
    static void AimBot(Hack* hack);
    void Esp();
    static void Control(Hack* hack);

public:
    uintptr_t clinetAddress = 0;
    uintptr_t engineAddress = 0;
	Memory mem;
    Overlay ov;
	int width = 0;
	int height = 0;
    static bool running;
    bool triggerBot = false;
    bool esp = false;
    bool aimBot = false;
    ImColor red = ImColor(255, 0, 0);
    ImColor green = ImColor(0, 255, 0);
    ImColor blue = ImColor(0, 0, 255);
    ImColor white = ImColor(255, 255, 255);
    uintptr_t bestId = 0;
};

inline uintptr_t Hack::GetEntity(uintptr_t id)
{
    return mem.Read<uintptr_t>(clinetAddress + hazedumper::signatures::dwEntityList + 0x10 * id);
}

inline Vec3 Hack::GetPos(uintptr_t id)
{
    return mem.Read<Vec3>(id + hazedumper::netvars::m_vecOrigin);
}

inline Vec3 Hack::GetBonePos(uintptr_t id, int Bone)
{
    uintptr_t BoneMatrix = mem.Read<uintptr_t>(id + hazedumper::netvars::m_dwBoneMatrix);
    if (!BoneMatrix)
    {
        return { 0.f, 0.f, 0.f };
    }
    Vec3 BonePos;
    BonePos.x = mem.Read<float>(BoneMatrix + 0x30 * Bone + 0x0c);
    BonePos.y = mem.Read<float>(BoneMatrix + 0x30 * Bone + 0x1c);
    BonePos.z = mem.Read<float>(BoneMatrix + 0x30 * Bone + 0x2c);
    return BonePos;
}

inline bool Hack::IsDormant(uintptr_t id)
{
    return mem.Read<bool>(id + hazedumper::signatures::m_bDormant);
}

inline int Hack::GetHealth(uintptr_t id)
{
    return mem.Read<int>(id + hazedumper::netvars::m_iHealth);
}

inline uintptr_t Hack::GetLocal()
{
    return mem.Read<uintptr_t>(clinetAddress + hazedumper::signatures::dwLocalPlayer);
}

inline int Hack::GetTeam(uintptr_t id)
{
    return mem.Read<int>(id + hazedumper::netvars::m_iTeamNum);
}

inline int Hack::GetFlag(uintptr_t id)
{
    return mem.Read<int>(id + hazedumper::netvars::m_fFlags);
}
 
inline int Hack::GetCrossId(uintptr_t id)
{
    return mem.Read<int>(id + hazedumper::netvars::m_iCrosshairId);
}

inline Vec3 Hack::GetCamera(uintptr_t id)
{
    return mem.Read<Vec3>(id + hazedumper::netvars::m_vecViewOffset);
}

inline bool Hack::WorldToScreen(Vec3 pos, Vec2& screen)
{
    float ViewMatrix[16];
    mem.ReadBuffer(clinetAddress + hazedumper::signatures::dwViewMatrix, ViewMatrix, sizeof(ViewMatrix));
    Vec4 ClipCoords;
    ClipCoords.x = pos.x * ViewMatrix[0] + pos.y * ViewMatrix[1] + pos.z * ViewMatrix[2] + ViewMatrix[3];
    ClipCoords.y = pos.x * ViewMatrix[4] + pos.y * ViewMatrix[5] + pos.z * ViewMatrix[6] + ViewMatrix[7];
    ClipCoords.z = pos.x * ViewMatrix[8] + pos.y * ViewMatrix[9] + pos.z * ViewMatrix[10] + ViewMatrix[11];
    ClipCoords.w = pos.x * ViewMatrix[12] + pos.y * ViewMatrix[13] + pos.z * ViewMatrix[14] + ViewMatrix[15];
    if (ClipCoords.w < 0.1f)
    {
        return false;
    }
    Vec3 NDC;
    NDC.x = ClipCoords.x / ClipCoords.w;
    NDC.y = ClipCoords.y / ClipCoords.w;
    NDC.z = ClipCoords.z / ClipCoords.w;

    screen.x = (width / 2 * NDC.x) + width / 2;
    screen.y = -(height / 2 * NDC.y) + height / 2;
    return true;
}

inline void Hack::SetViewAngle(Vec3 angle)
{
    uintptr_t clientState = mem.Read<uintptr_t>(engineAddress + hazedumper::signatures::dwClientState);
    mem.Write<Vec3>(clientState + hazedumper::signatures::dwClientState_ViewAngles, angle);
}

inline Vec3 Hack::GetViewAngle()
{
    uintptr_t clientState = mem.Read<uintptr_t>(engineAddress + hazedumper::signatures::dwClientState);
    return mem.Read<Vec3>(clientState + hazedumper::signatures::dwClientState_ViewAngles);
}