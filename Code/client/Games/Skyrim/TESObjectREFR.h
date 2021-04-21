#pragma once

#include <Games/Primitives.h>
#include <Forms/TESForm.h>
#include <NetImmerse/BSFaceGenNiNode.h>
#include <Games/ExtraData.h>
#include <ExtraData/ExtraContainerChanges.h>
#include <Games/Animation/IAnimationGraphManagerHolder.h>

struct AnimationVariables;
struct TESWorldSpace;

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
    virtual struct ActorMagicCaster* CreateMagicCaster(uint32_t aId);
    virtual void sub_5D();
    virtual void sub_5E();
    virtual void sub_5F();
    virtual void sub_60();
    virtual BSFaceGenNiNode* GetFaceGenNiNode();
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
    virtual NiNode* GetNiNode();
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
    virtual struct TESObjectCELL* GetParentCell() const;
    virtual void SetParentCell(struct TESObjectCELL* apParentCell);
    virtual bool CheckSomeFlags(bool aUnk);
    virtual void sub_9A();
    virtual void sub_9B();

    void SetRotation(float aX, float aY, float aZ) noexcept;

    uint32_t GetCellId() const noexcept;
    TESWorldSpace* GetWorldSpace() const noexcept;
    ExtraContainerChanges::Data* GetContainerChanges() const noexcept;
    BSExtraDataList* GetExtraDataList() noexcept;

    void SaveInventory(BGSSaveFormBuffer* apBuffer) const noexcept;
    void SaveAnimationVariables(AnimationVariables& aWriter) const noexcept;
    String SerializeInventory() const noexcept;

    void LoadAnimationVariables(const AnimationVariables& aReader) const noexcept;
    void LoadInventory(BGSLoadFormBuffer* apBuffer) noexcept;
    void DeserializeInventory(const String& acData) noexcept;
    
    void RemoveAllItems() noexcept;
    void Delete() const noexcept;
    void Disable() const noexcept;
    void Enable() const noexcept;
    void MoveTo(TESObjectCELL* apCell, const NiPoint3& acPosition) const noexcept;

    void Activate(TESObjectREFR* apActivator, uint8_t aUnk1, int64_t aUnk2, int aUnk3, char aUnk4) noexcept;

    BSHandleRefObject handleRefObject;
    uintptr_t unk1C;
    IAnimationGraphManagerHolder animationGraphHolder;
    TESForm* baseForm; // 40 SE
    NiPoint3 rotation;
    NiPoint3 position;
    TESObjectCELL* parentCell;
    void* loadedState;
    BSExtraDataList extraData;
    BSRecursiveLock refLock;
    uint16_t scale;
    uint16_t referenceFlags;
};

POINTER_SKYRIMSE(uint32_t, s_nullHandle, 0x141EBEABC - 0x140000000);

static_assert(sizeof(TESObjectREFR) == 0x98);
static_assert(offsetof(TESObjectREFR, loadedState) == 0x68);
