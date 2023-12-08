#pragma once

#include "TESHandleForm.h"
#include "BSCore/BSUntypedPointerHandle.h"
#include <BSCore/NiPoint3.h>

struct TESObjectCELL;
struct BGSInventoryList;
struct TESWorldSpace;
struct REFR_LOCK;
struct TESObjectREFR;

struct REFR_ACTIVATE_DATA
{
    TESObjectREFR* pActivatedRef;
    TESObjectREFR* pActionRef;
    TESForm* pObjectToGet;
    unsigned int count;
    bool defaultProcessingOnly;
    bool fromScript;
    bool looping;
};

static_assert(sizeof(REFR_ACTIVATE_DATA) == 0x20);

// TODO: multiple inheritance
struct TESObjectREFR : TESHandleForm
{
    static TESObjectREFR* GetByHandle(uint32_t aHandle) noexcept;

    void Delete() noexcept;
    bool ActivateRef(REFR_ACTIVATE_DATA* apData) noexcept;
    void Reset() noexcept; // TODO: is ResetReference a better alternative?

    BSPointerHandle<TESObjectREFR> GetHandle() noexcept;
    TESWorldSpace* GetWorldSpace() noexcept;
    REFR_LOCK* GetLock() noexcept;

    REFR_LOCK* AddLock() noexcept;
    void AddLockChange() noexcept;

    // Inventory
    void RemoveAllItems(TESObjectREFR* apTargetContainer = nullptr, bool aChangeOwnership = false, bool aIgnoreWeightLimit = false) noexcept;
    #if 0
    //  TODO: some stuff with FormOrInventoryObj, reverse that first
    void AddItem() noexcept;
    void RemoveItem() noexcept;
    //  TODO: some stuff with RefrOrInventoryObj, reverse that first
    void AttachMod() noexcept;
    void RemoveMod() noexcept;
    #endif

