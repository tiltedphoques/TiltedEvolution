#pragma once

#include <Games/Primitives.h>
#include <Forms/TESForm.h>
#include <NetImmerse/BSFaceGenNiNode.h>
#include "ExtraData.h"
#include <ExtraData/ExtraContainerChanges.h>
#include <Games/Animation/IAnimationGraphManagerHolder.h>
#include <Games/Misc/Lock.h>
#include <Games/Magic/MagicSystem.h>
#include <Magic/MagicCaster.h>
#include <Structs/Inventory.h>
#include <ExtraData/ExtraDataList.h>

struct AnimationVariables;
struct TESWorldSpace;
struct TESBoundObject;
struct TESContainer;

enum class ITEM_REMOVE_REASON
{
    kRemove,
    kSteal,
    kSelling,
    kDropping,
    kStoreInContainer,
    kStoreInTeammate
};

struct TESObjectREFR : TESForm
{
    enum ChangeFlags : uint32_t
    {
        CHANGE_REFR_MOVE = 1 << 1,
        CHANGE_REFR_HAVOK_MOVE = 1 << 2,
        CHANGE_REFR_CELL_CHANGED = 1 << 3,
        CHANGE_REFR_SCALE = 1 << 4,
        CHANGE_REFR_INVENTORY = 1 << 5,
        CHANGE_REFR_EXTRA_OWNERSHIP = 1 << 6,
        CHANGE_REFR_BASEOBJECT = 1 << 7,
        CHANGE_REFR_PROMOTED = 1 << 25,
        CHANGE_REFR_EXTRA_ACTIVATING_CHILDREN = 1 << 26,
        CHANGE_REFR_LEVELED_INVENTORY = 1 << 27,
        CHANGE_REFR_ANIMATION = 1 << 28,
        CHANGE_REFR_EXTRA_ENCOUNTER_ZONE = 1 << 29,
        CHANGE_REFR_EXTRA_CREATED_ONLY = 1 << 30,
        CHANGE_REFR_EXTRA_GAME_ONLY = 1u << 31,
    };

    static TESObjectREFR* GetByHandle(uint32_t aHandle) noexcept;
    static uint32_t* GetNullHandle() noexcept;

    static void GetItemFromExtraData(Inventory::Entry& arEntry, ExtraDataList* apExtraDataList) noexcept;
    static ExtraDataList* GetExtraDataFromItem(const Inventory::Entry& arEntry) noexcept;

    virtual void sub_39();
    virtual void sub_3A();
    virtual void sub_3B();
    virtual void sub_3C();
    virtual void sub_3D();
    virtual void sub_3E();
    virtual void sub_3F();
    virtual void sub_40();
    virtual void sub_41();
    virtual void sub_42();
    virtual void sub_43();
    virtual void sub_44();
    virtual void sub_45();
    virtual void sub_46();
    virtual void sub_47();
    virtual void sub_48();
    virtual void sub_49();
    virtual void sub_4A();
    virtual void sub_4B();
    virtual void sub_4C();
    virtual void sub_4D();
    virtual void sub_4E();
    virtual void StopCurrentDialogue(bool aForce);
    virtual void sub_50();
    virtual void sub_51();
    virtual void sub_52();
    virtual void sub_53();
    virtual void sub_54();
    virtual void sub_55();
    virtual BSPointerHandle<TESObjectREFR> RemoveItem(TESBoundObject* apItem, int32_t aCount, ITEM_REMOVE_REASON aReason, ExtraDataList* apExtraList, TESObjectREFR* apMoveToRef, const NiPoint3* apDropLoc = nullptr, const NiPoint3* apRotate = nullptr);
    virtual void sub_57();
    virtual void sub_58();
    virtual void sub_59();
    virtual void AddObjectToContainer(TESBoundObject* apObj, ExtraDataList* aspExtra, int32_t aicount, TESObjectREFR* apOldContainer);
    virtual void sub_5B();
    virtual MagicCaster* GetMagicCaster(MagicSystem::CastingSource aeSource);
    virtual void sub_5D();
    virtual void sub_5E();
    virtual void sub_5F();
    virtual void sub_60();
    virtual BSFaceGenNiNode* GetFaceGenNiNode();
    virtual void sub_62();
    virtual void sub_63();
    virtual void sub_64();
    virtual void DetachHavok();
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
    virtual NiNode* GetNiNode();
    virtual void sub_71();
    virtual void sub_72();
    virtual void sub_73();
    virtual NiPoint3 GetBoundMin();
    virtual NiPoint3 GetBoundMax();
    virtual void sub_76();
    virtual void sub_77();
    virtual void sub_78();
    virtual void sub_79();
    virtual void sub_7A();
    virtual void sub_7B();
    virtual void sub_7C();
    virtual void sub_7D();
    virtual void* sub_7E(bool aUnk);
    virtual void sub_7F();
    virtual void sub_80();
    virtual void sub_81();
    virtual void sub_82();
    virtual void sub_83();
    virtual void SetBaseForm(TESForm* apForm);
    virtual void sub_85();
    virtual void sub_86();
    virtual void sub_87();
    virtual void sub_88();
    virtual void DisableImpl();
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
    virtual struct TESObjectCELL* GetParentCell() const;
    virtual void SetParentCell(struct TESObjectCELL* apParentCell);
    virtual bool CheckSomeFlags(bool aUnk);
    virtual void sub_9A();
    virtual void sub_9B();

