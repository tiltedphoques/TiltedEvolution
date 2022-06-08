#pragma once

struct CachedString : TiltedPhoques::String
{
    CachedString() = default;
    ~CachedString() = default;

    CachedString& operator=(const TiltedPhoques::String& acRhs) noexcept;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;
};
