// freelist.hpp
// ----by wenqzheng
//-----------------------------------------------------------------------------

#pragma once

#include "accelerator.hpp"
#include "tagged_ptr.hpp"
#include <limits>
#include <memory>
#include <array>
#include <type_traits>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <atomic>

template <typename T, typename Alloc = std::allocator<T>>
class cache_freelist:Alloc
{
    struct freelist_node
    {
        tagged_ptr<freelist_node> next;
    };

    using tagged_node_ptr = tagged_ptr<freelist_node>;
    
    std::atomic<tagged_node_ptr> cache_pool;

public:
    using index_t = T*;
    using tagged_node_handle = tagged_ptr<T>;

private:
    void deallocate_impl(index_t n)
    {
        void* node = reinterpret_cast<void*>(n);
        tagged_node_ptr old_pool = cache_pool.load(std::memory_order_consume);
        freelist_node* new_pool_ptr = reinterpret_cast<freelist_node*>(node);

        for (;;) {
            tagged_node_ptr new_pool(new_pool_ptr, old_pool.get_tag());
            new_pool->next.set_ptr(old_pool.get_ptr());
            if (cache_pool.compare_exchange_weak(old_pool, new_pool))
                return;
        }
    }

    void deallocate_impl_unsafe(index_t n)
    {
        void* node = reinterpret_cast<void*>(n);
        tagged_node_ptr old_pool = cache_pool.load(std::memory_order_relaxed);
        freelist_node * new_pool_ptr = reinterpret_cast<freelist_node*>(node);

        tagged_node_ptr new_pool (new_pool_ptr, old_pool.get_tag());
        new_pool->next.set_ptr(old_pool.get_ptr());

        cache_pool.store(new_pool, std::memory_order_relaxed);
    }

    template <bool __bounded>
    index_t allocate_impl()
    {
        tagged_node_ptr old_pool = cache_pool.load(std::memory_order_consume);
        
        for (;;) {
            if (!old_pool.get_ptr()) {
                if (!__bounded)
                    return Alloc::allocate(1);
                else
                    return 0;
            }

            freelist_node* new_pool_ptr = old_pool->next.get_ptr();
            tagged_node_ptr new_pool(new_pool_ptr, old_pool.get_next_tag());
            if (cache_pool.compare_exchange_weak(old_pool, new_pool)) {
	            void* ptr = old_pool.get_ptr();
                return reinterpret_cast<T*>(ptr);
            }
        }
    }

    template <bool __bounded>
    index_t allocate_impl_unsafe()
    {
        tagged_node_ptr old_pool = cache_pool.load(std::memory_order_relaxed);

        if (!old_pool.get_ptr()) {
            if (!__bounded)
                return Alloc::allocate(1);
            else
                return 0;
        }

        freelist_node* new_pool_ptr = old_pool->next.get_ptr();
        tagged_node_ptr new_pool(new_pool_ptr, old_pool.get_next_tag());

        cache_pool.store(new_pool, std::memory_order_relaxed);
        void* ptr = old_pool.get_ptr();
        return reinterpret_cast<T*>(ptr);
    }

protected:
    template <bool __threadsafe>
    void deallocate(index_t n)
    {
        if (__threadsafe)
            deallocate_impl(n);
        else
            deallocate_impl_unsafe(n);
    }

    template <bool __threadsafe, bool __bounded>
    index_t allocate()
    {
        if (__threadsafe)
            return allocate_impl<__bounded>();
        else
            return allocate_impl_unsafe<__bounded>();
    }

public:
    cache_freelist() = default;

    cache_freelist(const cache_freelist&) = default;

    template <typename Allocator>
    cache_freelist(const Allocator& alloc, std::size_t n = 0):
        Alloc(alloc),
        cache_pool(tagged_node_ptr(NULL))
    {
        for (std::size_t i = 0; i < n; ++i) {
            index_t node = Alloc::allocate(1);
            destruct<true>(node);
        }
    }

    template <bool __threadsafe>
    void reserve(std::size_t count)
    {
        for (std::size_t i = 0; i < count; ++i) {
            T * node = Alloc::allocate(1);
            deallocate<__threadsafe>(node);
        }
    }

    template <bool __threadsafe, bool __bounded, typename... Args>
    T * construct(Args&&... args)
    {
        T * node = allocate<__threadsafe, __bounded>();
        if (node)
            new(node) T(std::forward<Args>(args)...);
        return node;
    }

    template <bool __threadsafe>
    void destruct(tagged_node_handle tagged_ptr_f)
    {
        index_t n = tagged_ptr_f.get_ptr();
        n->~T();
        deallocate<__threadsafe>(n);
    }

    ~cache_freelist()
    {
        tagged_node_ptr current = cache_pool.load();

        while (current) {
            freelist_node* current_ptr = current.get_ptr();
            if (current_ptr)
                current = current_ptr->next;
            Alloc::deallocate(reinterpret_cast<index_t>(current_ptr), 1);
        }
    }

    bool is_lock_free() const
    {
        return cache_pool.is_lock_free();
    }

