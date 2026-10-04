#include <TiltedOnlinePCH.h>

#include <Forms/ActorValueInfo.h>

ActorValueInfo* ActorValueInfo::GetHealth() noexcept
{
    struct ActorValueTable
    {
        uint8_t pad0[0xD8];
        ActorValueInfo* health;
    };

    using TGetActorValues = ActorValueTable*();
    static VersionDbPtr<TGetActorValues> getActorValues(2189587);
    return getActorValues()->health;
}

ActorValueInfo* ActorValueInfo::Resolve(uint32_t aId) noexcept
{
    uint32_t offset;
    switch (aId)
    {
    case kAggression: offset = 0x10; break;
    case kConfidence: offset = 0x98; break;
    case kEnergy: offset = 0x310; break;
    case kMorality: offset = 0x158; break;
    case kAssistance: offset = 0x40; break;
    case kBlock: offset = 0x58; break;
    case kPickpocket: offset = 0x180; break;
    case kLockpicking: offset = 0x138; break;
    case kSneak: offset = 0x228; break;
    case kAlchemy: offset = 0x28; break;
    case kSpeechcraft: offset = 0x230; break;
    case kEnchanting: offset = 0xB8; break;
    case kHealth: offset = 0xD8; break;
    case kStamina: offset = 0x240; break;
    case kHealRate: offset = 0x2A0; break;
    case kSpeedMult: offset = 0x238; break;
    case kCarryWeight: offset = 0x80; break;
    case kCritChance: offset = 0xA0; break;
    case kMeleeDamage: offset = 0x150; break;
    case kUnarmedDamage: offset = 0x260; break;
    case kMass: offset = 0x148; break;
    case kDamageResist: offset = 0xA8; break;
    case kPoisonResist: offset = 0x188; break;
    case kFireResist: offset = 0x2E0; break;
    case kElectricResist: offset = 0x2E8; break;
    case kFrostResist: offset = 0x2F0; break;
    case kPerceptionCondition: offset = 0x178; break;
    case kEnduranceCondition: offset = 0xC8; break;
    case kLeftAttackCondition: offset = 0x118; break;
    case kRightAttackCondition: offset = 0x1F8; break;
    case kLeftMobilityCondition: offset = 0x128; break;
    case kRightMobilityCondition: offset = 0x208; break;
    case kBrainCondition: offset = 0x78; break;
    case kParalysis: offset = 0x320; break;
    case kInvisibility: offset = 0x108; break;
    case kNightEye: offset = 0x168; break;
    case kWaterBreathing: offset = 0x280; break;
    case kWaterWalking: offset = 0x288; break;
    case kIgnoreCrippledLimbs: offset = 0xF8; break;
    case kWardPower: offset = 0x270; break;
    case kRightItemCharge: offset = 0x200; break;
    case kArmorPerks: offset = 0x38; break;
    case kShieldPerks: offset = 0x218; break;
    case kBowSpeedBonus: offset = 0x68; break;
    case kLeftItemCharge: offset = 0x120; break;
    case kAbsorbChance: offset = 0x0; break;
    case kBlindness: offset = 0x50; break;
    case kWeaponSpeedMult: offset = 0x290; break;
    case kShoutRecoveryMult: offset = 0x220; break;
    case kBowStaggerBonus: offset = 0x70; break;
    case kTelekinesis: offset = 0x258; break;
    case kMovementNoiseMult: offset = 0x160; break;
    case kWaitingForPlayer: offset = 0x278; break;
    case kLeftWeaponSpeedMult: offset = 0x130; break;
    case kCombatHealthRegenMult: offset = 0x90; break;
    case kAttackDamageMult: offset = 0x48; break;
    case kHealRateMult: offset = 0xE0; break;
    case kReflectDamage: offset = 0x1E8; break;
    case kStrength: offset = 0x248; break;
    case kPerception: offset = 0x170; break;
    case kEndurance: offset = 0xC0; break;
    case kCharisma: offset = 0x88; break;
    case kIntelligence: offset = 0x100; break;
    case kAgility: offset = 0x18; break;
    case kLuck: offset = 0x140; break;
    case kActionPoints: offset = 0x8; break;
    case kRads: offset = 0x1D0; break;
    case kRadHealthMax: offset = 0x1C8; break;
    case kFatigue: offset = 0x1E0; break;
    case kFatigueAPMax: offset = 0x1D8; break;
    case kPowerArmorBattery: offset = 0x190; break;
    default: return nullptr;
    }

    using TGetActorValues = void*();
    static VersionDbPtr<TGetActorValues> getActorValues(2189587);
    auto* pValues = static_cast<uint8_t*>(getActorValues.Get()());
    return pValues ? *reinterpret_cast<ActorValueInfo**>(pValues + offset) : nullptr;
}
