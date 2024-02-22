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
    std::map< uint64_t, BehaviorVars> m_map;

    // Singleton
    BehaviorVarsMap(){};
    BehaviorVarsMap(BehaviorVarsMap const&) = delete;
    void operator=(BehaviorVarsMap const&)  = delete;

  public:
    uint32_t find(const uint64_t key, const std::string name);

    std::string find(const uint64_t key, const uint32_t value);

    void Register(const BehaviorVars map);

    static BehaviorVarsMap& getInstance();
};

namespace BehaviorOrig
{
    extern void BehaviorOrigInit();
};
