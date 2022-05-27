#pragma once

#include <cstdint>

namespace creation
{
    class BSTArrayHeapAllocator
    {
    public:
        inline ~BSTArrayHeapAllocator()
        {
            if (m_pBuffer)
                Deallocate();

            m_uiBufferSize = 0;
        }

        bool Allocate(uint32_t auiMinSize, uint32_t auiSize);
        void Deallocate();
        bool Reallocate(uint32_t aiMinNewSizeInItems, uint32_t aiFrontCopyCount, uint32_t aiShiftCount, uint32_t aiBackCopyCount, uint32_t aiElemSize);
 
        inline void* GetBuffer() const;
        inline uint32_t GetBufferSize() const;

    protected:
        void* m_pBuffer = nullptr;
        uint32_t m_uiBufferSize = 0;
    };

    static_assert(sizeof(BSTArrayHeapAllocator) == 0x10);

    class BSTArrayBase
    {
    public:
        inline ~BSTArrayBase()
        {
            m_uiSize = 0;
        }

        struct IAllocatorFunctor
        {
            virtual bool Allocate(uint32_t auiSize, uint32_t auiItemSize) = 0;
            virtual bool Reallocate(uint32_t aiMinNewSizeInItems, uint32_t aiFrontCopyCount, uint32_t aiShiftCount, uint32_t aiBackCopyCount, uint32_t aiElemSize) = 0;
            virtual void Deallocate() = 0;
            // dtor comes last:
            virtual ~IAllocatorFunctor() {};
        };
        
        bool InitialReserve(IAllocatorFunctor* arFunctor, uint32_t auiReserveSize, uint32_t auiElemSize)
        {
            if (auiReserveSize)
                return arFunctor->Allocate(auiReserveSize, auiElemSize);
            else
                return true;
        }

        bool SetAllocSize(IAllocatorFunctor* arFunctor, uint32_t auiAllocSize, uint32_t auiNewAllocSize, uint32_t auiElemSize);
        void MoveItems(void* apBuffer, uint32_t aDestIndex, uint32_t aSrcIndex, uint32_t aCount, uint32_t aElemSize);
        int32_t AddUninitialized(IAllocatorFunctor& arFunctor, uint32_t aAllocSize, uint32_t aElemSize);
        bool InsertUninitialized(IAllocatorFunctor& arFunctor, void* apBuffer, uint32_t auiAllocSize, uint32_t auiIndex, uint32_t auiElemSize);

        uint32_t GetSize() const
        {
            return m_uiSize;
        }

    protected:
        uint32_t m_uiSize = 0;
    };

    template<class TAllocator>
    struct BSTArrayAllocatorFunctor : public BSTArrayBase::IAllocatorFunctor
    {
        inline BSTArrayAllocatorFunctor(TAllocator *apAllocator) : pAllocator(apAllocator) {}

        bool Allocate(uint32_t auiSize, uint32_t auiItemSize) override { return pAllocator->Allocate(auiSize, auiItemSize); }
        bool Reallocate(uint32_t aiMinNewSizeInItems, uint32_t aiFrontCopyCount, uint32_t aiShiftCount, uint32_t aiBackCopyCount, uint32_t aiElemSize) override
        {
            return pAllocator->Reallocate(aiMinNewSizeInItems, aiFrontCopyCount, aiShiftCount, aiBackCopyCount, aiElemSize);
        }

        void Deallocate() override {
            pAllocator->Deallocate();
        }

        ~BSTArrayAllocatorFunctor() override {
        }

        TAllocator* pAllocator;
    };

    static_assert(sizeof(BSTArrayBase) == 4);

    template<class T, class Allocator = BSTArrayHeapAllocator>
    class BSTArray final : public Allocator, public BSTArrayBase
    {
    public:
        using Functor_t = BSTArrayAllocatorFunctor<Allocator>;

        explicit BSTArray(uint32_t aiReserveSize)
        {
            Functor_t functor(this);
            InitialReserve(&functor, aiReserveSize, sizeof(T));
        }

        BSTArray() = default;

        T* operator[](uint32_t aIndex);

        const T* GetAt(uint32_t aIndex) const;
        T* GetAt(uint32_t aIndex);
        const T* GetFirst();
        const T* GetLast() const;
        T* GetLast();

		template<class T>
		class Iterator
		{
		public:
            Iterator(T* apCurrent) : pCurrent(apCurrent)
            {
            }

			Iterator operator++()
			{
				BSASSERT(!pCurrent, "Invalid iterator");

				++this->pCurrent;
				return *this;
			}

			bool operator==(const Iterator& acRhs) const { return pCurrent == acRhs.pCurrent; }
			bool operator!=(const Iterator& acRhs) const { return pCurrent != acRhs.pCurrent; }

			T& operator*() const { return *pCurrent; }
			T* operator->() const { return pCurrent; }

		private:
			T* pCurrent;
		};

		static_assert(sizeof(Iterator<uint32_t>) == 0x8);

        Iterator<T> begin()
        {
            if (m_uiSize)
                return Iterator<T>(reinterpret_cast<T*>(this->m_pBuffer));
            else
                return Iterator<T>(nullptr);
        }

        Iterator<T> end()
        {
            if (m_uiSize)
                return Iterator<T>(GetAt(m_uiSize));
			else
                return Iterator<T>(nullptr);
        }

		// T needs copy semantics implemented
        int64_t Add(const T* apValue);
        bool Insert(uint32_t aIndex, const T* apValue);
        void Remove(uint32_t aIndex, uint32_t aHowMany);
        void Clear(bool aFreeMemory);
        int64_t Find(const T& arValue);
        bool IsInArray(const T& arValue);

        void SetAllocSize(uint32_t aSize)
        {
            Functor_t functor(this);
            BSTArrayBase::SetAllocSize(&functor, this->m_uiBufferSize, aSize, sizeof(T));
        }
    };
} // namespace creation

#include "BSTArray.inl"
