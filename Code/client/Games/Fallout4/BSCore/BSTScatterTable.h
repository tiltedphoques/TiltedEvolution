#pragma once

#include "BSCore/BSCRC32.h"

namespace creation
{
    // Taken off libskyrim, see himika
    // https://github.com/himika/libSkyrim

    // 00
    template <class Key>
    struct BSTScatterTableDefaultHashPolicy
    {
        typedef uint32_t	hash_type;

        inline static hash_type get_hash(const Key& key) 
        {
            uint32_t crc = 0;
            BSCRC32::GenerateCRC(crc, key, 0);
            return crc;
        }

        inline static bool is_key_equal(const Key& a, const Key& b) {
            return a == b;
        }
    };


    template <class Key, class Value>
    struct BSTScatterTableDefaultKVStorage
    {
        typedef Key		key_type;
        typedef Value	value_type;

        Key		key;
        Value	value;
    };


    template <class Key, class Value, template <class, class> class Storage = BSTScatterTableDefaultKVStorage>
    struct BSTScatterTableEntry : public Storage<Key, Value>
    {
    private:
        typedef Storage<Key, Value> _base_t;

    public:
        BSTScatterTableEntry() : next(nullptr) {
        }
        ~BSTScatterTableEntry() {
            next = nullptr;
        }

        bool empty() const {
            return next == nullptr;
        }

        // @members
        //Key					key;
        //Value					value;
        BSTScatterTableEntry* next;
    };


    // 00
    template <class Entry>
    struct BSTScatterTableHeapAllocator
    {

    public:
        typedef Entry							entry_type;
        typedef typename entry_type::key_type	key_type;
        typedef typename entry_type::value_type	value_type;

    protected:
        static inline entry_type* Allocate(size_t num) {
           /*entry_type* entries = (entry_type*)Heap_Allocate(sizeof(entry_type) * num);
            if (entries) {
                entry_type* p = entries;
                while (num) {
                    p->next = nullptr;
                    ++p; --num;
                }
            }
            return entries;*/

            return nullptr;
        }

        static inline void Free(entry_type* p) {
            //Heap_Free(p);
        }

        static inline size_t GetRelevantSize(size_t num) {
            if (num > 0)
                num = 1 << _msb(num - 1);
            return num;
        }

    private:
        static inline size_t _msb(unsigned int x)
        {
            union {
                unsigned int as_uint32_t;
                float        as_float;
            } data;
            data.as_float = (float)x + 0.5f;

            return (data.as_uint32_t >> 23) - 126;
        }
    };


    // 00
    template <class Key, class Value, template <class, class> class Storage, class HashPolicy, class Allocator, size_t INITIAL_TABLE_SIZE>
    struct BSTScatterTableTraits
    {
        typedef uint32_t				size_type;
        typedef Key					key_type;
        typedef Value				value_type;
        typedef Storage<Key, Value>	storage_type;
        typedef HashPolicy			hash_policy;
        typedef Allocator			allocator_type;

        typedef typename allocator_type::entry_type		entry_type;
        typedef typename hash_policy::hash_type			hash_type;

        const static size_type		initial_table_size = INITIAL_TABLE_SIZE;
    };


