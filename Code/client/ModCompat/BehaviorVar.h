#pragma once

#include <Structs/AnimationGraphDescriptorManager.h>

struct BehaviorVar
{
    struct Replacer
    {
        uint64_t origHash;
        uint64_t newHash;
        TiltedPhoques::String signatureVar;
        TiltedPhoques::String creatureName;
        TiltedPhoques::Vector<TiltedPhoques::String> syncBooleanVar;
        TiltedPhoques::Vector<TiltedPhoques::String> syncFloatVar;
        TiltedPhoques::Vector<TiltedPhoques::String> syncIntegerVar;
        bool operator==(const uint64_t& acRHS) const { return origHash == acRHS; }
    };

    static BehaviorVar* Get();
    const AnimationGraphDescriptor* Patch(BSAnimationGraphManager* apManager, Actor* apActor);
    bool FailListed(const uint64_t acHash);
    void FailList(const uint64_t acHash);

    void Init();
    void Debug();

private:
    static BehaviorVar* single;
    uint64_t invocations = 0;

    void SeedAnimationVariables(
        const uint64_t acHash,
        const AnimationGraphDescriptor* acpDescriptor,
        TiltedPhoques::Map<TiltedPhoques::String, uint32_t>& acReverseMap,
        TiltedPhoques::Set<uint32_t>& aBoolVars,
        TiltedPhoques::Set<uint32_t>& aFloatVars,
        TiltedPhoques::Set<uint32_t>& aIntVars);

    const TiltedPhoques::Vector<TiltedPhoques::String> TokenizeBehaviorSig(const TiltedPhoques::String& acSignature) const;
    const AnimationGraphDescriptor* ConstructModdedDescriptor(
        const uint64_t acNewHash,
        const Replacer& acReplacer,
        TiltedPhoques::Map<TiltedPhoques::String, uint32_t>& acReverseMap);

    TiltedPhoques::Vector<std::filesystem::path> LoadDirs(const std::filesystem::path& acPath);
    Replacer* LoadReplacerFromDir(const std::filesystem::path& acDir);
    TiltedPhoques::Vector<uint64_t> SignatureMatches(const uint64_t acHash, const TiltedPhoques::String& acSignature) const;

    TiltedPhoques::Vector<Replacer> behaviorPool; // Pool for loaded behaviors
    TiltedPhoques::Map<uint64_t, std::chrono::steady_clock::time_point> failedBehaviors;
};

extern const AnimationGraphDescriptor* BehaviorVarPatch(BSAnimationGraphManager* apManager, Actor* apActor);
