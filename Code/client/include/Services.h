#pragma once

template<class T, class U>
auto BindEvent(T apFunc, U apInstance)
{
    return std::bind(apFunc, apInstance, std::placeholders::_1, std::placeholders::_2);
}