    // 15
    template <class Traits>
    class BSTScatterTableKernel :
        public Traits,						// 00
        protected Traits::hash_policy		// 01
    {
    public:
        typedef typename Traits::size_type			size_type;
        typedef typename Traits::key_type			key_type;
        typedef typename Traits::value_type			value_type;
        typedef typename Traits::storage_type		storage_type;
        typedef typename Traits::hash_policy		hash_policy;
        typedef typename Traits::allocator_type		allocator_type;
        typedef typename Traits::entry_type			entry_type;
        typedef typename Traits::hash_type			hash_type;

        using hash_policy::get_hash;
        using hash_policy::is_key_equal;

        class hasher
        {
        public:
            hasher() {}

            hash_type operator()(const key_type& key) const {
                return hash_policy::get_hash(key);
            }
        };

        class key_equal
        {
        public:
            key_equal() {}

            bool operator()(const key_type& a, const key_type& b) const {
                return hash_policy::is_key_equal(a, b);
            }
        };

        bool empty() const {
            return size() == 0;
        }

        size_type max_size() const {
            return m_size;
        }

        size_type size() const {
            return m_size - m_freeCount;
        }

        size_type free_count() const {
            return m_freeCount;
        }

    protected:
        BSTScatterTableKernel() : m_size(0), m_freeCount(0), m_freeOffset(0) {
            m_eolPtr = (entry_type*)&sentinel;
        }

        inline entry_type* _entry_at(entry_type* entries, hash_type hash) const {
            return entries + (hash & (m_size - 1));
        }

        // 005A6190
        static void _entry_new(entry_type* p, entry_type* next, const key_type& key, const value_type& value) {
            if (p) {
                new (&p->key)key_type(key);
                new (&p->value)value_type(value);
                p->next = next;
            }
        }

        static inline void _entry_move(entry_type* to, entry_type* from) {
            memcpy_s(to, sizeof(*to), from, sizeof(*from));
        }

        void _clear(entry_type* entries) {
            if (entries && m_size) {
                const entry_type* tail = entries + m_size;
                for (entry_type* p = entries; p < tail; ++p) {
                    if (!p->empty()) {
                        p->~entry_type();
                    }
                }
                m_freeCount = m_freeOffset = m_size;
            }
        }

        // 004FD070
        entry_type* _get_free_entry(entry_type* entries)
        {
            if (m_freeCount == 0)
                return nullptr;

            entry_type* p = nullptr;
            while (true)
            {
                m_freeOffset = (m_freeOffset - 1) & (max_size() - 1);
                p = entries + m_freeOffset;
                if (p->empty())
                    break;
            }

            m_freeCount--;

            return p;
        }

        entry_type* _find(const entry_type* entries, hash_type hash, const key_type& key) const {
            if (!entries)
                return nullptr;

            entry_type* p = _entry_at(const_cast<entry_type*>(entries), hash);

            if (p->empty())
                return nullptr;

            while (!is_key_equal(p->key, key)) {
                p = p->next;
                if (p == m_eolPtr) {
                    p = nullptr;
                    break;
                }
            }

            return p;
        }

        // 005B1840
        bool _insert(entry_type* entries, hash_type hash, const key_type& key, const value_type& value, uint16_t& unk) {
            if (!entries)
                return false;

            entry_type* targetEntry = _entry_at(entries, hash);
            if (targetEntry->empty()) {
                // target entry is free
                new (&targetEntry->key)key_type(key);
                new (&targetEntry->value)key_type(value);
                targetEntry->next = m_eolPtr;
                --m_freeCount;
                return true;
            }

            entry_type* p = targetEntry;
            do {
                if (is_key_equal(p->key, key)) {
                    // already exists
                    p->value = value;
                    return true;
                }
                p = p->next;
            } while (p != m_eolPtr);

            entry_type* freeEntry = _get_free_entry(entries);
            if (!freeEntry)
                return false;

            p = _entry_at(entries, get_hash(targetEntry->key));
            if (p == targetEntry) {
                // hash collision
                // insert new entry after target entry
                _entry_new(freeEntry, targetEntry->next, key, value);
                targetEntry->next = freeEntry;
                return true;
            }

            // bucket overlap
            while (p->next != targetEntry)
                p = p->next;

            _entry_move(freeEntry, targetEntry);

            p->next = freeEntry;
            _entry_new(targetEntry, m_eolPtr, key, value);
            return true;
        }

        entry_type* _insert(entry_type* entries, hash_type hash, const key_type& key) {
            if (!entries)
                return nullptr;

            entry_type* targetEntry = _entry_at(entries, hash);
            if (targetEntry->empty()) {
                // target entry is free
                --m_freeCount;
                targetEntry->next = m_eolPtr;
                new (&targetEntry->key)key_type(key);
                return targetEntry;
            }

            entry_type* p = targetEntry;
            do {
                if (is_key_equal(p->key, key)) {
                    // already exists
                    p->value.~value_type();
                    return p;
                }
                p = p->next;
            } while (p != m_eolPtr);

            entry_type* freeEntry = _get_free_entry(entries);
            if (!freeEntry)
                return nullptr;

            p = _entry_at(entries, get_hash(targetEntry->key));
            if (p == targetEntry) {
                // hash collision
                // insert new entry after target entry
                freeEntry->next = targetEntry->next;
                targetEntry->next = freeEntry;
                p = freeEntry;
            }
            else {
                // bucket overlap
                while (p->next != targetEntry)
                    p = p->next;
                p->next = freeEntry;

                _entry_move(freeEntry, targetEntry);

                targetEntry->next = m_eolPtr;
                p = targetEntry;
            }

            new (&p->key)key_type(key);
            return p;
        }

        // 008BF800
        bool _insert_move(entry_type* entries, hash_type hash, entry_type* kv) {
            if (!entries)
                return false;

            entry_type* targetEntry = _entry_at(entries, hash);
            if (targetEntry->empty()) {
                // target entry is free
                _entry_move(targetEntry, kv);
                targetEntry->next = m_eolPtr;
                --m_freeCount;
                return true;
            }

            entry_type* freeEntry = _get_free_entry(entries);

            entry_type* p = _entry_at(entries, get_hash(targetEntry->key));
            if (p == targetEntry) {
                // hash collision
                _entry_move(freeEntry, kv);
                freeEntry->next = targetEntry->next;
                targetEntry->next = freeEntry;
                return true;
            }

            // bucket overlap
            while (p->next != targetEntry)
                p = p->next;

            _entry_move(freeEntry, targetEntry);
            p->next = freeEntry;
            _entry_move(targetEntry, kv);
            targetEntry->next = m_eolPtr;

            return true;
        }

        bool _erase(entry_type* entries, hash_type hash, const key_type& key) {
            if (!entries)
                return false;

            entry_type* targetEntry = _entry_at(entries, hash);
            if (targetEntry->empty()) {
                return false;
            }

            if (is_key_equal(targetEntry->key, key)) {
                entry_type* nextEntry = targetEntry->next;
                targetEntry->~entry_type();
                ++m_freeCount;

                if (nextEntry != m_eolPtr) {
                    _entry_move(targetEntry, nextEntry);
                    nextEntry->next = nullptr;
                }
                return true;
            }

            entry_type* prevEntry = targetEntry;
            entry_type* curEntry = targetEntry->next;
            while (true)
            {
                if (curEntry == m_eolPtr)
                    return false;		// not found
                if (is_key_equal(curEntry->key, key))
                    break;				// found

                prevEntry = curEntry;
                curEntry = curEntry->next;
            }
            entry_type* nextEntry = curEntry->next;

            curEntry->~entry_type();
            ++m_freeCount;

            if (nextEntry != m_eolPtr) {
                _entry_move(curEntry, nextEntry);
                nextEntry->next = nullptr;
            }
            else {
                prevEntry->next = m_eolPtr;
            }

            return true;
        }


        // @members
        //Traits	pad00;				// 00
    public:
        uint32_t	m_size;					// 04
        uint32_t	m_freeCount;			// 08
        uint32_t	m_freeOffset;			// 0C
        entry_type* m_eolPtr;			// 10

        static uint32_t sentinel;		// DEADBEEF
    };

