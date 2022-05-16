#pragma once

#include <Windows.h>
#include <fstream>
#include <map>
#include <stdio.h>

#pragma comment(lib, "version.lib")

class VersionDb
{
public:
    VersionDb()
    {
        Clear();
    }
    ~VersionDb()
    {
    }

    static VersionDb& Get();

private:
    TiltedPhoques::Map<uintptr_t, uintptr_t> _data;
    TiltedPhoques::Map<uintptr_t, uintptr_t> _rdata;
    int _ver[4];
    std::string _verStr;
    std::string _moduleName;
    uintptr_t _base;

    template <typename T> static T read(std::ifstream& file)
    {
        T v;
        file.read((char*)&v, sizeof(T));
        return v;
    }

    static void* ToPointer(uintptr_t v)
    {
        return (void*)v;
    }

    static uintptr_t FromPointer(void* ptr)
    {
        return (uintptr_t)ptr;
    }

    static bool ParseVersionFromString(const char* ptr, int& major, int& minor, int& revision, int& build)
    {
        return sscanf_s(ptr, "%d.%d.%d.%d", &major, &minor, &revision, &build) == 4 &&
               ((major != 1 && major != 0) || minor != 0 || revision != 0 || build != 0);
    }

public:
    const std::string& GetModuleName() const
    {
        return _moduleName;
    }
    const std::string& GetLoadedVersionString() const
    {
        return _verStr;
    }

    const TiltedPhoques::Map<uintptr_t, uintptr_t>& GetOffsetMap() const
    {
        return _data;
    }

    void* FindAddressById(uintptr_t id) const
    {
        uintptr_t b = _base;
        if (b == 0)
            return NULL;

        uintptr_t offset = 0;
        if (!FindOffsetById(id, offset))
            return NULL;

        return ToPointer(b + offset);
    }

    bool FindOffsetById(uintptr_t id, uintptr_t& result) const
    {
        auto itr = _data.find(id);
        if (itr != _data.end())
        {
            result = itr->second;
            return true;
        }
        return false;
    }

    bool FindIdByAddress(void* ptr, uintptr_t& result) const
    {
        uintptr_t b = _base;
        if (b == 0)
            return false;

        uintptr_t addr = FromPointer(ptr);
        return FindIdByOffset(addr - b, result);
    }

    bool FindIdByOffset(uintptr_t offset, uintptr_t& result) const
    {
        auto itr = _rdata.find(offset);
        if (itr == _rdata.end())
            return false;

        result = itr->second;
        return true;
    }

    bool GetExecutableVersion(int& major, int& minor, int& revision, int& build) const
    {
        TCHAR szVersionFile[MAX_PATH];
        GetModuleFileName(NULL, szVersionFile, MAX_PATH);

        DWORD verHandle = 0;
        UINT size = 0;
        LPBYTE lpBuffer = NULL;
        DWORD verSize = GetFileVersionInfoSize(szVersionFile, &verHandle);

        if (verSize != NULL)
        {
            LPSTR verData = new char[verSize];

            if (GetFileVersionInfo(szVersionFile, verHandle, verSize, verData))
            {
                {
                    char* vstr = NULL;
                    UINT vlen = 0;
                    if (VerQueryValueA(verData, "\\StringFileInfo\\040904B0\\ProductVersion", (LPVOID*)&vstr, &vlen) &&
                        vlen && vstr && *vstr)
                    {
                        if (ParseVersionFromString(vstr, major, minor, revision, build))
                        {
                            delete[] verData;
                            return true;
                        }
                    }
                }

                {
                    char* vstr = NULL;
                    UINT vlen = 0;
                    if (VerQueryValueA(verData, "\\StringFileInfo\\040904B0\\FileVersion", (LPVOID*)&vstr, &vlen) &&
                        vlen && vstr && *vstr)
                    {
                        if (ParseVersionFromString(vstr, major, minor, revision, build))
                        {
                            delete[] verData;
                            return true;
                        }
                    }
                }
            }

            delete[] verData;
        }

        return false;
    }

    void GetLoadedVersion(int& major, int& minor, int& revision, int& build) const
    {
        major = _ver[0];
        minor = _ver[1];
        revision = _ver[2];
        build = _ver[3];
    }

    void Clear()
    {
        _data.clear();
        _rdata.clear();
        for (int i = 0; i < 4; i++)
            _ver[i] = 0;
        _moduleName = std::string();
        _base = 0;
    }

    bool Load(const std::filesystem::path& acGamePath, const TiltedPhoques::String& acExeVersion)
    {
        int major, minor, revision, build;

        if (!ParseVersionFromString(acExeVersion.c_str(), major, minor, revision, build))
        {
            return false;
        }

        return Load(acGamePath, major, minor, revision, build);
    }