    virtual void sub_62();
    virtual void sub_63();
    virtual void sub_64();
    virtual void sub_65();
    virtual void sub_66();
    virtual void sub_67();
    virtual void sub_68();
    virtual void sub_69();
    virtual void sub_6A();
    virtual void sub_6B();
    virtual void sub_6C();
    virtual void sub_6D();
    virtual void sub_6E();
    virtual void sub_6F();
    virtual void sub_70();
    virtual void sub_71();
    virtual void sub_72();
    virtual void sub_73();
    virtual void sub_74();
    virtual void sub_75();
    virtual void sub_76();
    virtual void sub_77();
    virtual void sub_78();
    virtual void sub_79();
    virtual void sub_7A();
    virtual void sub_7B();
    virtual void sub_7C();
    virtual void sub_7D();
    virtual void sub_7E();
    virtual void sub_7F();
    virtual void sub_80();
    virtual void sub_81();
    virtual void sub_82();
    virtual void sub_83();
    virtual void sub_84();
    virtual void sub_85();
    virtual void sub_86();
    virtual void sub_87();
    virtual void sub_88();
    virtual void sub_89();
    virtual void sub_8A();
    virtual void sub_8B();
    virtual void sub_8C();
    virtual void sub_8D();
    virtual void sub_8E();
    virtual void sub_8F();
    virtual void sub_90();
    virtual void sub_91();
    virtual void sub_92();
    virtual void sub_93();
    virtual void sub_94();
    virtual void sub_95();
    virtual void sub_96();
    virtual void sub_97();
    virtual void sub_98();
    virtual void sub_99();
    virtual void sub_9A();
    virtual void sub_9B();
    virtual void sub_9C();
    virtual void sub_9D();
    virtual void sub_9E();
    virtual void sub_9F();
    virtual void sub_A0();
    virtual void sub_A1();
    virtual void sub_A2();
    virtual void sub_A3();
    virtual void sub_A4();
    virtual void sub_A5();
    virtual void sub_A6();
    virtual void sub_A7();
    virtual void sub_A8();
    virtual void sub_A9();
    virtual void sub_AA();
    virtual void sub_AB();
    virtual void Get3D(bool abFirstPerson);
    virtual void Get3D();
    virtual void sub_AE();
    virtual void sub_AF();
    virtual void sub_B0();
    virtual void sub_B1();
    virtual void sub_B2();
    virtual void sub_B3();
    virtual void sub_B4();
    virtual void sub_B5();
    virtual void sub_B6();
    virtual void sub_B7();
    virtual void sub_B8();
    virtual void sub_B9();
    virtual void sub_BA();
    virtual void sub_BB();
    virtual void sub_BC();
    virtual void sub_BD();
    virtual void sub_BE();
    virtual void sub_BF();
    virtual void sub_C0();
    virtual void sub_C1();
    virtual void sub_C2();
    virtual void sub_C3();
    virtual void sub_C4();
    virtual void sub_C5();
    virtual void sub_C6();
    virtual void sub_C7();
    virtual void sub_C8();
    virtual void sub_C9();
    virtual void sub_CA();
    virtual void sub_CB();
    virtual void sub_CC();
    virtual void sub_CD();
    virtual void sub_CE();
    virtual void sub_CF();
    virtual void sub_D0();
    virtual void sub_D1();
    virtual void sub_D2();
    virtual void sub_D3();
    virtual void sub_D4();
    virtual void sub_D5();
    virtual void sub_D6();
    virtual void sub_D7();
    virtual void sub_D8();
    virtual void sub_D9();
    virtual void sub_DA();
    virtual void sub_DB();
    virtual void sub_DC();
    virtual void sub_DD();
    virtual void sub_DE();
    virtual void sub_DF();
    virtual void sub_E0();
    virtual void sub_E1();
    virtual void sub_E2();
    virtual void sub_E3();
    virtual void sub_E4();
    virtual void sub_E5();
    virtual void sub_E6();
    virtual void sub_E7();
    virtual void sub_E8();
    virtual void sub_E9();
    virtual void sub_EA();
    virtual void sub_EB();
    virtual void sub_EC();
    virtual void sub_ED();
    virtual void sub_EE();
    virtual void sub_EF();
    virtual void sub_F0();
    virtual void sub_F1();
    virtual void sub_F2();
    virtual void sub_F3();
    virtual void sub_F4();
    virtual void sub_F5();
    virtual void sub_F6();
    virtual void sub_F7();
    virtual void sub_F8();
    virtual void sub_F9();
    virtual void sub_FA();
    virtual void sub_FB();
    virtual void sub_FC();
    virtual void sub_FD();
    virtual void ResetInventory(bool aLeveledonly = false);
    virtual void sub_FF();
    virtual void sub_100();
    virtual void sub_101();
    virtual void sub_102();
    virtual void sub_103();
    virtual void sub_104();
    virtual void sub_105();
    virtual void sub_106();
    virtual void sub_107();
    virtual void sub_108();
    virtual void sub_109();
    virtual void sub_10A();
    virtual void sub_10B();
    virtual void SetParentCell(TESObjectCELL* apParentCell);
    virtual void IsDead(bool abNotEssential);
    virtual void sub_10E();
    virtual void sub_10F();
    virtual void sub_110();
    virtual void sub_111();
    virtual void sub_112();
    virtual void sub_113();
    virtual void sub_114();
    virtual void sub_115();
    virtual void sub_116();
    virtual void sub_117();
    virtual void sub_118();
    virtual void sub_119();
    virtual void sub_11A();
    virtual void sub_11B();
    virtual void sub_11C();
    virtual void sub_11D();
    virtual void sub_11E();
    virtual void sub_11F();
    virtual void sub_120();
    virtual void sub_121();
    virtual void sub_122();
    virtual void sub_123();
    virtual void sub_124();
    virtual void sub_125();
    virtual void sub_126();
    virtual void sub_127();
    virtual void sub_128();
    virtual void sub_129();
    virtual void sub_12A();
    virtual void sub_12B();
    virtual void sub_12C();
    virtual void sub_12D();
    virtual void sub_12E();
    virtual void sub_12F();

    uint8_t unk38[0xA0 - 0x38];
    NiPoint3 rotation;
    uint32_t padAC;
    NiPoint3 position;
    uint32_t padBC;
    TESForm* pBaseForm;
    uint64_t unkC8;
    BGSInventoryList* pInventoryList;
    uint64_t maybeInventoryLock;
    TESObjectCELL* pParentCell;
    void* pLoadedState;
    uint64_t maybeLoadedStateLock;
    uint8_t unkE8[0x110 - 0xF8];
};

static_assert(sizeof(TESObjectREFR) == 0x110);