    template <class Traits>
    uint32_t BSTScatterTableKernel<Traits>::sentinel = 0x0EFBEADDE;	// DEADBEEF


        //1>class BSTScatterTableBase<struct BSTScatterTableTraits<struct StringCache::Ref, class NiPointer<class BGSAttackData>, struct BSTScatterTableDefaultKVStorage, struct BSTScatterTableDefaultHashPolicy<struct StringCache::Ref>, struct BSTScatterTableHeapAllocator<struct BSTScatterTableEntry<struct StringCache::Ref, class NiPointer<class BGSAttackData>, struct BSTScatterTableDefaultKVStorage> >, 8> >	size(28) :
        //1>	+-- -
        //1> 0 | +-- - (base class BSTScatterTableKernel<struct BSTScatterTableTraits<struct StringCache::Ref, class NiPointer<class BGSAttackData>, struct BSTScatterTableDefaultKVStorage, struct BSTScatterTableDefaultHashPolicy<struct StringCache::Ref>, struct BSTScatterTableHeapAllocator<struct BSTScatterTableEntry<struct StringCache::Ref, class NiPointer<class BGSAttackData>, struct BSTScatterTableDefaultKVStorage> >, 8> >)
        //1> 0	| | +-- - (base class BSTScatterTableTraits<struct StringCache::Ref, class NiPointer<class BGSAttackData>, struct BSTScatterTableDefaultKVStorage, struct BSTScatterTableDefaultHashPolicy<struct StringCache::Ref>, struct BSTScatterTableHeapAllocator<struct BSTScatterTableEntry<struct StringCache::Ref, class NiPointer<class BGSAttackData>, struct BSTScatterTableDefaultKVStorage> >, 8>)
        //1>	| | +-- -
        //1> 1	| | +-- - (base class BSTScatterTableDefaultHashPolicy<struct StringCache::Ref>)
        //1>	| | +-- -
        //1>  	| | <alignment member> (size = 3)
        //1> 4	| | m_size
        //1> 8	| | m_freeCount
        //1>0C	| | m_freeOffset
        //1>10	| | m_eolPtr
        //1> | +-- -
        //1>19 | +-- - (base class BSTScatterTableHeapAllocator<struct BSTScatterTableEntry<struct StringCache::Ref, class NiPointer<class BGSAttackData>, struct BSTScatterTableDefaultKVStorage> >)
        //1> | +-- -
        //1> | <alignment member> (size = 7)
        //1>20 | m_entries
        //1>	+-- -

