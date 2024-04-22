#pragma once


struct BehaviorVars
{
    uint64_t m_key;
    std::map<std::string,  uint32_t> m_namemap;
    std::map<uint32_t,  std::string> m_valuemap;
};



class BehaviorVarsMap
{
  private:
    std::map<uint64_t, BehaviorVars> m_map;

    // Singleton
    BehaviorVarsMap(){};
    BehaviorVarsMap(BehaviorVarsMap const&) = delete;
    void operator=(BehaviorVarsMap const&)  = delete;

  public:
    uint32_t find(const uint64_t acKey, const std::string acName);

    std::string find(const uint64_t acKey, const uint32_t acValue);
    void hashes(std::vector<uint64_t>& aHashes) const
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