    index_t get_handle(index_t pointer) const
    {
        return pointer;
    }

    index_t get_handle(const tagged_node_handle& handle) const
    {
        return get_pointer(handle);
    }

    index_t get_pointer(const tagged_node_handle& tptr) const
    {
        return tptr.get_ptr();
    }

    index_t get_pointer(index_t pointer) const
    {
        return pointer;
    }

    index_t null_handle() const
    {
        return NULL;
    }
};

class alignas(4) tagged_index
{
public:
    typedef std::uint16_t tag_t;
    typedef std::uint16_t index_t;

    tagged_index() noexcept = default;

    tagged_index(tagged_index const & rhs) = default;

    explicit tagged_index(index_t i, tag_t t = 0):
        index(i), tag(t)
    {}

    index_t get_index() const
    {
        return index;
    }

    void set_index(index_t i)
    {
        index = i;
    }
    
    tag_t get_tag() const
    {
        return tag;
    }

    tag_t get_next_tag() const
    {
        tag_t next = (get_tag() + 1u) & (std::numeric_limits<tag_t>::max)();
        return next;
    }

    void set_tag(tag_t t)
    {
        tag = t;
    }

    bool operator==(const tagged_index& rhs) const
    {
        return (index == rhs.index) && (tag == rhs.tag);
    }

    bool operator!=(const tagged_index& rhs) const
    {
        return !operator==(rhs);
    }

protected:
    index_t index;
    tag_t tag;
};

template <typename T, std::size_t size>
struct compiletime_sized_freelist_storage
{
    static_assert(size < 65536);

    std::array<char, size * sizeof(T) + 64> arrT;

    template <typename Allocator>
    compiletime_sized_freelist_storage(const Allocator&, std::size_t)
    {}

    T* nodes() const
    {
        char* data_pointer = const_cast<char*>(arrT.data());
        return reinterpret_cast<T*>((reinterpret_cast<std::size_t>(data_pointer) + CACHE_LINE_SIZE - 1) & ~(CACHE_LINE_SIZE - 1));
    }

    std::size_t node_count(void) const
    {
        return size;
    }
};

template <typename T, typename Alloc = std::allocator<T>>
struct runtime_sized_freelist_storage:
    std::allocator_traits<Alloc>::template rebind_alloc<std::aligned_storage<sizeof(T), CACHE_LINE_SIZE>>
    //aligned_allocator_adaptor<Alloc, CACHE_LINE_SIZE>
{
    typedef typename std::allocator_traits<Alloc>::template rebind_traits<std::aligned_storage<sizeof(T), CACHE_LINE_SIZE>> allocator_type;

    //aligned_allocator_adaptor<Alloc, CACHE_LINE_SIZE > allocator_type;
    T * nodes_;
    std::size_t node_count_;

    template <typename Allocator>
    runtime_sized_freelist_storage(Allocator const & alloc, std::size_t count):
        allocator_type(alloc), node_count_(count) 
    {
        assert(count > 65535);
        nodes_ = allocator_type::allocate(count);
    }

    ~runtime_sized_freelist_storage(void)
    {
        allocator_type::deallocate(nodes_, node_count_);
    }

    T * nodes(void) const
    {
        return nodes_;
    }

    std::size_t node_count(void) const
    {
        return node_count_;
    }
};


template <typename T, typename NodeStorage = runtime_sized_freelist_storage<T>>
class fixed_size_freelist:NodeStorage
{
    struct freelist_node
    {
        tagged_index next;
    };

    void initialize(void)
    {
        T * nodes = NodeStorage::nodes();
        for (std::size_t i = 0; i != NodeStorage::node_count(); ++i) {
            tagged_index * next_index = reinterpret_cast<tagged_index*>(nodes + i);
            next_index->set_index(null_handle());
            destruct<false>(nodes + i);
        }
    }

public:
    typedef tagged_index tagged_node_handle;
    typedef tagged_index::index_t index_t;

    template <typename Allocator>
    fixed_size_freelist (Allocator const & alloc, std::size_t count):
        NodeStorage(alloc, count),
        pool_(tagged_index(static_cast<index_t>(count), 0))
    {
        initialize();
    }

    fixed_size_freelist (void):
        pool_(tagged_index(NodeStorage::node_count(), 0))
    {
        initialize();
    }

    template <bool ThreadSafe = true, bool Bounded = false>
    T * construct (void)
    {
        index_t node_index = allocate<ThreadSafe>();
        if (node_index == null_handle())
            return NULL;

        T * node = NodeStorage::nodes() + node_index;
        new(node) T();
        return node;
    }

    template <bool ThreadSafe, bool Bounded, typename ArgumentType>
    T * construct (ArgumentType const & arg)
    {
        index_t node_index = allocate<ThreadSafe>();
        if (node_index == null_handle())
            return NULL;

        T * node = NodeStorage::nodes() + node_index;
        new(node) T(arg);
        return node;
    }

