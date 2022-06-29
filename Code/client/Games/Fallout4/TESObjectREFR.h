#pragma once

#include <ExtraData/ExtraData.h>
#include <Forms/TESForm.h>
#include <Games/Animation/IAnimationGraphManagerHolder.h>
#include <Forms/ActorValueInfo.h>
#include <Misc/ActorValueOwner.h>
#include <Games/Misc/Lock.h>
#include <Forms/BGSInventoryList.h>

struct NiNode;
struct BSFaceGenNiNode;
struct TESObjectCELL;
struct AnimationVariables;
struct TESBoundObject;
struct BGSEquipSlot;
struct ExtraDataList;

struct TESObjectREFR : TESForm
{
    static TESObjectREFR* GetByHandle(uint32_t aHandle) noexcept;
    static uint32_t* GetNullHandle() noexcept;

    enum ITEM_REMOVE_REASON : int32_t
    {
        IRR_NONE = 0x0,
        IRR_STEALING = 0x1,
        IRR_SELLING = 0x2,
        IRR_DROPPING = 0x3,
        IRR_STORE_IN_CONTAINER = 0x4,
        IRR_STORE_IN_TEAMMATE = 0x5,
    };

    struct RemoveItemData
    {
        uint8_t stackData[0x20]{};
        TESBoundObject* pObject{};
        int32_t iNumber{};
        ITEM_REMOVE_REASON eReason{};
        TESObjectREFR* pOtherContainer{};
        NiPoint3* pDropLoc{};
        NiPoint3* pRotate{};
    };

    virtual void sub_48();
    virtual void sub_49();
    virtual void sub_4A();
    virtual void sub_4B();
    virtual void sub_4C();
    virtual void sub_4D();
    virtual void sub_4E();
    virtual void sub_4F();
    virtual void sub_50();
    virtual void sub_51();
    virtual void sub_52();
    virtual void sub_53();
    virtual void sub_54();
    virtual void sub_55();
    virtual void sub_56();
    virtual void sub_57();
    virtual void sub_58();
    virtual void sub_59();
    virtual void sub_5A();
    virtual void sub_5B();
    virtual void sub_5C();
    virtual void sub_5D();
    virtual void sub_5E();
    virtual void sub_5F();
    virtual void sub_60();
    virtual void StopCurrentDialogue(bool aForce);
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
    virtual BSPointerHandle<TESObjectREFR> RemoveItem(RemoveItemData* arData);
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
    virtual void AddObjectToContainer(TESBoundObject* apObj, ExtraDataList* aspExtra, int32_t aicount, TESObjectREFR* apOldContainer);
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
    virtual NiNode* GetNiNode();
    virtual void sub_8D();
    virtual void sub_8E();
    virtual void sub_8F();
    virtual NiPoint3 GetBoundMin();
    virtual NiPoint3 GetBoundMax();
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
    virtual void sub_A7(void*);
    virtual void sub_A8();
    virtual void sub_A9();
    virtual void SetBaseForm(TESForm* apBaseForm);
    virtual void sub_AB();
    virtual void sub_AC();
    virtual void sub_AD();
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
    virtual TESObjectCELL* GetParentCell() const;
    virtual void SetParentCell(const TESObjectCELL* apCell);
    virtual void sub_C0();
    virtual void sub_C1();
    virtual void sub_C2();
    virtual void sub_C3();

    void SetRotation(float aX, float aY, float aZ) noexcept;

    uint32_t GetCellId() const noexcept;
    struct TESWorldSpace* GetWorldSpace() const noexcept;
    ExtraDataList* GetExtraDataList() noexcept;
    ActorValueInfo* GetActorValueInfo(uint32_t aId) const noexcept;
    Lock* GetLock() noexcept;
    const BGSEquipSlot* GetEquipSlot(uint32_t uiIndex) const noexcept;

    void SaveAnimationVariables(AnimationVariables& aWriter) const noexcept;
    void LoadAnimationVariables(const AnimationVariables& aReader) const noexcept;

    void RemoveAllItems() noexcept;
    void Delete() const noexcept;
    void Disable() const noexcept;
    void Enable() const noexcept;
    void MoveTo(TESObjectCELL* apCell, const NiPoint3& acPosition) const noexcept;
    void PayGold(int32_t aAmount) noexcept;
    void PayGoldToContainer(TESObjectREFR* pContainer, int32_t aAmount) noexcept;

    void Activate(TESObjectREFR* apActivator, TESBoundObject* apObjectToGet, int32_t aCount, bool aDefaultProcessing, bool aFromScript, bool aIsLooping) noexcept;

    Lock* CreateLock() noexcept;
    void LockChange() noexcept;

    const float GetHeight() noexcept;

    Inventory GetInventory() const noexcept;
    void SetInventory(const Inventory& acContainer) noexcept;
    void AddOrRemoveItem(const Inventory::Entry& arEntry) noexcept;

public:

    BSHandleRefObject handleRefObject;
    uint8_t unk20[0x48 - 0x30];
    IAnimationGraphManagerHolder animationGraphHolder;
    uint8_t unk50[0x58 - 0x50];
    ActorValueOwner actorValueOwner; // used when (flags >> 11) & 1 == 0 other wise uses the baseform's in TESActorBase
    void* unk60; 
    uint8_t unk68[0xB8 - 0x68];
    TESObjectCELL* parentCell;
    NiPoint3 rotation;
    uint32_t padCC; 
    NiPoint3 position;
    uint8_t unkDC[0xE0 - 0xDC];
    TESForm* baseForm;
    void* unkE8;
    void* loadState;
    BGSInventoryList* pInventoryList;
    ExtraDataList* extraData;
    uint64_t unk108;
};

static_assert(offsetof(TESObjectREFR, animationGraphHolder) == 0x48);
static_assert(offsetof(TESObjectREFR, baseForm) == 0xE0);
static_assert(offsetof(TESObjectREFR, position) == 0xD0);
static_assert(offsetof(TESObjectREFR, rotation) == 0xC0);
static_assert(offsetof(TESObjectREFR, parentCell) == 0xB8);
static_assert(offsetof(TESObjectREFR, pInventoryList) == 0xF8);
static_assert(offsetof(TESObjectREFR, extraData) == 0x100);
static_assert(sizeof(TESObjectREFR) == 0x110);
