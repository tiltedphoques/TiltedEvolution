#include <Structs/AnimationGraphDescriptorManager.h>
#include <mutex>

#include <Structs/Skyrim/AnimationGraphDescriptor_Master_Behavior.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_BHR_Master.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_WolfRootBehavior.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_Root.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_TrollBehavior.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_DraugrBehavior.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_GiantRootBehavior.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_DogRootBehavior.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_CowRootBehavior.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_GoatRootBehavior.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_HorseRootBehavior.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_AtronachFrostRootBehavior.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_AtronachStormBehavior.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_SteamBehavior.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_Falmer_Master_Behavior.h>

AnimationGraphDescriptorManager::AnimationGraphDescriptorManager() noexcept
{
    static std::once_flag s_initOnce;
    std::call_once(s_initOnce, [this]() 
    {
        AnimationGraphDescriptor_Master_Behavior initAnimationGraphDescriptor_Master_Behavior(*this);
        AnimationGraphDescriptor_BHR_Master initAnimationGraphDescriptor_BHR_Master(*this);
        AnimationGraphDescriptor_WolfRootBehavior initAnimationGraphDescriptor_WolfRootBehavior(*this);
        //AnimationGraphDescriptor_Root initAnimationGraphDescriptor_Root(*this);
        AnimationGraphDescriptor_TrollBehavior initAnimationGraphDescriptor_TrollBehavior(*this);
        AnimationGraphDescriptor_DraugrBehavior initAnimationGraphDescriptor_DraugrBehavior(*this);
        AnimationGraphDescriptor_GiantRootBehavior initAnimationGraphDescriptor_GiantRootBehavior(*this);
        AnimationGraphDescriptor_DogRootBehavior initAnimationGraphDescriptor_DogRootBehavior(*this);
        AnimationGraphDescriptor_CowRootBehavior initAnimationGraphDescriptor_CowRootBehavior(*this);
        AnimationGraphDescriptor_HorseRootBehavior initAnimationGraphDescriptor_HorseRootBehavior(*this);
        AnimationGraphDescriptor_AtronachFrostRootBehavior initAnimationGraphDescriptor_AtronachFrostRootBehavior(*this);
        AnimationGraphDescriptor_AtronachStormBehavior initAnimationGraphDescriptor_AtronachStormBehavior(*this);
        AnimationGraphDescriptor_SteamBehavior initAnimationGraphDescriptor_SteamBehavior(*this);
        AnimationGraphDescriptor_Falmer_Master_Behavior initAnimationGraphDescriptor_Falmer_Master_Behavior(*this);
    });
}