    template <bool ThreadSafe, bool Bounded,
	typename ArgumentType1, typename ArgumentType2>
    T * construct (ArgumentType1 const & arg1, ArgumentType2 const & arg2)
    {
        index_t node_index = allocate<ThreadSafe>();
        if (node_index == null_handle())
            return NULL;

        T * node = NodeStorage::nodes() + node_index;
        new(node) T(arg1, arg2);
        return node;
    }

    template <bool ThreadSafe>
    void destruct (tagged_node_handle tagged_index)
    {
        index_t index = tagged_index.get_index();
        T * n = NodeStorage::nodes() + index;
        (void)n;
        n->~T();
        deallocate<ThreadSafe>(index);
    }

    template <bool ThreadSafe>
    void destruct (T * n)
    {
        n->~T();
        deallocate<ThreadSafe>(n - NodeStorage::nodes());
    }

    bool is_lock_free(void) const
    {
        return pool_.is_lock_free();
    }

    index_t null_handle(void) const
    {
        return static_cast<index_t>(NodeStorage::node_count());
    }

    index_t get_handle(T * pointer) const
    {
        if (pointer == NULL)
            return null_handle();
        else
            return static_cast<index_t>(pointer - NodeStorage::nodes());
    }

    index_t get_handle(tagged_node_handle const & handle) const
    {
        return handle.get_index();
    }

    T * get_pointer(tagged_node_handle const & tptr) const
    {
        return get_pointer(tptr.get_index());
    }

    T * get_pointer(index_t index) const
    {
        if (index == null_handle())
            return 0;
        else
            return NodeStorage::nodes() + index;
    }

    T * get_pointer(T * ptr) const
    {
        return ptr;
    }

protected: // allow use from subclasses
    template <bool ThreadSafe>
    index_t allocate (void)
    {
        if (ThreadSafe)
            return allocate_impl();
        else
            return allocate_impl_unsafe();
    }

private:
    index_t allocate_impl (void)
    {
        tagged_index old_pool = pool_.load(std::memory_order_consume);

        for(;;) {
            index_t index = old_pool.get_index();
            if (index == null_handle())
                return index;

            T * old_node = NodeStorage::nodes() + index;
            tagged_index * next_index = reinterpret_cast<tagged_index*>(old_node);

            tagged_index new_pool(next_index->get_index(), old_pool.get_next_tag());

            if (pool_.compare_exchange_weak(old_pool, new_pool))
                return old_pool.get_index();
        }
    }

    index_t allocate_impl_unsafe (void)
    {
        tagged_index old_pool = pool_.load(std::memory_order_consume);

        index_t index = old_pool.get_index();
        if (index == null_handle())
            return index;

        T * old_node = NodeStorage::nodes() + index;
        tagged_index * next_index = reinterpret_cast<tagged_index*>(old_node);

        tagged_index new_pool(next_index->get_index(), old_pool.get_next_tag());

        pool_.store(new_pool, std::memory_order_relaxed);
        return old_pool.get_index();
    }

    template <bool ThreadSafe>
    void deallocate (index_t index)
    {
        if (ThreadSafe)
            deallocate_impl(index);
        else
            deallocate_impl_unsafe(index);
    }

    void deallocate_impl (index_t index)
    {
        freelist_node * new_pool_node = reinterpret_cast<freelist_node*>(NodeStorage::nodes() + index);
        tagged_index old_pool = pool_.load(std::memory_order_consume);

        for(;;) {
            tagged_index new_pool (index, old_pool.get_tag());
            new_pool_node->next.set_index(old_pool.get_index());

            if (pool_.compare_exchange_weak(old_pool, new_pool))
                return;
        }
    }

    void deallocate_impl_unsafe (index_t index)
    {
        freelist_node * new_pool_node = reinterpret_cast<freelist_node*>(NodeStorage::nodes() + index);
        tagged_index old_pool = pool_.load(std::memory_order_consume);

        tagged_index new_pool (index, old_pool.get_tag());
        new_pool_node->next.set_index(old_pool.get_index());

        pool_.store(new_pool);
    }

    std::atomic<tagged_index> pool_;
};

template <typename T,
          typename Alloc,
          bool IsCompileTimeSized,
          bool IsFixedSize,
          std::size_t Capacity
          >
struct select_freelist
{
    typedef typename std::conditional_t<IsCompileTimeSized,
                               compiletime_sized_freelist_storage<T, Capacity>,
                               runtime_sized_freelist_storage<T, Alloc>
                              > fixed_sized_storage_type;

    typedef typename std::conditional_t<IsCompileTimeSized || IsFixedSize,
                               fixed_size_freelist<T, fixed_sized_storage_type>,
                               cache_freelist<T, Alloc>
                              > type;
};

template <typename T, bool IsNodeBased>
struct select_tagged_handle
{
    typedef typename std::conditional_t<IsNodeBased,
                               tagged_ptr<T>,
                               tagged_index
                              > tagged_handle_type;

    typedef typename std::conditional_t<IsNodeBased,
                               T*,
                               typename tagged_index::index_t
                              > handle_type;
};
