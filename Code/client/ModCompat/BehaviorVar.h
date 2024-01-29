#pragma once

#include <Structs/AnimationGraphDescriptorManager.h>

struct BehaviorVar
{
    struct Replacer
    {
        uint64_t orgHash;
        uint64_t newHash;
        std::string signatureVar;
        std::vector<std::string> syncBooleanVar;
        std::vector<std::string> syncFloatVar;
        std::vector<std::string> syncIntegerVar;
    };

    static BehaviorVar* Get();
    const AnimationGraphDescriptor* Patch(BSAnimationGraphManager* apManager, Actor* apActor);
    void ReplaceDescriptor(BSAnimationGraphManager* apManager, Actor* apActor);

    void Init();
    void ReplaceDescriptors();
    void Debug();

  private:
    static BehaviorVar* single;
    uint64_t invocations = 0;

    Replacer* loadReplacerFromDir(std::string aDir);

    std::vector<Replacer> behaviorPool; // Pool for loaded behaviours
};

extern const AnimationGraphDescriptor* BehaviorVarPatch(BSAnimationGraphManager* apManager, Actor* apActor);
