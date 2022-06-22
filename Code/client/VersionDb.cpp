#include "VersionDb.h"

VersionDb& VersionDb::Get()
{
    static VersionDb s_db;
    return s_db;
}

bool VersionDb::DumpVersionIDC(const std::string& acPath)
{
    std::ofstream f(acPath.c_str());
    if (!f.good())
        return false;

    for (auto& it : _data)
        f << fmt::format("MakeRptCmt(0x{:x},\"Adl: {}\");\n", it.second + 0x140000000, it.first);

    return true;
}

constexpr char kPrelude[] =
    R"(#include <ida.idc>
static safename(ea, name) {
 if(!has_user_name(GetFlags(ea))){
   MakeName(ea, name);
}
}
static main() {
)";

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to)
{
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

/*
bool VersionDb::CreateMapping(const std::string& inf, const std::string& outf)
{
    std::ofstream out(outf.c_str());
    if (!out.good())
        return false;
    std::ifstream in(inf.c_str());
    if (!in.good())
        return false;

    out << kPrelude;

    std::string line;
    while (std::getline(in, line))
    {
        line = ReplaceAll(line, "_*", "");
        // sanitize for ida
        // std::replace(line.begin(), line.end(), ':', '_');
        // std::replace(line.begin(), line.end(), '_*', '');

        size_t p = line.find_first_of(' ');
        if (p == std::string::npos)
            continue;
        size_t var = 0;
        std::from_chars(line.c_str(), line.data() + p, var);

        if (p + 1 >= line.length())
            continue;

        auto* name = &line[p + 1];
        if (!name)
            continue;

        const VersionDbPtr<uint8_t> vptr(var);
        out << fmt::format("safename(0x{:x},\"{}\");\n", reinterpret_cast<uintptr_t>(vptr.Get()), name);
    }
    out << "}";

    return false;
}
*/
