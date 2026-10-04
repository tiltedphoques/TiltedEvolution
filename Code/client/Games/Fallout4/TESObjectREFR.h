#pragma once

#include <Games/Primitives.h>
#include <NetImmerse/NiPoint3.h>
#include <NetImmerse/NiPointer.h>
#include <Forms/TESForm.h>

// FO4 stores smart-pointer-like refs in these slots; a raw pointer has the
// same ABI and the client only ever reads through them.
template <class T> struct BSTSmartPointer
{
    T* object{nullptr};
    T* operator->() const { return object; }
    operator T*() const { return object; }
};
#include <NetImmerse/BSFaceGenNiNode.h>
#include "ExtraData.h"
#include <ExtraData/ExtraContainerChanges.h>
#include <Games/Animation/IAnimationGraphManagerHolder.h>
#include <Games/Misc/Lock.h>
#include <Games/Magic/MagicSystem.h>
#include <Magic/MagicCaster.h>
#include <Magic/MagicTarget.h>
#include <Structs/Inventory.h>
#include <ExtraData/ExtraDataList.h>

struct AnimationVariables;
struct TESWorldSpace;
struct TESBoundObject;
struct TESActorBase;
struct TESContainer;
struct TESObjectCELL;

enum class ITEM_REMOVE_REASON
{
    kRemove,
    kSteal,
    kSelling,
    kDropping,
    kStoreInContainer,
    kStoreInTeammate
};

// Fallout 4's reference payload: rotation, position and base object.
struct OBJ_REFR
{
    NiPoint3A angle;            // 00
    NiPoint3A location;         // 10
    TESBoundObject* objectReference; // 20
};
static_assert(sizeof(OBJ_REFR) == 0x30);

struct LOADED_REF_DATA
{
    void* handleList;           // 00
    NiPointer<NiAVObject> data3D; // 08
    void* currentWaterType;     // 10
    float relevantWaterHeight;  // 18
    float cachedRadius;         // 1C
    uint16_t flags;             // 20
    int16_t underwaterCount;    // 22
};

struct RemoveItemData
{
    uint8_t stackData[0x20];              // BSTSmallArray<uint32_t, 4>
    TESBoundObject* object;               // 20
    int32_t count;                        // 28
    ITEM_REMOVE_REASON reason;            // 2C
    TESObjectREFR* a_otherContainer;      // 30
    const NiPoint3* dropLoc;              // 38
    const NiPoint3* rotate;               // 40
};
static_assert(sizeof(RemoveItemData) == 0x48);

