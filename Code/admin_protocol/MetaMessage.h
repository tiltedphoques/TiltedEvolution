#pragma once

namespace details
{
template <class T> struct MetaMessage
{
    using Type = T;
};


} // namespace details


template <class... T>
auto CreateMessageVisitor = [](auto&& f) mutable 
{
    auto expender = [f](auto&&... xs) { (... && !f(xs)); };

    expender(::details::MetaMessage<T>{}...);
};