    bool Load(const std::filesystem::path& acGamePath, int major, int minor, int revision, int build)
    {
        Clear();

        char fileName[256];
        _snprintf_s(fileName, 256, "versionlib-%d-%d-%d-%d.bin", major, minor, revision, build);

        std::ifstream file(acGamePath / "Data" / "SKSE" / "Plugins" / fileName, std::ios::binary);
        if (!file.good())
            return false;

        int format = read<int>(file);

        if (format != 2)
            return false;

        for (int i = 0; i < 4; i++)
            _ver[i] = read<int>(file);

        {
            char verName[64];
            _snprintf_s(verName, 64, "%d.%d.%d.%d", _ver[0], _ver[1], _ver[2], _ver[3]);
            _verStr = verName;
        }

        int tnLen = read<int>(file);

        if (tnLen < 0 || tnLen >= 0x10000)
            return false;

        if (tnLen > 0)
        {
            char* tnbuf = (char*)malloc(tnLen + 1);
            file.read(tnbuf, tnLen);
            tnbuf[tnLen] = '\0';
            _moduleName = tnbuf;
            free(tnbuf);
        }

        {
            HMODULE handle = GetModuleHandleA(NULL);
            _base = (uintptr_t)handle;
        }

        int ptrSize = read<int>(file);

        int addrCount = read<int>(file);

        unsigned char type, low, high;
        unsigned char b1, b2;
        unsigned short w1, w2;
        unsigned int d1, d2;
        uintptr_t q1, q2;
        uintptr_t pvid = 0;
        uintptr_t poffset = 0;
        uintptr_t tpoffset;
        for (int i = 0; i < addrCount; i++)
        {
            type = read<unsigned char>(file);
            low = type & 0xF;
            high = type >> 4;

            switch (low)
            {
            case 0:
                q1 = read<uintptr_t>(file);
                break;
            case 1:
                q1 = pvid + 1;
                break;
            case 2:
                b1 = read<unsigned char>(file);
                q1 = pvid + b1;
                break;
            case 3:
                b1 = read<unsigned char>(file);
                q1 = pvid - b1;
                break;
            case 4:
                w1 = read<unsigned short>(file);
                q1 = pvid + w1;
                break;
            case 5:
                w1 = read<unsigned short>(file);
                q1 = pvid - w1;
                break;
            case 6:
                w1 = read<unsigned short>(file);
                q1 = w1;
                break;
            case 7:
                d1 = read<unsigned int>(file);
                q1 = d1;
                break;
            default: {
                Clear();
                return false;
            }
            }

            tpoffset = (high & 8) != 0 ? (poffset / (uintptr_t)ptrSize) : poffset;

            switch (high & 7)
            {
            case 0:
                q2 = read<uintptr_t>(file);
                break;
            case 1:
                q2 = tpoffset + 1;
                break;
            case 2:
                b2 = read<unsigned char>(file);
                q2 = tpoffset + b2;
                break;
            case 3:
                b2 = read<unsigned char>(file);
                q2 = tpoffset - b2;
                break;
            case 4:
                w2 = read<unsigned short>(file);
                q2 = tpoffset + w2;
                break;
            case 5:
                w2 = read<unsigned short>(file);
                q2 = tpoffset - w2;
                break;
            case 6:
                w2 = read<unsigned short>(file);
                q2 = w2;
                break;
            case 7:
                d2 = read<unsigned int>(file);
                q2 = d2;
                break;
            default:
                throw std::exception();
            }

            if ((high & 8) != 0)
                q2 *= (uintptr_t)ptrSize;

            _data[q1] = q2;
            _rdata[q2] = q1;

            poffset = q2;
            pvid = q1;
        }

        return true;
    }

    bool Dump(const std::string& path)
    {
        std::ofstream f = std::ofstream(path.c_str());
        if (!f.good())
            return false;

        for (auto& it : _data)
        {
            f << std::dec;
            f << it.first;
            f << '\t';
            f << std::hex;
            f << it.second + 0x140000000;
            f << '\n';
        }

        return true;
    }

    bool DumpVersionIDC(const std::string& path);
    bool CreateMapping(const std::string& inf, const std::string& outf);
};

template <class T> 
struct VersionDbPtr
{
    VersionDbPtr(const uint32_t aId) noexcept
        : m_pPtr{nullptr}
        , m_id{aId}
    {
    }

    VersionDbPtr() = delete;
    VersionDbPtr(VersionDbPtr&) = delete;
    VersionDbPtr& operator=(VersionDbPtr&) = delete;

    operator T*() const noexcept
    {
        return Get();
    }

    T* operator->() const noexcept
    {
        return Get();
    }

    T* Get() const noexcept
    {
        return static_cast<T*>(GetPtr());
    }

    void* GetPtr() const noexcept
    {
        if (m_pPtr == nullptr)
            m_pPtr = VersionDb::Get().FindAddressById(m_id);

        return m_pPtr;
    }

private:

    mutable void* m_pPtr;
    uint32_t m_id;
};