    template <class Traits>
    class BSTScatterTableBase :
        public BSTScatterTableKernel<Traits>,	// 00
        protected Traits::allocator_type		// 18
    {
    public:
        typedef typename creation::BSTScatterTableKernel<Traits>::hasher hasher;
        typedef typename creation::BSTScatterTableKernel<Traits>::key_equal key_equal;

        typedef BSTScatterTableKernel<Traits>	Kernel;

        typedef typename Kernel::size_type			size_type;
        typedef typename Kernel::key_type			key_type;
        typedef typename Kernel::value_type			value_type;
        typedef typename Kernel::storage_type		storage_type;
        typedef typename Kernel::hash_policy		hash_policy;
        typedef typename Kernel::allocator_type		allocator_type;
        typedef typename Kernel::entry_type			entry_type;
        typedef typename Kernel::hash_type			hash_type;

        // compile hints for GCC
        using Kernel::m_size;
        using Kernel::m_freeCount;
        using Kernel::m_freeOffset;
        using Kernel::m_eolPtr;
        using Kernel::size;
        using Kernel::max_size;

        class const_iterator
        {
        public:
            typedef const_iterator		_iter;
            typedef const storage_type* pointer;
            typedef const storage_type& reference;

            const_iterator(const BSTScatterTableBase* a_table, entry_type* a_entry) : m_table(a_table), m_entry(a_entry) {
                if (a_entry) {
                    const entry_type* tail = m_table->_entry_tail();
                    while (m_entry < tail && m_entry->empty()) {
                        m_entry += 1;
                    }
                }
                else {
                    m_entry = const_cast<entry_type*>(m_table->_entry_tail());
                }
            }

            operator pointer() const {
                return static_cast<pointer>(m_entry);
            }
            reference operator*() const {
                return *(pointer)*this;
            }
            pointer operator->() const {
                return (pointer)*this;
            }

            _iter& operator++() {
                const entry_type* tail = m_table->_entry_tail();
                do {
                    m_entry += 1;
                } while (m_entry < tail && m_entry->empty());

                return *this;
            }
            _iter operator++(int) {
                _iter tmp = *this;
                ++* this;
                return tmp;
            }

            _iter& operator--() {
                const entry_type* head = m_table->_entry_head();
                do {
                    m_entry -= 1;
                } while (m_entry >= head && m_entry->empty());

                return *this;
            }
            _iter operator--(int) {
                _iter tmp = *this;
                --* this;
                return tmp;
            }

            bool operator==(const _iter& rhs) const {
                return (m_entry == rhs.m_entry);
            }
            bool operator!=(const _iter& rhs) const {
                return (!(*this == rhs));
            }

        protected:
            const BSTScatterTableBase* m_table;
            entry_type* m_entry;
        };

        class iterator : public const_iterator
        {
        public:
            typedef iterator		_iter;
            typedef storage_type* pointer;
            typedef storage_type& reference;

            // compiler hits for GCC
            using const_iterator::m_entry;

            iterator(const BSTScatterTableBase* a_table, entry_type* a_entry) : const_iterator(a_table, a_entry) {}

            operator pointer() {
                return static_cast<pointer>(m_entry);
            }
            reference operator*() {
                return *(pointer)*this;
            }
            pointer operator->() {
                return (pointer)*this;
            }
        };

        iterator begin() {
            return iterator(this, _entry_head());
        }
        const_iterator begin() const {
            return const_iterator(this, _entry_head());
        }
        const_iterator cbegin() const {
            return const_iterator(this, _entry_head());
        }

        iterator end() {
            return iterator(this, _entry_head() + m_size);
        }
        const_iterator end() const {
            return const_iterator(this, _entry_head() + m_size);
        }
        const_iterator cend() const {
            return const_iterator(this, _entry_head() + m_size);
        }

        hasher hash_function() const {
            return hasher();
        }

        key_equal key_eq() const {
            return key_equal();
        }

        inline void clear() {
            this->_clear(m_entries);
        }

    protected:
        BSTScatterTableBase() : Kernel(), m_entries(nullptr) {
        }

        ~BSTScatterTableBase() {
            if (m_entries) {
                clear();
                this->Free(m_entries);
            }
        }

        inline entry_type* _entry_head() const {
            return const_cast<entry_type*>(m_entries);
        }
        inline entry_type* _entry_last() const {
            return _entry_head() + (max_size() - 1);
        }
        inline entry_type* _entry_tail() const {
            return _entry_head() + max_size();
        }

        inline entry_type* _get_free_entry() {
            return BSTScatterTableKernel<Traits>::_get_free_entry(m_entries);
        }

        // 008BF970
        void _grow_table() {
            size_type oldSize = m_size;
            entry_type* oldEntries = m_entries;

            m_size = (oldSize) ? (oldSize * 2) : this->initial_table_size;
            m_entries = (entry_type*)allocator_type::Allocate(m_size);
            m_freeCount = m_size;
            m_freeOffset = m_size;

            if (m_entries && m_size) {
                // init entries
                entry_type* p = m_entries;
                for (size_type i = m_size; i; --i) {
                    p->next = nullptr;
                    ++p;
                };
            }

            // move from old entries to new one
            if (oldEntries) {
                entry_type* from = oldEntries;
                for (size_type i = oldSize; i; --i) {
                    if (!from->empty()) {
                        hash_type hash = hash_policy::get_hash(from->key);
                        _insert_move(m_entries, hash, from);
                    }
                    ++from;
                }

                Heap_Free(oldEntries);
            }
        }

        void _assign(const BSTScatterTableBase& rhs) {
            if (!this->empty()) {
                clear();
                Free(m_entries);
                m_entries = nullptr;
            }

            if (rhs.empty())
                return;

            m_entries = allocator_type::Allocate(rhs.m_size);
            if (m_entries) {
                m_size = rhs.m_size;
                m_freeCount = rhs.m_freeCount;
                m_freeOffset = rhs.m_freeOffset;

                entry_type* to = m_entries;
                entry_type* from = rhs.m_entries;
                size_t count = m_size;
                while (--count) {
                    if (!from->empty()) {
                        new (&to->key)key_type(from->key);
                        new (&to->value)value_type(from->value);
                        to->next = (from->next == rhs.m_eolPtr) ? m_eolPtr : (m_entries + (from - rhs.m_entries));
                    }
                    ++to; ++from;
                }
            }
        }

        void _assign(BSTScatterTableBase&& rhs) {
            if (!this->empty()) {
                clear();
                Free(m_entries);
                m_entries = nullptr;
            }

            m_size = rhs.m_size;
            m_freeCount = rhs.m_freeCount;
            m_freeOffset = rhs.m_freeOffset;
            m_eolPtr = rhs.m_eolPtr;
            m_entries = rhs.m_entries;

            rhs.m_size = 0;
            rhs.m_freeCount = 0;
            rhs.m_freeOffset = 0;
            rhs.m_entries = nullptr;
        }

        // @members
        //Traits	pad00;			     	00
        //uint32_t	m_size;					04
        //uint32_t	m_freeCount;			08
        //uint32_t	m_freeOffset;			0C
        //entry_type	* m_eolPtr;			10
        //allocator_type			pad14;				// 18
    public:
        entry_type* m_entries;	// 20
    };


