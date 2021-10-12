#include <Structs/AnimationGraphDescriptorManager.h>
#include <mutex>

#include <Structs/Fallout4/AnimationGraphDescriptor_Master_Behavior.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_CaveCricketRoot.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_RootState.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_SuperMutantRootBehavior.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_Alien.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_Angler.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_InsectsSmall.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_Bloatfly.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_Bloodbug.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_Bloodworm.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_Brahmin.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_Deathclaw.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_Deer.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_FogCrawler.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_Ghoul.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_Gorilla.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_MutantHound.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_RabbitChicken.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_Radscorpion.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_Rat.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_Stingwing.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_Wolflike.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_YaoGuai.h>

AnimationGraphDescriptorManager::AnimationGraphDescriptorManager() noexcept
{
    static std::once_flag s_initOnce;
    std::call_once(s_initOnce, [this]() {
        AnimationGraphDescriptor_Master_Behavior initAnimationGraphDescriptor_Master_Behavior(*this);
        AnimationGraphDescriptor_CaveCricketRoot initAnimationGraphDescriptor_CaveCricketRoot(*this);
        AnimationGraphDescriptor_RootState initAnimationGraphDescriptor_RootState(*this);
        AnimationGraphDescriptor_SuperMutantRootBehavior initAnimationGraphDescriptor_SuperMutantRootBehavior(*this);
        AnimationGraphDescriptor_Alien initAnimationGraphDescriptor_Alien(*this);
        AnimationGraphDescriptor_Angler initAnimationGraphDescriptor_Angler(*this);
        AnimationGraphDescriptor_InsectsSmall initAnimationGraphDescriptor_InsectsSmall(*this);
        AnimationGraphDescriptor_Bloatfly initAnimationGraphDescriptor_Bloatfly(*this);
        AnimationGraphDescriptor_Bloodbug initAnimationGraphDescriptor_Bloodbug(*this);
        AnimationGraphDescriptor_Bloodworm initAnimationGraphDescriptor_Bloodworm(*this);
        AnimationGraphDescriptor_Brahmin initAnimationGraphDescriptor_Brahmin(*this);
        AnimationGraphDescriptor_Deathclaw initAnimationGraphDescriptor_Deathclaw(*this);
        AnimationGraphDescriptor_Deer initAnimationGraphDescriptor_Deer(*this);
        AnimationGraphDescriptor_FogCrawler initAnimationGraphDescriptor_FogCrawler(*this);
        AnimationGraphDescriptor_Ghoul initAnimationGraphDescriptor_Ghoul(*this);
        AnimationGraphDescriptor_Gorilla initAnimationGraphDescriptor_Gorilla(*this);
        AnimationGraphDescriptor_MutantHound initAnimationGraphDescriptor_MutantHound(*this);
        AnimationGraphDescriptor_RabbitChicken initAnimationGraphDescriptor_RabbitChicken(*this);
        AnimationGraphDescriptor_Radscorpion initAnimationGraphDescriptor_Radscorpion(*this);
        AnimationGraphDescriptor_Rat initAnimationGraphDescriptor_Rat(*this);
        AnimationGraphDescriptor_Stingwing initAnimationGraphDescriptor_Stingwing(*this);
        AnimationGraphDescriptor_Wolflike initAnimationGraphDescriptor_Wolflike(*this);
        AnimationGraphDescriptor_YaoGuai initAnimationGraphDescriptor_YaoGuai(*this);
    });
}
