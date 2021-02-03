#pragma once

struct bhkCharRigidBodyController;

struct MiddleProcess
{
    uint8_t unk0[0xC0];
    float direction; // C0
    uint8_t unkC4[0x3D0 - 0xC4];
    bhkCharRigidBodyController* rigidBodyController;
};

static_assert(offsetof(MiddleProcess, direction) == 0xC0);
static_assert(offsetof(MiddleProcess, rigidBodyController) == 0x3D0);