    //1>class BSTScatterTable<struct StringCache::Ref, class NiPointer<class BGSAttackData>, struct BSTScatterTableDefaultKVStorage, struct BSTScatterTableDefaultHashPolicy, struct BSTScatterTableHeapAllocator, 8>	size(28) :
    //1>	+-- -
    //1> 0 | +-- - (base class BSTScatterTableBase<struct BSTScatterTableTraits<struct StringCache::Ref, class NiPointer<class BGSAttackData>, struct BSTScatterTableDefaultKVStorage, struct BSTScatterTableDefaultHashPolicy<struct StringCache::Ref>, struct BSTScatterTableHeapAllocator<struct BSTScatterTableEntry<struct StringCache::Ref, class NiPointer<class BGSAttackData>, struct BSTScatterTableDefaultKVStorage> >, 8> >)
    //1> 0	| | +-- - (base class BSTScatterTableKernel<struct BSTScatterTableTraits<struct StringCache::Ref, class NiPointer<class BGSAttackData>, struct BSTScatterTableDefaultKVStorage, struct BSTScatterTableDefaultHashPolicy<struct StringCache::Ref>, struct BSTScatterTableHeapAllocator<struct BSTScatterTableEntry<struct StringCache::Ref, class NiPointer<class BGSAttackData>, struct BSTScatterTableDefaultKVStorage> >, 8> >)
    //1> 0	| | | +-- - (base class BSTScatterTableTraits<struct StringCache::Ref, class NiPointer<class BGSAttackData>, struct BSTScatterTableDefaultKVStorage, struct BSTScatterTableDefaultHashPolicy<struct StringCache::Ref>, struct BSTScatterTableHeapAllocator<struct BSTScatterTableEntry<struct StringCache::Ref, class NiPointer<class BGSAttackData>, struct BSTScatterTableDefaultKVStorage> >, 8>)
    //1>	| | | +-- -
    //1> 1	| | | +-- - (base class BSTScatterTableDefaultHashPolicy<struct StringCache::Ref>)
    //1>	| | | +-- -
    //1>  	| | | <alignment member> (size = 3)
    //1> 4	| | | m_size
    //1> 8	| | | m_freeCount
    //1>0C	| | | m_freeOffset
    //1>10	| | | m_eolPtr
    //1>	| | +-- -
    //1>19	| | +-- - (base class BSTScatterTableHeapAllocator<struct BSTScatterTableEntry<struct StringCache::Ref, class NiPointer<class BGSAttackData>, struct BSTScatterTableDefaultKVStorage> >)
    //1>	| | +-- -
    //1>  	| | <alignment member> (size = 7)
    //1>20	| | m_entries
    //1> | +-- -
    //1>	+-- -
    template <
        class Key,
        class Value,
        template <class, class> class Storage,
        template <class> class Policy,
        template <class> class Allocator,
        size_t INITIAL_TABLE_SIZE
    >
        class BSTScatterTable : public BSTScatterTableBase< BSTScatterTableTraits<Key, Value, Storage, Policy<Key>, Allocator<BSTScatterTableEntry<Key, Value, Storage> >, INITIAL_TABLE_SIZE> >
    {
    public:
        typedef BSTScatterTableBase< BSTScatterTableTraits<Key, Value, Storage, Policy<Key>, Allocator<BSTScatterTableEntry<Key, Value, Storage> >, INITIAL_TABLE_SIZE> > Base;

