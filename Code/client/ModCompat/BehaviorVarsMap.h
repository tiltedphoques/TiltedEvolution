#pragma once


struct BehaviorVars
{
    uint64_t m_key;
    TiltedPhoques::Map<TiltedPhoques::String, uint32_t> m_nameMap;
    TiltedPhoques::Map<uint32_t, TiltedPhoques::String> m_valueMap;
};



class BehaviorVarsMap
{
  private:
    TiltedPhoques::Map<uint64_t, BehaviorVars> m_map;

    // Singleton
    BehaviorVarsMap(){};
    BehaviorVarsMap(BehaviorVarsMap const&) = delete;
    void operator=(BehaviorVarsMap const&)  = delete;

  public:
    const uint32_t find(const uint64_t acBehaviorVarKey, const TiltedPhoques::String acName);

    const TiltedPhoques::String find(const uint64_t acBehaviorVarKey, const uint32_t acValue);
    void Hashes(TiltedPhoques::Vector<uint64_t>& aHashes) const
    {
        aHashes.clear();
        for (auto& item : m_map)
            aHashes.push_back(item.first);
    };

    void Register(const BehaviorVars aMap);

    static BehaviorVarsMap& getInstance();
};

namespace BehaviorOrig
{
    extern void BehaviorOrigInit();
};
