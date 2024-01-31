#pragma once
#ifdef MODDED_BEHAVIOR_COMPATIBILITY

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
    boolean failListed(uint64_t hash);
    void failList(uint64_t hash);

    void Init();
    void Debug();

  private:
    static BehaviorVar* single;
    uint64_t invocations = 0;

    Replacer* loadReplacerFromDir(std::string aDir);

    std::vector<Replacer> behaviorPool; // Pool for loaded behaviours
    std::map<uint64_t, std::chrono::steady_clock::time_point> failedBehaviors;
};

extern const AnimationGraphDescriptor* BehaviorVarPatch(BSAnimationGraphManager* apManager, Actor* apActor);


#endif MODDED_BEHAVIOR_COMPATIBILITY
