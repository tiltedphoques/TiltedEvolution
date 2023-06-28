#include <ModCompat/BehaviorVar.h>

#include <Structs/AnimationGraphDescriptorManager.h>

#include <BSAnimationGraphManager.h>
#include <Havok/BShkbHkxDB.h>

BehaviorVar* BehaviorVar::single = nullptr;

BehaviorVar* BehaviorVar::Get()
{
    if (!BehaviorVar::single)
        BehaviorVar::single = new BehaviorVar();
    return BehaviorVar::single;
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
    std::vector<std::string> floatVarsFile;
    std::vector<std::string> intVarsFile;
    std::vector<std::string> boolVarsFile;
    
    for (auto& p : std::filesystem::directory_iterator(aDir))
    {
        std::string path = p.path().string();
        std::string base_filename = path.substr(path.find_last_of("/\\") + 1);

        spdlog::info("base_path: {}", base_filename);

        if (base_filename.find("__hash.txt") != std::string::npos)
        {
            hashFile = path;

            spdlog::info("name file: {}", hashFile);
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

    // Check that there is a hash replacer within the folder
    if (hashFile == "")
        return nullptr;

    // Prepare reading files
    uint64_t orgHash;
    uint64_t newHash;
    std::set<std::string> floatVar;
    std::set<std::string> intVar;
    std::set<std::string> boolVar;

    std::string tempString;  // Temp string

    // Read the hash replacer file
    std::ifstream file(hashFile);
    getline(file, tempString);
    file.close();

    // split string to get replacer
    size_t equalPos = tempString.find('=>');

    if (equalPos != std::string::npos)
    {
        std::string orgHashStr = tempString.substr(0, equalPos);
        std::string newHashStr = tempString.substr(equalPos + 1);

        if (orgHashStr == "" || newHashStr == "")
            return nullptr;

        orgHash = std::strtoull(orgHashStr.c_str(), nullptr, 10);
        newHash = std::strtoull(newHashStr.c_str(), nullptr, 10);
    }
    else
    {
        return nullptr;
    }

    spdlog::info("Replacer found for {} => {}", orgHash, newHash);

    // Read float behavior variables
    spdlog::info("reading float var");
    for (auto item : floatVarsFile)
    {
        std::ifstream file2(item);
        while (std::getline(file2, tempString))
        {
            floatVar.insert(tempString);

            spdlog::info(tempString);
        }
        file2.close();
    }

    // Read integer behavior variables
    spdlog::info("reading int var");
    for (auto item : intVarsFile)
    {
        std::ifstream file3(item);
        while (std::getline(file3, tempString))
        {
            intVar.insert(tempString);

            spdlog::info(tempString);
        }
        file3.close();
    }

    // Read boolean behavior variables
    spdlog::info("reading bool var");
    for (auto item : boolVarsFile)
    {
        std::ifstream file4(item);
        while (std::getline(file4, tempString))
        {
            boolVar.insert(tempString);

            spdlog::info(tempString);
        }
        file4.close();
    }

    // convert set to vector
    std::vector<std::string> floatVector;
    std::vector<std::string> intVector;
    std::vector<std::string> boolVector;

    for (auto item : floatVar)
    {
        floatVector.push_back(item);
    }

    for (auto item : intVar)
    {
        intVector.push_back(item);
    }

    for (auto item : boolVar)
    {
        boolVector.push_back(item);
    }

    // create the sig
    Replacer* result = new Replacer();

    result->orgHash = orgHash;
    result->newHash = newHash;
    result->syncBooleanVar = boolVector;
    result->syncFloatVar = floatVector;
    result->syncIntegerVar = intVector;

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
    BehaviorVar::ReplaceDescriptors();
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
