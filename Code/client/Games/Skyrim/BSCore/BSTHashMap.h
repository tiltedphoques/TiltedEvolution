#pragma once

#include "BSCore/BSTScatterTable.h"

namespace creation
{
    // Verified to be same on FT as ST

    // Taken off libskyrim, see himika
    // https://github.com/himika/libSkyrim

    template <class Key, class Value, class Table>
    struct BSTHashMapTraits
    {
        typedef Table	table_type;
    };

    template <class Traits>
    class BSTHashMapBase :
        public Traits,
        public Traits::table_type
    {
    };

    template <class Key, class Value, template <class, class> class Table = BSTDefaultScatterTable>
    class BSTHashMap : public BSTHashMapBase< BSTHashMapTraits<Key, Value, Table<Key, Value>> >
    {
    };

    //static_assert(sizeof(BSTHashMap<uint32_t, void*>) == 0x30);
}