struct BGSInventoryList;

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

    enum OpenState : uint8_t
    {
        kNone = 0,
        kOpen,
        kOpening,
        kClosed,
        kClosing,
    };

    static TESObjectREFR* GetByHandle(uint32_t aHandle) noexcept;
    static uint32_t* GetNullHandle() noexcept;

    static void GetItemFromExtraData(Inventory::Entry& arEntry, ExtraDataList* apExtraDataList) noexcept;
    static ExtraDataList* GetExtraDataFromItem(const Inventory::Entry& arEntry) noexcept;

    virtual void sub_4A();
    virtual void sub_4B();
    virtual void sub_4C();
    virtual void sub_4D();
    virtual void sub_4E();
    virtual void sub_4F();
    virtual void Update3DPosition(bool abWarp);
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
    virtual BSPointerHandle<TESObjectREFR> RemoveItem(RemoveItemData& arData);
    virtual void sub_6E();
    virtual void sub_6F();
    virtual void sub_71();
    virtual void sub_70();
    virtual void sub_72();
    virtual void sub_73();
    virtual void sub_74();
    virtual void sub_75();
    virtual void sub_76();
    virtual void sub_77();
    virtual void sub_78();
    virtual void sub_79();
    virtual void AddObjectToContainer(TESBoundObject* apObj, ExtraDataList* apExtra, int32_t aCount, TESObjectREFR* apOldContainer);
    virtual void sub_7B();
    virtual MagicCaster* GetMagicCaster(MagicSystem::CastingSource aeSource);
    virtual MagicTarget* GetMagicTarget();
    virtual void sub_7E();
    virtual void sub_7F();
    virtual void sub_80();
    virtual BSFaceGenNiNode* GetFaceNodeSkinned();
    virtual BSFaceGenNiNode* GetFaceNode();
    virtual void sub_83();
    virtual bool DetachHavok(NiAVObject* apObj3D);
    virtual void sub_85();
    virtual void sub_86();
    virtual void sub_87();
    virtual void Set3D(NiAVObject* apObject, bool aQueue3DTasks);
    virtual void sub_89();
    virtual void sub_8A();
    virtual NiAVObject* Get3D() const;
    virtual NiAVObject* Get3D(bool aFirstPerson) const;
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
    virtual void sub_A2();
    virtual void sub_A1();
    virtual void sub_A3();
    virtual void sub_A4();
    virtual void sub_A5();
    virtual void sub_A6();
    virtual void sub_A7();
    virtual void sub_A8();
    virtual void sub_A9();
    virtual void SetObjectReference(TESBoundObject* apObject);
    virtual void sub_AB();
    virtual void sub_AC();
    virtual void sub_AD();
    virtual void sub_AE();
    virtual void sub_AF();
    virtual void Disable();
    virtual void ResetInventory(bool abLeveledOnly);
    virtual void sub_B2();
    virtual void sub_B4();
    virtual void sub_B3();
    virtual void sub_B6();
    virtual void sub_B5();
    virtual void sub_B7();
    virtual void sub_B8();
    virtual void sub_B9();
    virtual void sub_BA();
    virtual void sub_BB();
    virtual void sub_BC();
    virtual void sub_BD();
    virtual TESObjectCELL* GetSaveParentCell() const;
    virtual void SetParentCell(TESObjectCELL* apParentCell);
    virtual bool IsDead(bool abNotEssential) const;
    virtual void sub_C1();
    virtual void sub_C2();
    virtual void sub_C3();
    virtual void sub_C4();
    virtual void sub_C5();

    void SetRotation(float aX, float aY, float aZ) noexcept;
    void SetLeveledCreature(TESActorBase* apOriginalBase, TESActorBase* apTemplateA) noexcept;

    BSPointerHandle<TESObjectREFR> GetHandle() const noexcept;
    uint32_t GetCellId() const noexcept;
    TESWorldSpace* GetWorldSpace() const noexcept;
    ExtraContainerChanges::Data* GetContainerChanges() const noexcept;
    ExtraDataList* GetExtraDataList() noexcept;
    Lock* GetLock() const noexcept;
    TESContainer* GetContainer() const noexcept;
    int64_t GetItemCountInInventory(TESForm* apItem) const noexcept;
    TESObjectCELL* GetParentCellEx() const noexcept;

    void SaveAnimationVariables(AnimationVariables& aWriter) const noexcept;
    void LoadAnimationVariables(const AnimationVariables& aReader) const noexcept;
    uint32_t GetAnimationVariableInt(BSFixedString* apVariableName) noexcept;

    void RemoveAllItems() noexcept;
    Vector<uint32_t> RemoveNonQuestItems(Inventory& aCurrentInventory) noexcept;
    void Delete() const noexcept;
    void Enable() const noexcept;
    void MoveTo(TESObjectCELL* apCell, const NiPoint3& acPosition) const noexcept;
    void PayGold(int32_t aAmount) noexcept;
    void PayGoldToContainer(TESObjectREFR* pContainer, int32_t aAmount) noexcept;
    bool SendAnimationEvent(BSFixedString* apEventName) noexcept;

    bool Activate(TESObjectREFR* apActivator, uint8_t aUnk1, TESBoundObject* apObjectToGet, int32_t aCount, char aDefaultProcessing) noexcept;

    bool PlayAnimationAndWait(BSFixedString* apAnimation, BSFixedString* apEventName) noexcept;
    bool PlayAnimation(BSFixedString* apEventName) noexcept;

    Lock* CreateLock() noexcept;
    void LockChange() noexcept;

    const float GetHeight() noexcept;
    void EnableImpl() noexcept;
    OpenState GetOpenState() noexcept;

    Inventory GetInventory() const noexcept;
    Inventory GetInventory(std::function<bool(TESForm&)> aFilter) const noexcept;
    Inventory GetArmor() const noexcept;
    Inventory GetWornArmor() const noexcept;

    bool IsItemInInventory(uint32_t aFormID) const noexcept;

    void SetInventory(const Inventory& acContainer) noexcept;
    void SetInventoryRetainingQuestItems(Inventory& aCurrentInventory, const Inventory& acSourceInventory) noexcept;
    void AddOrRemoveItem(const Inventory::Entry& arEntry, bool aIsSettingInventory = false) noexcept;
    void UpdateItemList(TESForm* pUnkForm) noexcept;

    BSHandleRefObject handleRefObject; // 0x20
    uint8_t pad30[0xB8 - 0x30];        // event sinks, graph holder, keyword form, AV owner, AV event source

    TESObjectCELL* parentCell;         // 0xB8
    OBJ_REFR data;                     // 0xC0: rotation, position, base form
    LOADED_REF_DATA* loadedState;      // 0xF0
    BGSInventoryList* inventoryList;   // 0xF8
    BSTSmartPointer<ExtraDataList> extraData; // 0x100
    uint16_t scale;                    // 0x108
    uint8_t modelState;                // 0x10A
    bool predestroyed;                 // 0x10B
    uint8_t pad10C[4];
};

static_assert(sizeof(TESObjectREFR) == 0x110);
static_assert(offsetof(TESObjectREFR, parentCell) == 0xB8);
static_assert(offsetof(TESObjectREFR, loadedState) == 0xF0);
static_assert(offsetof(TESObjectREFR, extraData) == 0x100);