        typedef typename Base::size_type		size_type;
        typedef typename Base::key_type			key_type;
        typedef typename Base::value_type		value_type;
        typedef typename Base::storage_type		storage_type;
        typedef typename Base::hash_policy		hash_policy;
        typedef typename Base::allocator_type	allocator_type;
        typedef typename Base::entry_type		entry_type;
        typedef typename Base::hash_type		hash_type;

        // compiler hints for GCC
        using typename Base::const_iterator;
        using typename Base::iterator;
        using Base::m_size;
        using Base::m_freeCount;
        using Base::m_freeOffset;
        using Base::m_eolPtr;
        using Base::m_entries;
        using Base::size;
        using Base::max_size;
        using Base::free_count;
        using Base::initial_table_size;
        using Base::empty;
        using Base::clear;
        using Base::hash_function;
        using Base::key_eq;


        BSTScatterTable() : Base() {
        }
        BSTScatterTable(const BSTScatterTable& rhs) : Base() {
            _assign(rhs);
        }
        BSTScatterTable(BSTScatterTable&& rhs) {
            _assign(rhs);
        }

        BSTScatterTable& operator=(const BSTScatterTable& rhs) {
            _assign(rhs);
            return *this;
        }
        BSTScatterTable& operator=(BSTScatterTable&& rhs) {
            _assign(rhs);
            return *this;
        }

