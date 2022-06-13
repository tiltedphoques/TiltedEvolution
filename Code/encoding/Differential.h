#pragma once


template <class T>
struct Differential
{
    [[nodiscard]] static Differential Prepare(const T& aPreviousValue) noexcept;
    [[nodiscard]] static Differential Make(const T& aPreviousValue, const T& aNextValue) noexcept;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;

private:

    Differential() = default;

    T m_previousValue{};
    std::optional<T> m_nextValue{};
};

template <class T>
Differential<T> Differential<T>::Prepare(const T& aPreviousValue) noexcept
{
    Differential<T> diff;
    diff.m_previousValue = aPreviousValue;

    return diff;
}

template <class T>
Differential<T> Differential<T>::Make(const T& aPreviousValue, const T& aNextValue) noexcept
{
    Differential<T> diff;
    diff.m_previousValue = aPreviousValue;
    diff.m_nextValue = aNextValue;

    return diff;
}

template <class T>
void Differential<T>::Serialize(TiltedPhoques::Buffer::Writer& aWriter) noexcept
{
    m_nextValue.GenerateDifferential(m_previousValue, aWriter);
}

template <class T>
void Differential<T>::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    m_nextValue = m_previousValue;
    m_nextValue.ApplyDifferential(aReader);
}
