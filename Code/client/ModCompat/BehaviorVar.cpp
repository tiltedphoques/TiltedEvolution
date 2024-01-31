
#ifdef MODDED_BEHAVIOR_COMPATIBILITY

#include <BSAnimationGraphManager.h>
#include <Games/ActorExtension.h>
#include <ModCompat/BehaviorVar.h>

#if TP_SKYRIM64
#include <Structs/Skyrim/AnimationGraphDescriptor_Master_Behavior.h>
#include <Camera/TESCamera.h>       // Camera 1st person is only in Skyrim?
#include <Camera/PlayerCamera.h>
#endif

#include <mutex>
std::mutex mutex_lock;

// How long we failList a behavior hash that can't be translated.
// See explanation in BehaviorVar::Patch
using namespace std::literals;
const std::chrono::steady_clock::duration FAILLIST_DURATION(5min);

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

const AnimationGraphDescriptor* BehaviorVar::Patch(BSAnimationGraphManager* apManager, Actor* apActor)
{
    // Serialize, Actors are multi-threaded. Might not be strictly necessary, I think we're on
    // the main game loop, but if so it won't hurt anything.
    std::lock_guard guard(mutex_lock);

    // Check if the animation descriptor has already been built.
    uint32_t hexFormID = apActor->formID;
    auto pExtendedActor = apActor->GetExtension();
    auto hash = pExtendedActor->GraphDescriptorHash;
    const AnimationGraphDescriptor* pGraph = AnimationGraphDescriptorManager::Get().GetDescriptor(hash);

#if TP_SKYRIM64
    // If it is the player-character AND they are in 1st person, we don't have the data to mod them;
    // Used the base game animation graphs until a Master Behavior actor enters the room. Could be an NPC,
    // but will always happen no later than when the 2nd person joins the server and room.
    // Remote players are ALWAYS in 3rd person by definition
    if (hexFormID == 0x14 && PlayerCamera::Get()->IsFirstPerson())
    {
        hash   = AnimationGraphDescriptor_Master_Behavior::m_key;
        pGraph = AnimationGraphDescriptorManager::Get().GetDescriptor(hash);
    }
#endif

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
    if (failListed(hash))
        return nullptr;

    // Up to here the routine is pretty cheap, and WILL be called a bunch of times
    // if the only humanoid Actor is the Dragonborn/player in 1st person.
    // With that case filtered out, keep a counter to see if we need to defend against other 
    // cases (like a modded behavior where we CAN't find the signature var).
    if (invocations++ == 100)
        spdlog::warn("BehaviorVar::Patch: warning, more than 100 invocations, investigate why");
 
    spdlog::info("BehaviorVar::Patch: actor with formID {:x} with hash of {} has modded behavior", hexFormID, hash);

    // Get all animation variables for this actor, then create a reversemap to go from strings to aniamation enum.
    auto pDumpVar = apManager->DumpAnimationVariables(false);
    std::map<const std::string, const uint32_t> reversemap;
    spdlog::debug("Known behavior variables for formID {:x}:", hexFormID);
    for (auto& item : pDumpVar)
    {
        spdlog::debug("    {}:{}", item.first, item.second);
        reversemap.insert({static_cast<const std::string>(item.second), item.first});
    }

    // See if these animation variables include a signature variable for one of the replacers.
    auto iter = behaviorPool.begin();
    for (; iter < behaviorPool.end(); iter++)
        if (reversemap.find(iter->signatureVar) != reversemap.end())
            break;
    if (iter >= behaviorPool.end())
    {
        spdlog::warn("No replacer found for behavior hash {:x} (found on formID {:x}), adding to fail list", hash, hexFormID);
        failList(hash);
        return nullptr;
    }
    spdlog::info("Found match, behavior replacer signature {}", iter->signatureVar);

    // Build the set of BehaviorVar strings as sets (not vectors) to eliminate dups
    // Also, we want the set sorted, to make sure we get the same hash if mods
    // end up rearranging order, so these have to be std::set s. TiltedPhoques::Set
    // uses a hash map instead of a sorted tree, and I don't want to have to figure
    // out how to override that.
    std::set<uint32_t> boolVar;
    std::set<uint32_t> floatVar;
    std::set<uint32_t> intVar;

    // If we can find the original behavior that is being modded, 
    // get the descriptor and seed the behavior vars with it. 
    // That way mod developers only need to know what vars their
    // mod adds, they don't have to know what the STR devs picked
    const AnimationGraphDescriptor* pTmpGraph = nullptr;
    if (iter->orgHash && (pTmpGraph = AnimationGraphDescriptorManager::Get().GetDescriptor(iter->orgHash)))
    {
        boolVar.insert(pTmpGraph->BooleanLookUpTable.begin(), pTmpGraph->BooleanLookUpTable.end());
        floatVar.insert(pTmpGraph->FloatLookupTable.begin(), pTmpGraph->FloatLookupTable.end());
        intVar.insert(pTmpGraph->IntegerLookupTable.begin(), pTmpGraph->IntegerLookupTable.end());
        spdlog::info("Original game descriptor with hash {} has {} boolean, {} float, {} integer behavior vars",
                     iter->orgHash, boolVar.size(), floatVar.size(), intVar.size());
    }

    // Check requested behavior vars for those that ARE legit
    // behavior vars for this Actor AND are not yet synced
    int foundCount = 0;
    for (auto& item : iter->syncBooleanVar)
    {
        bool found = reversemap.find(item) != reversemap.end();
        if (found && boolVar.find(reversemap[item]) == boolVar.end())
        {
            if (foundCount++ == 0)
                spdlog::info("Boolean variables added to sync:");
            boolVar.insert(reversemap[item]);
            spdlog::info("    {}", item);
        }
 
    }
    if (foundCount)
        spdlog::info("Now have {} boolVar descriptors after searching {} BehavivorVar strings", boolVar.size(), iter->syncBooleanVar.size());

    foundCount = 0;
    for (auto& item : iter->syncFloatVar)
    {
        bool found = reversemap.find(item) != reversemap.end();
        if (found && floatVar.find(reversemap[item]) == floatVar.end())
        {
            if (foundCount++ == 0)
                spdlog::info("Float variables added to sync:");
            floatVar.insert(reversemap[item]);
            spdlog::info("    {}", item);
        }
    }
    if (foundCount)
        spdlog::info("Now have {} floatVar descriptors after searching {} BehavivorVar strings", floatVar.size(), iter->syncFloatVar.size());

    foundCount = 0;
    for (auto& item : iter->syncIntegerVar)
    {
        bool found = reversemap.find(item) != reversemap.end();
        if (found && intVar.find(reversemap[item]) == intVar.end())
        {
            if (foundCount++ == 0)
                spdlog::info("Int variables added to sync:");
            intVar.insert(reversemap[item]);
            spdlog::info("    {}", item);
        }
    }
    if (foundCount)
        spdlog::info("Now have {} intVar descriptors after searching {} BehavivorVar strings", intVar.size(), iter->syncIntegerVar.size());

    // Ensure we aren't over the limits. If we are, we won't update
    // the animation. Performance will be terrible unless we kill some of the logging
    // or keep track of failed signatures.
    std::string msgString;
    if (boolVar.size() > 64)
        msgString = "boolean";

    else if ((floatVar.size() + intVar.size()) > 64)
        msgString = "float+integer";

    if (msgString.size() > 0)
    {
        spdlog::error("Too many {} behavior vars to sync for actor, max is 64.", msgString);
        spdlog::error("Actor with formID {:x}, signature {}, original hash {} cannot be synced", 
                      hexFormID, iter->orgHash, iter->signatureVar);
        spdlog::error("Fail listing behavior hash {:x} found on formID {:x}", hash, hexFormID);
        failList(hash);
        return nullptr;
    }
      
    // Reshape the (sorted, unique) sets to vectors
    TiltedPhoques::Vector<uint32_t> boolVector(boolVar.begin(), boolVar.end());
    TiltedPhoques::Vector<uint32_t> floatVector(floatVar.begin(), floatVar.end());
    TiltedPhoques::Vector<uint32_t> intVector(intVar.begin(), intVar.end());

    // Construct a new descriptor
    auto panimGraphDescriptor = new AnimationGraphDescriptor();

    // This is a bit grubby, but it IS a struct with public members.
    // Maybe the devs will help us out by agreeing to an additional constructor.
    panimGraphDescriptor->BooleanLookUpTable = boolVector;
    panimGraphDescriptor->FloatLookupTable = floatVector;
    panimGraphDescriptor->IntegerLookupTable = intVector;

    // Add the new graph to the known behavior graphs
    new AnimationGraphDescriptorManager::Builder(AnimationGraphDescriptorManager::Get(), hash, *panimGraphDescriptor);
    return AnimationGraphDescriptorManager::Get().GetDescriptor(hash);
}

