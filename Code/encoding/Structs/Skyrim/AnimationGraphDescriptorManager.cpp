#include <Structs/AnimationGraphDescriptorManager.h>
#include <mutex>

#include <Structs/Skyrim/AnimationGraphDescriptor_Master_Behavior.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_BHR_Master.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_WolfRootBehavior.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_Chaurus.h>
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
#include <Structs/Skyrim/AnimationGraphDescriptor_HagravenMasterBehavior.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_ScribRootBehavior.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_BearRootBehavior.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_SabreCatRootBehavior.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_SkeeverRootBehavior.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_HorkerRootBehavior.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_WerewolfBehavior.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_MammothRootBehavior.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_DeerRootBehavior.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_VampireBruteRootBehavior.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_VampireLordBehavior.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_Chicken.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_DwarvenSpider.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_FrostbiteSpider.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_Spriggan.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_Mudcrab.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_Rabbit.h>

AnimationGraphDescriptorManager::AnimationGraphDescriptorManager() noexcept
{
    static std::once_flag s_initOnce;
    std::call_once(s_initOnce, [this]() 
    {
        AnimationGraphDescriptor_Master_Behavior initAnimationGraphDescriptor_Master_Behavior(*this);
        AnimationGraphDescriptor_BHR_Master initAnimationGraphDescriptor_BHR_Master(*this);
        AnimationGraphDescriptor_WolfRootBehavior initAnimationGraphDescriptor_WolfRootBehavior(*this);
        AnimationGraphDescriptor_Chaurus initAnimationGraphDescriptor_Chaurus(*this);
        AnimationGraphDescriptor_TrollBehavior initAnimationGraphDescriptor_TrollBehavior(*this);
        AnimationGraphDescriptor_DraugrBehavior initAnimationGraphDescriptor_DraugrBehavior(*this);
        AnimationGraphDescriptor_GiantRootBehavior initAnimationGraphDescriptor_GiantRootBehavior(*this);
        AnimationGraphDescriptor_DogRootBehavior initAnimationGraphDescriptor_DogRootBehavior(*this);
        AnimationGraphDescriptor_CowRootBehavior initAnimationGraphDescriptor_CowRootBehavior(*this);
        AnimationGraphDescriptor_GoatRootBehavior initAnimationGraphDescriptor_GoatRootBehavior(*this);
        AnimationGraphDescriptor_HorseRootBehavior initAnimationGraphDescriptor_HorseRootBehavior(*this);
        AnimationGraphDescriptor_AtronachFrostRootBehavior initAnimationGraphDescriptor_AtronachFrostRootBehavior(*this);
        AnimationGraphDescriptor_AtronachStormBehavior initAnimationGraphDescriptor_AtronachStormBehavior(*this);
        AnimationGraphDescriptor_SteamBehavior initAnimationGraphDescriptor_SteamBehavior(*this);
        AnimationGraphDescriptor_Falmer_Master_Behavior initAnimationGraphDescriptor_Falmer_Master_Behavior(*this);
        AnimationGraphDescriptor_HagravenMasterBehavior initAnimationGraphDescriptor_HagravenMasterBehavior(*this);
        AnimationGraphDescriptor_ScribRootBehavior initAnimationGraphDescriptor_ScribRootBehavior(*this);
        AnimationGraphDescriptor_BearRootBehavior initAnimationGraphDescriptor_BearRootBehavior(*this);
        AnimationGraphDescriptor_SabreCatRootBehavior initAnimationGraphDescriptor_SabreCatRootBehavior(*this);
        AnimationGraphDescriptor_SkeeverRootBehavior initAnimationGraphDescriptor_SkeeverRootBehavior(*this);
        AnimationGraphDescriptor_HorkerRootBehavior initAnimationGraphDescriptor_HorkerRootBehavior(*this);
        AnimationGraphDescriptor_WerewolfBehavior initAnimationGraphDescriptor_WerewolfBehavior(*this);
        AnimationGraphDescriptor_MammothRootBehavior initAnimationGraphDescriptor_MammothRootBehavior(*this);
        AnimationGraphDescriptor_DeerRootBehavior initAnimationGraphDescriptor_DeerRootBehavior(*this);
        AnimationGraphDescriptor_VampireBruteRootBehavior initAnimationGraphDescriptor_VampireBruteRootBehavior(*this);
        AnimationGraphDescriptor_VampireLordBehavior initAnimationGraphDescriptor_VampireLordBehavior(*this);
        AnimationGraphDescriptor_Chicken initAnimationGraphDescriptor_Chicken(*this);
        AnimationGraphDescriptor_DwarvenSpider initAnimationGraphDescriptor_DwarvenSpider(*this);
        AnimationGraphDescriptor_FrostbiteSpider initAnimationGraphDescriptor_FrostbiteSpider(*this);
        AnimationGraphDescriptor_Spriggan initAnimationGraphDescriptor_Spriggan(*this);
        AnimationGraphDescriptor_Mudcrab initAnimationGraphDescriptor_Mudcrab(*this);
        AnimationGraphDescriptor_Rabbit initAnimationGraphDescriptor_Rabbit(*this);
    });
}
