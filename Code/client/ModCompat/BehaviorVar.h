#pragma once

struct BehaviorVar
{
    struct Replacer
    {
        uint64_t orgHash;
        uint64_t newHash;
        std::vector<std::string> syncBooleanVar;
        std::vector<std::string> syncFloatVar;
        std::vector<std::string> syncIntegerVar;
    };

    static BehaviorVar* Get();

    void Init();
    void ReplaceDescriptors();
    void Debug();

  private:
    static BehaviorVar* single;

    Replacer* loadReplacerFromDir(std::string aDir);

    std::vector<Replacer> behaviorPool; // Pool for loaded behaviours
};
