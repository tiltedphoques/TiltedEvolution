
//
// Core support for modified behavior (as in Nemesis or Pandora behavior modification or extension).
//
// Credit to Edho08 for the original implementation. This version advances to include support
// for unlimited behavior variables, for seeding modded behaviors with the original BehaviorVars
// (decoupling STR devs decisions from what modders need to know) and support for beast forms
// among many more changes.
//
// One of the greatest features ensuring the longevity of Bethesda games is their support
// for community modification, and many of the most popular mods also change the behavior
// of creatures. STR and FTR don't support mods as a matter of policy. And supporting
// behavior mods is particularly difficult. This mod provides the option to use modified
// behaviors in Skyrim|Fallout Together Reborn, but has yet to be endorsed by the TiltedPhoques
// team. Use at own risk.
//
// The game determines a set of behavior variables that must be synced and locks that down.
// Changing the list is difficult, because behavior mods won't just add to the list, they
// will change the order of the list also changing the shorthand numeric codes for behavior
// vars to sync.
//
// So, supporting behavior mods means:
//     1) We must be able the translate the built-in behavior var codes back to their string values.
//     2) We must then retranslate the string values back to their (new) numeric values.
//     3) We must add in the additional behavior variables requested by modders
//     4) Finally, that will push us over some hard-coded limits, in particular
//        a limit of 64 boolean vars that can be synced. Remove the limit.
//

#include <immersive_launcher/launcher.h>
#include <BSAnimationGraphManager.h>
#include <Games/ActorExtension.h>
#include "BehaviorVar.h"
#include "BehaviorVarsMap.h"

#include <Camera/TESCamera.h> // Camera 1st person is only in Skyrim?
#include <Camera/PlayerCamera.h>

#include <mutex>
std::mutex mutex_lock;

// How long we failList a behavior hash that can't be translated.
// See explanation in BehaviorVar::Patch
using namespace std::literals;
const std::chrono::steady_clock::duration FAILLIST_DURATION(10min);

BehaviorVar* BehaviorVar::single = nullptr;
BehaviorVar* BehaviorVar::Get()
{
    if (!BehaviorVar::single)
        BehaviorVar::single = new BehaviorVar();
    return BehaviorVar::single;
}

// A simple function can be declared, caller file doesn't have to include our
// class/struct header.
const AnimationGraphDescriptor* BehaviorVarPatch(BSAnimationGraphManager* apManager, Actor* apActor)
{
    return BehaviorVar::Get()->Patch(apManager, apActor);
}

