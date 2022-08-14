
static const char kAppClassName[]{"SkyrimTogether"};
static const char kAppININame[]{"STR.INI"};
static const char kAppCustomININame[]{"STRCustom.ini"};
static const char kAppPrefININame[]{"STRPrefs.ini"};

static TiltedPhoques::Initializer s_mainHooks([]() {
    const VersionDbPtr<uint8_t> pAppClassName(380738);
    *reinterpret_cast<uintptr_t*>(pAppClassName.GetPtr()) = reinterpret_cast<uintptr_t>(&kAppClassName);

    const VersionDbPtr<uint8_t> pAppIniName(380740);
    *reinterpret_cast<uintptr_t*>(pAppIniName.GetPtr()) = reinterpret_cast<uintptr_t>(&kAppININame);

    const VersionDbPtr<uint8_t> pAppCustomININame(380741);
    *reinterpret_cast<uintptr_t*>(pAppCustomININame.GetPtr()) = reinterpret_cast<uintptr_t>(&kAppCustomININame);

    const VersionDbPtr<uint8_t> pAppPrefININame(380742);
    *reinterpret_cast<uintptr_t*>(pAppPrefININame.GetPtr()) = reinterpret_cast<uintptr_t>(&kAppPrefININame);
});
