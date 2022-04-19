// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include <base/containers/fast_queue.hpp>
#include <console/ArgStack.h>
#include <console/Command.h>

namespace Console
{
class CommandQueue
{
  public:
    struct Item
    {
        // Strictly speaking, this also isn't thread safe...
        CommandBase* m_pCommand{nullptr};
        ArgStack m_stack;
    };

    inline bool HasWork() const
    {
        return !m_queue.empty();
    }

    // Strong-inline me.
    inline void Upload(CommandBase* apCommand, ArgStack& arStack)
    {
        // Upload Item
        Item* pItem;
        {
            auto lock = m_queue.write_acquire(pItem);
            pItem->m_pCommand = apCommand;
            pItem->m_stack = std::move(arStack);
        }
    }

    inline void Fetch(Item& arOut)
    {
        Item* pItem;
        auto lock = m_queue.read_acquire(pItem);
        {
            arOut.m_pCommand = pItem->m_pCommand;
            arOut.m_stack = std::move(pItem->m_stack);
        }
    }

  private:
    // fast queue with up to n outstanding elements.
    // (8 would be 256)
    fast_queue<Item, 2> m_queue;
};
} // namespace Console
