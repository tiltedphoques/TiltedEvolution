
#include <BSAnimationGraphManager.h>
#include <Games/ActorExtension.h>

#if 0
#include<Havok / BShkbHkxDB.h>
#endif

#include <ModCompat/BehaviorVar.h>

BehaviorVar* BehaviorVar::single = nullptr;
BehaviorVar* BehaviorVar::Get()
{
    if (!BehaviorVar::single)
        BehaviorVar::single = new BehaviorVar();
    return BehaviorVar::single;
}

const AnimationGraphDescriptor* BehaviorVarPatch(BSAnimationGraphManager* apManager, Actor* apActor)
{
    return BehaviorVar::Get()->Patch(apManager, apActor);
}

const AnimationGraphDescriptor* BehaviorVar::Patch(BSAnimationGraphManager* apManager, Actor* apActor)
{
    // Make sure this isn't called so many times we need to filter out "never works" cases.
    invocations++;
    
    // Check if the animation descriptor has already been built.
    uint32_t hexFormID = apActor->formID;
    auto pExtendedActor = apActor->GetExtension();
    auto hash = pExtendedActor->GraphDescriptorHash;
    const AnimationGraphDescriptor* pGraph = AnimationGraphDescriptorManager::Get().GetDescriptor(hash);

    // If we found the descriptor we're done.
    if (pGraph)
        return pGraph;
    spdlog::info("Patch: actor with formID {:x} with hash of {} has modded behavior", hexFormID, hash);

    // Get all animation variables for this actor, then create a reversemap to go from strings to aniamation enum.
    auto pDumpVar = apManager->DumpAnimationVariables(false);
    std::map<const std::string, const uint32_t> reversemap;
    spdlog::info("Known behavior variables for formID {:x}:", hexFormID);
    for (auto& item : pDumpVar)
    {
        spdlog::info("{}:{}", item.first, item.second);
        reversemap.insert({static_cast<const std::string>(item.second), item.first});
    }

    // See if these animation variables include a signature variable for one of the replacers.
    auto iter = behaviorPool.begin();
    for (; iter < behaviorPool.end(); iter++)
        if (reversemap.find(iter->signatureVar) != reversemap.end())
            break;
    if (iter >= behaviorPool.end())
    {
        spdlog::info("No replacer found");
        return nullptr;
    }

    spdlog::info("Found match, behavior replacer signature {}", iter->signatureVar);

    // Build the set of BehaviorVar strings as sets (not vectors) to eliminate dups
    TiltedPhoques::Set<uint32_t> boolVar;
    TiltedPhoques::Set<uint32_t> floatVar;
    TiltedPhoques::Set<uint32_t> intVar;

    // If we can find the original behavior that is being modded, 
    // get the descriptor and seed the behavior vars with it. 
    // That way mod developers only need to know what vars their
    // mod adds, they don't have to know what the STR devs picked

    if (iter->orgHash)
    {
        const AnimationGraphDescriptor* pTmpGraph = AnimationGraphDescriptorManager::Get().GetDescriptor(iter->orgHash);
        if (pTmpGraph)
        {
            boolVar.insert(pTmpGraph->BooleanLookUpTable.begin(), pTmpGraph->BooleanLookUpTable.end());
            floatVar.insert(pTmpGraph->FloatLookupTable.begin(), pTmpGraph->FloatLookupTable.end());
            intVar.insert(pTmpGraph->IntegerLookupTable.begin(), pTmpGraph->IntegerLookupTable.end());
            spdlog::info("Original descriptor with hash {} has {} boolean, {} float, {} integer behavior vars",
                         iter->orgHash, boolVar.size(), floatVar.size(), intVar.size());
        }
    }

    bool found;
    for (auto& item : iter->syncBooleanVar)
    {
        found = reversemap.find(item) != reversemap.end();
        if (!found)
            spdlog::info("boolVar {} not found in reversemap", item);
        else
        {
            spdlog::info("boolVar {}:{} found in reversemap", item, reversemap[item]);
            if (boolVar.find(reversemap[item]) == boolVar.end())
                boolVar.insert(reversemap[item]);
            else
               spdlog::info("boolVar {} already in descriptor", item);
        }
    }
    spdlog::info("Now have {} boolVar descriptors after searching {} BehavivorVar strings", boolVar.size(), iter->syncBooleanVar.size());

     for (auto& item : iter->syncFloatVar)
    {
        found = reversemap.find(item) != reversemap.end();
        if (!found)
            spdlog::info("floatVar {} not found in reversemap", item);
        else
        {
            spdlog::info("floatVar {}:{} found in reversemap", item, reversemap[item]);
            if (floatVar.find(reversemap[item]) == floatVar.end())
                floatVar.insert(reversemap[item]);
            else
                spdlog::info("floatVar {} already in descriptor", item);
        }
    } 
     spdlog::info("Now have {} floatVar descriptors after searching {} BehavivorVar strings", floatVar.size(),
                 iter->syncFloatVar.size());
 
     for (auto& item : iter->syncIntegerVar)
     {
        found = reversemap.find(item) != reversemap.end();
        if (!found)
            spdlog::info("intVar {} not found in reversemap", item);
        else
        {
            spdlog::info("intVar {}:{} found in reversemap", item, reversemap[item]);
            if (intVar.find(reversemap[item]) == intVar.end())
                intVar.insert(reversemap[item]);
            else
                spdlog::info("intVar {} already in descriptor", item);
        }
     }
     spdlog::info("Now have {} intVar descriptors after searching {} BehaviorVar strings", intVar.size(),
                  iter->syncIntegerVar.size());
 
    // Reshape the sets to vectors
    TiltedPhoques::Vector<uint32_t> boolVector(boolVar.begin(), boolVar.end());
    TiltedPhoques::Vector<uint32_t> floatVector(floatVar.begin(), floatVar.end());
    TiltedPhoques::Vector<uint32_t> intVector(intVar.begin(), intVar.end());

    // Construct a new descriptor
    auto panimGraphDescriptor = new AnimationGraphDescriptor();
    panimGraphDescriptor->BooleanLookUpTable = boolVector;
    panimGraphDescriptor->FloatLookupTable = floatVector;
    panimGraphDescriptor->IntegerLookupTable = intVector;

    // Add the new graph to the var graph
    new AnimationGraphDescriptorManager::Builder(AnimationGraphDescriptorManager::Get(), hash, *panimGraphDescriptor);
    return AnimationGraphDescriptorManager::Get().GetDescriptor(hash);
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

    std::string hashFile;
    std::string signatureFile;
    std::vector<std::string> floatVarsFile;
    std::vector<std::string> intVarsFile;
    std::vector<std::string> boolVarsFile;
    
    for (auto& p : std::filesystem::directory_iterator(aDir))
    {
        std::string path = p.path().string();
        std::string base_filename = path.substr(path.find_last_of("/\\") + 1);

        spdlog::info("base_path: {}", base_filename);

        if (base_filename.find("__sig.txt") != std::string::npos)
        {
            signatureFile = path;

            spdlog::info("signature variable file: {}", signatureFile);
        }
        else if (base_filename.find("__hash.txt") != std::string::npos)
        {
            hashFile = path;

            spdlog::info("hash file: {}", hashFile);
        }
        else if (base_filename.find("__float.txt") != std::string::npos)
        {
            floatVarsFile.push_back(path);

            spdlog::info("float file: {}", path);
        }
        else if (base_filename.find("__int.txt") != std::string::npos)
        {
            intVarsFile.push_back(path);

            spdlog::info("int file: {}", path);
        }
        else if (base_filename.find("__bool.txt") != std::string::npos)
        {
            boolVarsFile.push_back(path);

            spdlog::info("bool file: {}", path);
        }
    }

    // Check that there is a signature file (an identifying variable)
    // When an Actor's animation descriptor isn't found, we search
    // its animation/behavior varibles for this to match it to modded behavior
    if (signatureFile == "")
        return nullptr;

#if 0
    uint64_t orgHash;
    uint64_t newHash;
#endif
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
    spdlog::info("Replacer found signature variable {}", sigVar);

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
    unsigned long long orgHash = 0;
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
    spdlog::info("reading float var");
    for (auto item : floatVarsFile)
    {
        std::ifstream file(item);
        while (std::getline(file, tempString))
        {
            floatVar.push_back(tempString);

            spdlog::info(" " + tempString);
        }
        file.close();
    }

    // Read integer behavior variables
    spdlog::info("reading int var");
    for (auto item : intVarsFile)
    {
        std::ifstream file(item);
        while (std::getline(file, tempString))
        {
            intVar.push_back(tempString);

            spdlog::info(" " + tempString);
        }
        file.close();
    }

    // Read boolean behavior variables
    spdlog::info("reading bool var");
    for (auto item : boolVarsFile)
    {
        std::ifstream file(item);
        while (std::getline(file, tempString))
        {
            boolVar.push_back(tempString);

            spdlog::info(" " + tempString);
        }
        file.close();
    }


    // create the sig
    Replacer* result = new Replacer();

 #if 0
    // This should become dead
    //result->newHash = newHash;
#endif

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

    // Replace loaded descriptors
    // BehaviorVar::ReplaceDescriptors();
}