        const_iterator find(const key_type& key) const {
            entry_type* entry = _find(m_entries, get_hash(key), key);
            return entry ? const_iterator(this, entry) : this->cend();
        }
        iterator find(const key_type& key) {
            entry_type* entry = _find(m_entries, get_hash(key), key);
            return entry ? iterator(this, entry) : this->end();
        }

        size_type count(const key_type& key) const {
            return _find(m_entries, get_hash(key), key) ? 1 : 0;
        }

        bool insert(const key_type& key, const value_type& lvalue) {
            hash_type hash = get_hash(key);
            entry_type* p = nullptr;

            while (!(p = _insert(m_entries, hash, key))) {
                this->_grow_table();
                if (!m_entries || !m_freeCount)
                    return false;
            }

            new (&p->value)value_type(lvalue);
            return true;
        }
        bool insert(const key_type& key, value_type&& rvalue) {
            hash_type hash = get_hash(key);
            entry_type* p = nullptr;

            while (!(p = _insert(m_entries, hash, key))) {
                this->_grow_table();
                if (!m_entries || !m_freeCount)
                    return false;
            }

            new (&p->value)value_type(rvalue);
            return true;
        }
        // original implementation
        bool insert(const key_type& key, const value_type& value, uint16_t& unk) {
            hash_type hash = get_hash(key);
            while (!insert_original(m_entries, hash, key, value, unk)) {
                this->_grow_table();
            }
            return true;
        }

        inline size_type erase(const key_type& key) {
            return _erase(m_entries, get_hash(key), key) ? 1 : 0;
        }

        bool GetAt(key_type key, value_type& value) const {
            entry_type* entry = _find(m_entries, get_hash(key), key);
            if (entry) {
                value = entry->value;
                return true;
            }
            return false;
        }

        inline bool SetAt(key_type key, const value_type& lvalue) {
            return insert(key, lvalue);
        }