    void SetRotation(float aX, float aY, float aZ) noexcept;

    uint32_t GetCellId() const noexcept;
    TESWorldSpace* GetWorldSpace() const noexcept;
    ExtraContainerChanges::Data* GetContainerChanges() const noexcept;
    ExtraDataList* GetExtraDataList() noexcept;
    Lock* GetLock() noexcept;
    TESContainer* GetContainer() const noexcept;
    int64_t GetItemCountInInventory(TESForm* apItem) const noexcept;
    TESObjectCELL* GetParentCellEx() const noexcept;

    void SaveAnimationVariables(AnimationVariables& aWriter) const noexcept;
    void LoadAnimationVariables(const AnimationVariables& aReader) const noexcept;

    void RemoveAllItems() noexcept;
    void Delete() const noexcept;
    void Disable() const noexcept;
    void Enable() const noexcept;
    void MoveTo(TESObjectCELL* apCell, const NiPoint3& acPosition) const noexcept;
    void PayGold(int32_t aAmount) noexcept;
    void PayGoldToContainer(TESObjectREFR* pContainer, int32_t aAmount) noexcept;

    void Activate(TESObjectREFR* apActivator, uint8_t aUnk1, TESBoundObject* apObjectToGet, int32_t aCount, char aDefaultProcessing) noexcept;

    bool PlayAnimationAndWait(BSFixedString* apAnimation, BSFixedString* apEventName) noexcept;
    bool PlayAnimation(BSFixedString* apEventName) noexcept;

    Lock* CreateLock() noexcept;
    void LockChange() noexcept;

    const float GetHeight() noexcept;
    void EnableImpl() noexcept;

    Inventory GetInventory() const noexcept;
    Inventory GetInventory(std::function<bool(TESForm&)> aFilter) const noexcept;
    Inventory GetArmor() const noexcept;
    Inventory GetWornArmor() const noexcept;

    bool IsItemInInventory(uint32_t aFormID) const noexcept;

    void SetInventory(const Inventory& acContainer) noexcept;
    void AddOrRemoveItem(const Inventory::Entry& arEntry, bool aIsSettingInventory = false) noexcept;
    void UpdateItemList(TESForm* pUnkForm) noexcept;

    BSHandleRefObject handleRefObject;
    uintptr_t unk1C;
    IAnimationGraphManagerHolder animationGraphHolder;
    TESForm* baseForm; // 40 SE
    NiPoint3 rotation;
    NiPoint3 position;
    TESObjectCELL* parentCell;
    void* loadedState;
    ExtraDataList extraData;
    BSRecursiveLock refLock;
    uint16_t scale;
    uint16_t referenceFlags;
};

static_assert(sizeof(TESObjectREFR) == 0x98);
static_assert(offsetof(TESObjectREFR, loadedState) == 0x68);