void BehaviorVar::ReplaceDescriptors()
{
    // Replace all in pool
    for (const auto& sig : behaviorPool)
    {
        spdlog::info("Replacing behavior hash {} with {}", sig.orgHash, sig.newHash);

        auto pDescriptor = AnimationGraphDescriptorManager::Get().GetDescriptor(sig.orgHash);

        if (pDescriptor == nullptr)
            continue;

        // Remake the anim graph descriptor and insert new bools, floats, ints
        TiltedPhoques::Vector<uint32_t> boolVar;
        TiltedPhoques::Vector<uint32_t> floatVar;
        TiltedPhoques::Vector<uint32_t> intVar;

        for (auto var : pDescriptor->BooleanLookUpTable)
        {
            boolVar.push_back(var);
        }

        for (auto var : pDescriptor->FloatLookupTable)
        {
            floatVar.push_back(var);
        }

        for (auto var : pDescriptor->IntegerLookupTable)
        {
            intVar.push_back(var);
        }

        // TODO: Insert new bools, floats, ints
#if 0
        const AnimationGraphDescriptor* pGraph =
            AnimationGraphDescriptorManager::Get().GetDescriptor(sig.orgHash);

        BSAnimationGraphManager* apManager;
        IAnimationGraphManagerHolder::GetBSAnimationGraph(&apManager);
        auto dumpVar = apManager->DumpAnimationVariables(true);
        //std::unordered_map<std::string, uint32_t> reverseMap;
#endif

        auto mDescriptor = new AnimationGraphDescriptor({0}, {0}, {0});
        mDescriptor->BooleanLookUpTable = boolVar;
        mDescriptor->FloatLookupTable = floatVar;
        mDescriptor->IntegerLookupTable= intVar;

        // Update the existing descriptor
        AnimationGraphDescriptorManager::Get().Update(sig.orgHash, sig.newHash, *mDescriptor);

        //AnimationGraphDescriptorManager::Get().UpdateKey(sig.orgHash, sig.newHash);
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