        inline bool SetAt(key_type key, value_type&& rvalue) {
            return insert(key, rvalue);
        }

        inline bool RemoveAt(key_type key) {
            return erase(key) != 0;
        }

        inline void RemoveAll() {
            clear();
        }

        // @members
        //BSTScatterTableTraits		pad00;				// 00 
        //size_type					m_size;				// 04
        //size_type					m_freeCount;		// 08
        //size_type					m_freeOffset;		// 0C
        //entry_type				* m_eolPtr;			// 10
        //allocator_type			pad14;				// 18
        //entry_type				* m_entries;		// 20
    };


    //1>class BSTDefaultScatterTable<struct StringCache::Ref, class NiPointer<class BGSAttackData> >	size(28) :
    //1>	+-- -
    //1> 0 | +-- - (base class BSTScatterTable<struct StringCache::Ref, class NiPointer<class BGSAttackData>, struct BSTScatterTableDefaultKVStorage, struct BSTScatterTableDefaultHashPolicy, struct BSTScatterTableHeapAllocator, 8>)
    //1> 0	| | +-- - (base class BSTScatterTableBase<struct BSTScatterTableTraits<struct StringCache::Ref, class NiPointer<class BGSAttackData>, struct BSTScatterTableDefaultKVStorage, struct BSTScatterTableDefaultHashPolicy<struct StringCache::Ref>, struct BSTScatterTableHeapAllocator<struct BSTScatterTableEntry<struct StringCache::Ref, class NiPointer<class BGSAttackData>, struct BSTScatterTableDefaultKVStorage> >, 8> >)
    //1> 0	| | | +-- - (base class BSTScatterTableKernel<struct BSTScatterTableTraits<struct StringCache::Ref, class NiPointer<class BGSAttackData>, struct BSTScatterTableDefaultKVStorage, struct BSTScatterTableDefaultHashPolicy<struct StringCache::Ref>, struct BSTScatterTableHeapAllocator<struct BSTScatterTableEntry<struct StringCache::Ref, class NiPointer<class BGSAttackData>, struct BSTScatterTableDefaultKVStorage> >, 8> >)
    //1> 0	| | | | +-- - (base class BSTScatterTableTraits<struct StringCache::Ref, class NiPointer<class BGSAttackData>, struct BSTScatterTableDefaultKVStorage, struct BSTScatterTableDefaultHashPolicy<struct StringCache::Ref>, struct BSTScatterTableHeapAllocator<struct BSTScatterTableEntry<struct StringCache::Ref, class NiPointer<class BGSAttackData>, struct BSTScatterTableDefaultKVStorage> >, 8>)
    //1>	| | | | +-- -
    //1> 1	| | | | +-- - (base class BSTScatterTableDefaultHashPolicy<struct StringCache::Ref>)
    //1>	| | | | +-- -
    //1>  	| | | | <alignment member> (size = 3)
    //1> 4	| | | | m_size
    //1> 8	| | | | m_freeCount
    //1>0C	| | | | m_freeOffset
    //1>10	| | | | m_eolPtr
    //1>	| | | +-- -
    //1>19	| | | +-- - (base class BSTScatterTableHeapAllocator<struct BSTScatterTableEntry<struct StringCache::Ref, class NiPointer<class BGSAttackData>, struct BSTScatterTableDefaultKVStorage> >)
    //1>	| | | +-- -
    //1>  	| | | <alignment member> (size = 7)
    //1>20	| | | m_entries
    //1>	| | +-- -
    //1> | +-- -
    //1>	+-- -
    //1>
    template <class Key, class Value>
    struct BSTDefaultScatterTable : public BSTScatterTable<Key, Value, BSTScatterTableDefaultKVStorage, BSTScatterTableDefaultHashPolicy, BSTScatterTableHeapAllocator, 8>
    {
    };

    using Test = BSTDefaultScatterTable<uint32_t, uint32_t>;

    //static_assert(sizeof(Test) == 0x28);
}
