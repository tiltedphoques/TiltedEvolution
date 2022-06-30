#pragma once

struct AnimationGraphDescriptorManager;

struct AnimationGraphDescriptor_Master_Behavior
{
    static std::optional<uint32_t> TranslateThirdToFirstPerson(uint32_t aThirdPersonVar);

    AnimationGraphDescriptor_Master_Behavior(AnimationGraphDescriptorManager& aManager);
};