// Check if the behavior hash is on the failed liist
boolean BehaviorVar::failListed(uint64_t hash)
{
    auto iter = failedBehaviors.find(hash);
    return iter != failedBehaviors.end() && std::chrono::steady_clock::now() < iter->second;
}

//Place the behavior hash on the failed list
void BehaviorVar::failList(uint64_t hash)
{
    failedBehaviors.insert_or_assign(hash, std::chrono::steady_clock::now() + FAILLIST_DURATION);
}

bool dirExists(std::string aPath)
{
    return std::filesystem::is_directory(aPath);
}

std::vector<std::string> loadDirs(const std::string& acPATH)
{
    std::vector<std::string> result;
    for (auto& p : std::filesystem::directory_iterator(acPATH))
        if (p.is_directory())
            result.push_back(p.path().string());
    return result;
}

BehaviorVar::Replacer* BehaviorVar::loadReplacerFromDir(std::string aDir)
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
    std::string hashFile;
    std::string signatureFile;
    std::vector<std::string> floatVarsFile;
    std::vector<std::string> intVarsFile;
    std::vector<std::string> boolVarsFile;
    
    for (auto& p : std::filesystem::directory_iterator(aDir))
    {
        std::string path = p.path().string();
        std::string base_filename = path.substr(path.find_last_of("/\\") + 1);

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
    std::string sigVar;
    std::vector<std::string> floatVar;
    std::vector<std::string> intVar;
    std::vector<std::string> boolVar;
    std::string tempString;  // Temp string

    std::ifstream fileSig(signatureFile);
    getline(fileSig, sigVar);  // grab the signature variable
    fileSig.close();
    erase_if(sigVar, isspace); // removes any inadvertant whitespace
    if (sigVar.size() == 0)
        return nullptr;
    spdlog::info("BehaviorVar::loadReplacerFromDir found signature variable {}", sigVar);

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
        std::ifstream file(hashFile);
        getline(file, tempString);
        file.close();
        erase_if(tempString, isspace); // removes any inadvertant whitespace
        orgHash = std::strtoull(tempString.c_str(), nullptr, 10);
        spdlog::info("Replacer specifies original hash {} for {}", orgHash, sigVar);
    }

    // Build lists of variables to sync
    // Read float behavior variables
    spdlog::debug("reading float var");
    for (auto item : floatVarsFile)
    {
        std::ifstream file(item);
        while (std::getline(file, tempString))
        {
            floatVar.push_back(tempString);

            spdlog::debug("    " + tempString);
        }
        file.close();
    }

    // Read integer behavior variables
    spdlog::debug("reading int vars");
    for (auto item : intVarsFile)
    {
        std::ifstream file(item);
        while (std::getline(file, tempString))
        {
            intVar.push_back(tempString);

            spdlog::debug("    " + tempString);
        }
        file.close();
    }

    // Read boolean behavior variables
    spdlog::debug("reading bool vars");
    for (auto item : boolVarsFile)
    {
        std::ifstream file(item);
        while (std::getline(file, tempString))
        {
            boolVar.push_back(tempString);

            spdlog::debug("     " + tempString);
        }
        file.close();
    }

    // Create the replacer
    Replacer* result = new Replacer();

    result->orgHash = orgHash;
    result->signatureVar   = sigVar;
    result->syncBooleanVar = boolVar;
    result->syncFloatVar   = floatVar;
    result->syncIntegerVar = intVar;

    return result;
}

void BehaviorVar::Init()
{
    // Check if the behaviors folder exists
    const std::string behaviorPath = TiltedPhoques::GetPath().string() + "/behaviors";

    if (!dirExists(behaviorPath))
        return;

    std::vector<std::string> behaviorDirs = loadDirs(behaviorPath);

    for (auto dir : behaviorDirs)
    {
        Replacer* sig = loadReplacerFromDir(dir);
        if (sig)
        {
            behaviorPool.push_back(*sig);
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
        spdlog::info("orgHash: {} => newHash: {}", sig.orgHash, sig.newHash);

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

#endif MODDED_BEHAVIOR_COMPATIBILITY