// Utility function to convert a string to lowercase
// This is useful as at least one vanilla variable, "Speed", is sometimes lowercase
// We'd like to be able to handle other vars for which this may be the case.
namespace
{
TiltedPhoques::String ToLowerCase(const TiltedPhoques::String& acStr)
{
    TiltedPhoques::String lowerCaseStr(acStr);
    std::transform(acStr.begin(), acStr.end(), lowerCaseStr.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    return lowerCaseStr;
}

// Converts the keys of a map (which are TiltedPhoques::String) to lowercase.
// This is our "Plan C" if a vanilla var isn't found to make sure it isn't just a case-sensitivity issue.
void LowerCaseKeys(const TiltedPhoques::Map<TiltedPhoques::String, uint32_t>& acMap, TiltedPhoques::Map<TiltedPhoques::String, uint32_t>& aLowerCaseMap)
{
    for (const auto& item : acMap)
        aLowerCaseMap.insert({ToLowerCase(item.first), item.second});
}

// Process a set of variables, adding them to the aVariableSet
void ProcessVariableSet(
    TiltedPhoques::Map<TiltedPhoques::String, uint32_t>& acReverseMap,
    TiltedPhoques::Set<uint32_t>& aVariableSet,
    const TiltedPhoques::Vector<TiltedPhoques::String>& acVariables,
    spdlog::level::level_enum aLogLevel)
{
    // Not filled until needed, which should be never.
    TiltedPhoques::Map<TiltedPhoques::String, uint32_t> lowerCaseMap;

    for (const auto& item : acVariables)
    {
        auto foundForm = item;
        auto found = acReverseMap.find(item);
        if (found == acReverseMap.end())
        {
            // Check if first letter incorrectly lowercase.
            foundForm[0] = std::toupper(item[0]);
            found = acReverseMap.find(foundForm);
        }

        if (found == acReverseMap.end())
        {
            // Check lower case.
            foundForm = ToLowerCase(item);
            found = acReverseMap.find(foundForm);
        }

        if (found == acReverseMap.end())
        {
            // Check case-independent
            if (lowerCaseMap.empty())
                LowerCaseKeys(acReverseMap, lowerCaseMap);
            found = lowerCaseMap.find(foundForm);
        }

        if (found == acReverseMap.end())
            spdlog::warn(__FUNCTION__ ": unable to find variable {} in any of the common misspellings", item);
        else
        {
            aVariableSet.insert(found->second);
            if (item != found->first)
                spdlog::warn(__FUNCTION__ ": misspelled variable {} corrected to {}", item, found->first);
        }
    }
}

} // End anonymous namespace

//
// Translate modded behavior numeric values.
// When a behavior is modified (at least by Nemesis) it can rearrange the order of BehaviorVars.
// Since their corresponding numeric value is based on the order, we have to translate the
// BehaviorVars chosen (numerically) by the STR devs to their new numeric values.
// We do this with a hack to translate old numeric value back to a string, then we
// can forward-translate the string to its new numeric value.
//
// The machine-generated table hack to do this can be removed with STR-devs'
// permission to also embed the string information in the Code\encoding\structs files.
//
void BehaviorVar::SeedAnimationVariables(
    const uint64_t acHash,
    const AnimationGraphDescriptor* acpDescriptor,
    TiltedPhoques::Map<TiltedPhoques::String, uint32_t>& acReverseMap,
    TiltedPhoques::Set<uint32_t>& aBoolVars,
    TiltedPhoques::Set<uint32_t>& aFloatVars,
    TiltedPhoques::Set<uint32_t>& aIntVars)
{
    auto& origVars = BehaviorVarsMap::getInstance();

    // Prepare lists of variables to process
    TiltedPhoques::Vector<TiltedPhoques::String> boolVarNames;
    TiltedPhoques::Vector<TiltedPhoques::String> floatVarNames;
    TiltedPhoques::Vector<TiltedPhoques::String> intVarNames;

    // Populate lists from the original descriptor
    TiltedPhoques::String strValue;
    for (auto& item : acpDescriptor->BooleanLookUpTable)
        if ((strValue = origVars.find(acHash, item)).empty())
            spdlog::error(__FUNCTION__ ": unable to find string for original BooleanVar {}", item);
        else
            boolVarNames.push_back(strValue);

    for (auto& item : acpDescriptor->FloatLookupTable)
        if ((strValue = origVars.find(acHash, item)).empty())
            spdlog::error(__FUNCTION__ ": unable to find string for original FloatVar {}", item);
        else
            floatVarNames.push_back(strValue);

    for (auto& item : acpDescriptor->IntegerLookupTable)
        if ((strValue = origVars.find(acHash, item)).empty())
            spdlog::error(__FUNCTION__ ": unable to find string for original IntVar {}", item);
        else
            intVarNames.push_back(strValue);

    // Process each set of variables
    ProcessVariableSet(acReverseMap, aBoolVars, boolVarNames, spdlog::level::level_enum::err);
    ProcessVariableSet(acReverseMap, aFloatVars, floatVarNames, spdlog::level::level_enum::err);
    ProcessVariableSet(acReverseMap, aIntVars, intVarNames, spdlog::level::level_enum::err);
}

// Syntax for a signature is [!]sig1[,[!]sig2]... Must be at least one. Each signature var possibly negated
// signature var possibly negated (meaning it MUST NOT be in the BehaviorVars of the actor). Separated by commas.
// Requires that whitespace has already been deleted (which it was, when directories were loaded).
// Tokenize on ','
//
const TiltedPhoques::Vector<TiltedPhoques::String> BehaviorVar::TokenizeBehaviorSig(const TiltedPhoques::String& acSignature) const
{
    const static TiltedPhoques::String notVal{"!"};
    TiltedPhoques::Vector<TiltedPhoques::String> retVal;
    size_t commaPos;
    size_t offset = 0;

    do
    {
        if (acSignature[offset] == '!')
        {
            offset++;
            retVal.push_back(notVal);
        }

        commaPos = acSignature.find(',', offset);
        retVal.push_back(acSignature.substr(offset, commaPos));
        offset = commaPos + 1;

    } while (commaPos != std::string::npos);

    return retVal;
}
const AnimationGraphDescriptor* BehaviorVar::ConstructModdedDescriptor(
    const uint64_t acNewHash,
    const Replacer& acReplacer,
    TiltedPhoques::Map<TiltedPhoques::String, uint32_t>& acReverseMap)
{
    // Build the set of BehaviorVar strings as sets (not vectors) to eliminate dups
    TiltedPhoques::Set<uint32_t> boolVar;
    TiltedPhoques::Set<uint32_t> floatVar;
    TiltedPhoques::Set<uint32_t> intVar;

    // If we can find the original behavior that is being modded,
    // get the descriptor and seed the behavior vars with it.
    // That way mod developers only need to know what vars their
    // mod adds, they don't have to know what the STR devs picked
    const AnimationGraphDescriptor* pTmpGraph = nullptr;
    if (acReplacer.origHash && (pTmpGraph = AnimationGraphDescriptorManager::Get().GetDescriptor(acReplacer.origHash)))
    {
        SeedAnimationVariables(acReplacer.origHash, pTmpGraph, acReverseMap, boolVar, floatVar, intVar);
        spdlog::info(
            __FUNCTION__ ": Original game descriptor with hash {} has {} boolean, {} float, {} integer behavior vars",
            acReplacer.origHash,
            boolVar.size(),
            floatVar.size(),
            intVar.size());
    }

    // Check requested behavior vars for those that ARE legit
    // behavior vars for this Actor AND are not yet synced
    int foundCount = 0;
    for (auto& item : acReplacer.syncBooleanVar)
    {
        bool found = acReverseMap.find(item) != acReverseMap.end();
        if (found && boolVar.find(acReverseMap[item]) == boolVar.end())
        {
            if (foundCount++ == 0)
                spdlog::info("Boolean variables added to sync:");
            boolVar.insert(acReverseMap[item]);
            spdlog::info("    {}", item);
        }
    }
    if (foundCount)
        spdlog::info("Now have {} boolVar descriptors after searching {} BehavivorVar strings", boolVar.size(), acReplacer.syncBooleanVar.size());

    foundCount = 0;
    for (auto& item : acReplacer.syncFloatVar)
    {
        bool found = acReverseMap.find(item) != acReverseMap.end();
        if (found && floatVar.find(acReverseMap[item]) == floatVar.end())
        {
            if (foundCount++ == 0)
                spdlog::info("Float variables added to sync:");
            floatVar.insert(acReverseMap[item]);
            spdlog::info("    {}", item);
        }
    }
    if (foundCount)
        spdlog::info("Now have {} floatVar descriptors after searching {} BehavivorVar strings", floatVar.size(), acReplacer.syncFloatVar.size());

    foundCount = 0;
    for (auto& item : acReplacer.syncIntegerVar)
    {
        bool found = acReverseMap.find(item) != acReverseMap.end();
        if (found && intVar.find(acReverseMap[item]) == intVar.end())
        {
            if (foundCount++ == 0)
                spdlog::info("Int variables added to sync:");
            intVar.insert(acReverseMap[item]);
            spdlog::info("    {}", item);
        }
    }
    if (foundCount)
        spdlog::info(__FUNCTION__ ": now have {} intVar descriptors after searching {} BehavivorVar strings", intVar.size(), acReplacer.syncIntegerVar.size());

    // We need the sets sorted, and TiltedPhoques::Set is not. Copying to an std::set
    // isn't the most efficient approach, but it is simple and doesn't happen often enough
    // to worry about. And doing it here isolates/minimizes use of std::set.
    std::set<uint32_t> boolVarSorted(boolVar.begin(), boolVar.end());
    std::set<uint32_t> floatVarSorted(floatVar.begin(), floatVar.end());
    std::set<uint32_t> intVarSorted(intVar.begin(), intVar.end());

    // Reshape the (sorted, unique) sets to vectors
    TiltedPhoques::Vector<uint32_t> boolVector(boolVarSorted.begin(), boolVarSorted.end());
    TiltedPhoques::Vector<uint32_t> floatVector(floatVarSorted.begin(), floatVarSorted.end());
    TiltedPhoques::Vector<uint32_t> intVector(intVarSorted.begin(), intVarSorted.end());

    // Construct a new descriptor
    auto panimGraphDescriptor = new AnimationGraphDescriptor();

    // This is a bit grubby, but it IS a struct with public members.
    // Maybe the devs will help us out by agreeing to an additional constructor.
    panimGraphDescriptor->BooleanLookUpTable = boolVector;
    panimGraphDescriptor->FloatLookupTable = floatVector;
    panimGraphDescriptor->IntegerLookupTable = intVector;

    // Add the new graph to the known behavior graphs
    // TODO: doesn't handle case of the acNewHash already existing.
    // That has to be addressed before we can support just adding
    // more pre-existing vars to sync. For now, only get here
    // when mods have changed the hash.
    new AnimationGraphDescriptorManager::Builder(AnimationGraphDescriptorManager::Get(), acNewHash, *panimGraphDescriptor);
    return AnimationGraphDescriptorManager::Get().GetDescriptor(acNewHash);
}

const AnimationGraphDescriptor* BehaviorVar::Patch(BSAnimationGraphManager* apManager, Actor* apActor)
{
    // Serialize, Actors are multi-threaded. Might not be strictly necessary between being on
    // the main game loop and under a BSRecursiveLock, but if so it won't hurt anything.
    std::lock_guard guard(mutex_lock);

    // Check if the animation descriptor has already been built.
    uint32_t hexFormID = apActor->formID;
    auto pExtendedActor = apActor->GetExtension();
    auto hash = pExtendedActor->GraphDescriptorHash;
    const AnimationGraphDescriptor* pGraph = AnimationGraphDescriptorManager::Get().GetDescriptor(hash);

    // If it is the player-character AND they are in 1st person, we don't have the data to mod them;
    // Use the base game animation graphs until a Master Behavior actor enters the room. Could be an NPC,
    // but will always happen no later than when the 2nd person joins the server and room.
    // Remote players are ALWAYS in 3rd person by definition
    if (hexFormID == 0x14 && PlayerCamera::Get()->IsFirstPerson())
    {
        hash = AnimationGraphDescriptor_Master_Behavior::m_key;
        pGraph = AnimationGraphDescriptorManager::Get().GetDescriptor(hash);
    }

    // If we found the descriptor we're done.
    if (pGraph)
        return pGraph;

    // If BehaviorVar replacement fails, the hash is failListed for a period of time.
    // This handles a number of special cases for us. Many behaviors are not synced; the
    // game just ignores them and let's the multiple clients each handle it.
    // Examples are the "Root" behavior (not synced as a behavior, but handled),
    // or any number of lower-priority actors such as Wisps (location is always synced,
    // but otherwise behavior sync not needed).
    //
    // FailListed for minutes to avoid performance hit of constantly checking for
    // modded behavior. Only failListed for minutes to occasionally get log messages
    // so we can think about fixing it. Or the case of dynamic behavior and behavior
    // signature changes that might work later (yes, mods like that exist)
    if (FailListed(hash))
        return nullptr;

    // Up to here the routine is pretty cheap, and WILL be called a bunch of times
    // if the only humanoid Actor is the Dragonborn/player in 1st person.
    // With that case filtered out, keep a counter to see if we need to defend against other
    // cases (like a modded behavior where we CAN't find the signature var).
    if (invocations++ == 1000)
        spdlog::warn(__FUNCTION__ ": warning, more than 1000 invocations, investigate why");

    spdlog::info(__FUNCTION__ ": actor with formID {:x} with hash of {} has modded (or not synced) behavior", hexFormID, hash);

    // Get all animation variables for this actor, then create a acReverseMap to go from strings to animation enum.
    auto pDumpVar = apManager->DumpAnimationVariables(false);
    TiltedPhoques::Map<TiltedPhoques::String, uint32_t> reverseMap;
    spdlog::info("Known behavior variables for formID {:x}:", hexFormID);
    for (auto& item : pDumpVar)
    {
        spdlog::info("    {}:{}", item.first, item.second);
        reverseMap.insert({item.second, item.first});
    }

    // See if these animation variables include a signature for one of the replacers.
    // Since some of the original behaviors don't have a single variable name that is completely
    // unique, we allow a syntax of "!?signaturevar\(,!?signaturevar\)*". That is, an initial
    // (possibly negated) signaturevar, followed by zero or more comma separated additional
    // signaturevars (possibly negated).
    //
    // That's enough info to create a unique signature in the base game descriptors. At least,
    // it was at the time of writing.
    //
    // O(N) loop, but this typically only gets executed a few times (once for each modded
    // creature TYPE, not each modded creature).
    //
    std::vector<size_t> matchedReplacers;

    for (size_t i = 0; i < behaviorPool.size(); i++)
    {
        auto tokens = TokenizeBehaviorSig(behaviorPool[i].signatureVar);
        auto found = tokens.size() > 0;

        for (auto titer = tokens.begin(); found && titer < tokens.end(); titer++)
        {
            if (*titer == "!")
                found = reverseMap.find(*++titer) == reverseMap.end();
            else
                found = reverseMap.find(*titer) != reverseMap.end();
        }
        if (found)
            matchedReplacers.push_back(i);
    }

    switch (matchedReplacers.size())
    {
    case 0:
        spdlog::warn(
            __FUNCTION__ ": no original behavior found for behavior hash {:x} (found on formID {:x}), adding to fail list",
            hash,
            hexFormID);
        FailList(hash);
        return nullptr;

    case 1: break;

    default:
        spdlog::critical(__FUNCTION__ ": multiple behavior replacers have the same signature, this must be corrected:");
        for (auto& item : matchedReplacers)
            spdlog::critical("   {}", behaviorPool[item].creatureName);
        spdlog::warn("Multiple behavior replacers have the same signature, choosing the first one.");
        break;
    }

    auto& foundRep = behaviorPool[matchedReplacers[0]];
    spdlog::info(
        __FUNCTION__ ": found match, behavior hash {:x} (found on formID {:x}) has original behavior {} signature {}",
        hash,
        hexFormID,
        foundRep.creatureName,
        foundRep.signatureVar);

    // Save the new hash for the actor. Save copies of the new hash when humanoids (Master_Behavior)
    // or dragons (BHR_Master) are modified. The humanoid hash is used to return from beast mode
    // (werewolf or vampire), and the dragon hash is used to implement an IsDragon() primitive.
    pExtendedActor->GraphDescriptorHash = hash;
    switch (foundRep.origHash)
    {
    case AnimationGraphDescriptor_Master_Behavior::m_key:
        m_humanoidGraphDescriptorHash = hash;
        spdlog::info(__FUNCTION__ ": captured modified Master_Behavior hash {:X}", hash);
        break;

    case AnimationGraphDescriptor_BHR_Master::m_key:
        m_dragonGraphDescriptorHash = hash;
        spdlog::info(__FUNCTION__ ": captured modified BHR_Master (dragon) hash {:X}", hash);
        break;
    }

    return ConstructModdedDescriptor(hash, foundRep, reverseMap);
}

// Check if the behavior hash is on the failed liist
bool BehaviorVar::FailListed(uint64_t hash)
{
    auto iter = failedBehaviors.find(hash);
    return iter != failedBehaviors.end() && std::chrono::steady_clock::now() < iter->second;
}

// Place the behavior hash on the failed list
void BehaviorVar::FailList(uint64_t hash)
{
    failedBehaviors.insert_or_assign(hash, std::chrono::steady_clock::now() + FAILLIST_DURATION);
}

// Find all the subdirectories of behavior variables
TiltedPhoques::Vector<std::filesystem::path> BehaviorVar::LoadDirs(const std::filesystem::path& acPath)
{
    TiltedPhoques::Vector<std::filesystem::path> result;
    for (auto& p : std::filesystem::directory_iterator(acPath))
        if (p.is_directory())
            result.push_back(p.path().string());
    return result;
}

BehaviorVar::Replacer* BehaviorVar::LoadReplacerFromDir(const std::filesystem::path& aDir)
{
    // Enumerate all files in the directory and push bools, ints and floats
    // to their respective vectors

    // The hashFile *__hash.txt file should contain the hashed signature of the ORIGINAL GAME
    // actor. That enables us to look up the synced animation variables selected by
    // the STR devs and include them in the updated behavior/animation sync. Mod
    // devs don't have to know anything but what they've added, and this support for
    // Nemesis will work with more versions of the game.
    //
    // The signatureFile *__sig.txt should contain the name of a uniquely named variable for the
    // modded actor, that we can use to connect the actor to their modded behavior variables.
    // It's not actually sync'ed, just there so we can find the match. For example, currently
    // bSTRMaster is used for humanoid/Player actors, and bSTRDragon for dragon behavior.
    TiltedPhoques::String hashFile;
    TiltedPhoques::String signatureFile;
    TiltedPhoques::Vector<TiltedPhoques::String> floatVarsFile;
    TiltedPhoques::Vector<TiltedPhoques::String> intVarsFile;
    TiltedPhoques::Vector<TiltedPhoques::String> boolVarsFile;

    for (auto& p : std::filesystem::directory_iterator(aDir))
    {
        TiltedPhoques::String path(p.path().string());
        TiltedPhoques::String base_filename = path.substr(path.find_last_of("/\\") + 1);

        spdlog::debug("base_path: {}", base_filename);

        if (base_filename.find("__sig.txt") != std::string::npos)
        {
            signatureFile = path;

            spdlog::debug("signature variable file: {}", signatureFile);
        }
        else if (base_filename.find("__hash.txt") != std::string::npos)
        {
            hashFile = path;

            spdlog::debug("hash file: {}", hashFile);
        }
        else if (base_filename.find("__float.txt") != std::string::npos)
        {
            floatVarsFile.push_back(path);

            spdlog::debug("float file: {}", path);
        }
        else if (base_filename.find("__int.txt") != std::string::npos)
        {
            intVarsFile.push_back(path);

            spdlog::debug("int file: {}", path);
        }
        else if (base_filename.find("__bool.txt") != std::string::npos)
        {
            boolVarsFile.push_back(path);

            spdlog::debug("bool file: {}", path);
        }
    }

    // Check that there is a signature file (an identifying variable)
    // When an Actor's behavior is modified, it's animation signature doesn't match
    // any of the base STR built-in descriptors. We try to match up on a distinguishing
    // animation variable added by the animation. But without such a distinquishing
    if (signatureFile == "")
        return nullptr;

    // Prepare reading files
    TiltedPhoques::String sigVar;
    TiltedPhoques::String creatureName(aDir.filename().string());
    TiltedPhoques::Vector<TiltedPhoques::String> floatVar;
    TiltedPhoques::Vector<TiltedPhoques::String> intVar;
    TiltedPhoques::Vector<TiltedPhoques::String> boolVar;
    TiltedPhoques::String tempString; // Temp string

    // If signature file is unreadable, this directory is useless.
    std::ifstream fileSig(signatureFile.c_str(), std::ifstream::in);
    if (!fileSig.is_open())
        return nullptr;

    getline(fileSig, sigVar); // grab the signature variable
    fileSig.close();
    erase_if(sigVar, isspace); // removes any inadvertent whitespace
    if (sigVar.size() == 0)
        return nullptr;
    spdlog::info(__FUNCTION__ ": found {} with signature variable {}", creatureName, sigVar);

    // Check to see if there is a hash file.
    // This is recommended, and shouild contain the ORIGINAL hash,
    // before modding, of the behavior. It's not reasonable for a mod
    // developer to have to "know" the complete set of variables the STR
    // devs have selected, so we'll want to get them from the original behaviors
    // via the original hash. If provided, we'll start with those vars and the
    // mod developer only lists the added ones they need.
    //
    // The orginal version of this patch by Spvvd also included the "new" hash,
    // but that seems to be another thing a mod dev has no easy way to get their hands
    // on. So stripped off the need to provide the new hash, we'll just calculate it.
    //
    uint64_t orgHash = 0;
    if (hashFile.size())
    {
        std::ifstream file(hashFile.c_str(), std::ifstream::in);
        if (file.is_open())
        {
            getline(file, tempString);
            file.close();
            erase_if(tempString, isspace); // removes any inadvertant whitespace
            orgHash = std::strtoull(tempString.c_str(), nullptr, 10);
            spdlog::info("Replacer specifies original hash {} for {}", orgHash, creatureName);
        }
    }

    // Build lists of variables to sync
    // Read float behavior variables
    spdlog::debug("reading float var");
    for (auto& item : floatVarsFile)
    {
        std::ifstream file(item.c_str(), std::ifstream::in);
        if (file.is_open())
        {
            while (std::getline(file, tempString))
            {
                floatVar.push_back(tempString);

                spdlog::debug("    " + tempString);
            }
            file.close();
        }
    }

    // Read integer behavior variables
    spdlog::debug("reading int vars");
    for (auto& item : intVarsFile)
    {
        std::ifstream file(item.c_str(), std::ifstream::in);
        if (file.is_open())
        {
            while (std::getline(file, tempString))
            {
                intVar.push_back(tempString);

                spdlog::debug("    " + tempString);
            }
            file.close();
        }
    }

    // Read boolean behavior variables
    spdlog::debug("reading bool vars");
    for (auto& item : boolVarsFile)
    {
        std::ifstream file(item.c_str(), std::ifstream::in);
        if (file.is_open())
        {
            while (std::getline(file, tempString))
            {
                boolVar.push_back(tempString);

                spdlog::debug("     " + tempString);
            }
            file.close();
        }
    }

    // Create the replacer
    Replacer* result = new Replacer();

    result->origHash = orgHash;
    result->signatureVar = sigVar;
    result->creatureName = creatureName;
    result->syncBooleanVar = boolVar;
    result->syncFloatVar = floatVar;
    result->syncIntegerVar = intVar;

    return result;
}

// Find any behaviors which match the signature.
// There should be exactly one.
//
TiltedPhoques::Vector<uint64_t> BehaviorVar::SignatureMatches(const uint64_t acHash, const TiltedPhoques::String& acSignature) const
{
    auto& bvMap = BehaviorVarsMap::getInstance();
    TiltedPhoques::Vector<uint64_t> hashes;

    bvMap.Hashes(hashes);
    auto tokens = TokenizeBehaviorSig(acSignature);
    for (size_t i = 0; i < hashes.size(); i++)
    {
        if (hashes[i] == acHash)
            continue;

        auto found = true;
        for (auto titer = tokens.begin(); found && titer < tokens.end(); titer++)
        {
            if (*titer == "!")
                found = bvMap.find(hashes[i], *++titer) == UINT32_MAX;
            else
                found = bvMap.find(hashes[i], *titer) != UINT32_MAX;
        }
        if (!found)
            hashes.erase(hashes.begin() + i--);
    }

    return hashes;
}

void BehaviorVar::Init()
{
    // Initialize original (base STR) behaviors so we can search them.
    BehaviorOrig::BehaviorOrigInit();

    // Check if the behaviors folder exists
    std::filesystem::path pBehaviorsPath;
    pBehaviorsPath = launcher::GetLaunchContext()->gamePath / L"Data" / L"SkyrimTogetherRebornBehaviors";

    if (!std::filesystem::is_directory(pBehaviorsPath))
        return;

    auto behaviorDirs = LoadDirs(pBehaviorsPath);

    for (auto& dir : behaviorDirs)
    {
        Replacer* sig = LoadReplacerFromDir(dir);
        if (sig)
        {
            behaviorPool.push_back(*sig);
        }
    }

    // Ambiguousness check.
    for (auto& signature : behaviorPool)
    {
        static size_t firsttime = 0;

        auto matches = SignatureMatches(signature.origHash, signature.signatureVar);
        switch (matches.size())
        {
        case 0: spdlog::critical(__FUNCTION__ ": failure to find self in behaviorPool, {}", signature.creatureName);
        case 1: break;

        default:
            if (firsttime++ == 0)
                spdlog::warn(__FUNCTION__ ": some creatures have ambiguous signatures. This is expected for now,\n"
                                          "    but a modder must create a unique signature in their mod.");

            spdlog::warn(__FUNCTION__ ": {} signature {} matches:", signature.creatureName, signature.signatureVar);
            for (auto hash : matches)
            {
                auto iter = std::find(behaviorPool.begin(), behaviorPool.end(), hash);
                if (iter < behaviorPool.end())
                    spdlog::warn("    {}", std::find(behaviorPool.begin(), behaviorPool.end(), hash)->creatureName);
                else
                    spdlog::warn("    {}: unable to find creature name for this hash, likely typo in SkyrimTogetherReborn tree", hash);
            }
            break;
        }
    }
}

void BehaviorVar::Debug()
{
    auto descriptors = AnimationGraphDescriptorManager::Get().GetDescriptors();

    // Descriptor hash dump
    spdlog::info("Dumping descriptor hashes:");
    for (const auto& pair : descriptors)
    {
        spdlog::info("Hash: {}", pair.first);
    }

    // Loaded behavior dump
    spdlog::info("Dumping behavior sigs:");
    for (const auto& sig : behaviorPool)
    {
        spdlog::info("origHash: {} => newHash: {}", sig.origHash, sig.newHash);

        // bools
        spdlog::info("bools:");
        for (const auto& var : sig.syncBooleanVar)
        {
            spdlog::info(var);
        }
        // bools
        spdlog::info("floats:");
        for (const auto& var : sig.syncFloatVar)
        {
            spdlog::info(var);
        }
        // bools
        spdlog::info("integers:");
        for (const auto& var : sig.syncIntegerVar)
        {
            spdlog::info(var);
        }
    }
}
