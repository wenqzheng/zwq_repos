// hashmap_multiple.hpp
// ----by wenqzheng
// make use of liburcu
//-----------------------------------------------------------------------------

#pragma once

#include "shared_ptr_wrapper.hpp"
#include <cstdint>
#include <cassert>
#include <thread>
#include <cstddef>
#include <iterator>
#include <utility>
#include <atomic>
#include <urcu-bp.h>
#include <urcu/rculfhash.h>

#define __likely(x) __builtin_expect(!!(x),1)
#define __unlikely(x) __builtin_expect(!!(x),0)

namespace __inner_wg
{
    constexpr unsigned long __power2(const unsigned long& size)
    {
        assert(size >= 0 && size <= ULONG_MAX);
        if (0 == size)
            return 0;
        else {
            for (auto i = 0; i < 63; ++i)
                if (!(size >> (i + 1)))
                    return (1 << i);
        }

        return 0;
    }
};

template<typename keyType, typename valueType,
    class hashFunc = std::hash<keyType>>
class hashmap
{
public:
    using nodeType = std::pair<keyType, valueType>;

private:
    hashFunc hash_fn;
    struct cds_lfht* ht;

    struct mynode
    {
        nodeType kvItem;
        struct cds_lfht_node node;
        struct rcu_head rcu_head;
    };

    static int match(struct cds_lfht_node* ht_node, const void* _key)
    {
        struct mynode* node = caa_container_of(ht_node, struct mynode, node);
        const keyType* key = reinterpret_cast<const keyType*>(_key);

        return *(const_cast<keyType*>(key)) == node->kvItem.first;
    }

    static void free_node(struct rcu_head* head)
    {
        struct mynode* node = caa_container_of(head, struct mynode, rcu_head);
        free(node);
    }

public:
    hashmap(unsigned long init_size = std::thread::hardware_concurrency(),
        unsigned long min_nr_alloc_buckets = std::thread::hardware_concurrency(),
        unsigned long max_nr_buckets = 0)
        :ht(cds_lfht_new(__inner_wg::__power2(init_size),
            __inner_wg::__power2(min_nr_alloc_buckets),
            __inner_wg::__power2(max_nr_buckets),
            CDS_LFHT_AUTO_RESIZE|CDS_LFHT_ACCOUNTING,
            NULL))
    {}

    ~hashmap()
    {
        cds_lfht_destroy(ht, NULL);
    }

    class hashmap_iterator:public std::iterator<std::input_iterator_tag,
        valueType, std::ptrdiff_t, valueType*, valueType&>
    {
        cds_lfht* ht;
        cds_lfht_iter iter;

        friend class hashmap;
    public:
        explicit hashmap_iterator(cds_lfht* ht_)
            :ht(ht_)
        {}

        hashmap_iterator& operator++()
        {
            cds_lfht_next(ht, &iter);
            return *this;
        }

        hashmap_iterator operator++(int)
        {
            hashmap_iterator retval = *this;
            ++(*this);
            return retval;
        }

        bool operator==(hashmap_iterator other) const
        {
            return cds_lfht_iter_get_node(const_cast<cds_lfht_iter*>(&iter)) ==
                cds_lfht_iter_get_node(const_cast<cds_lfht_iter*>(&(other.iter)));
        }

        bool operator!=(hashmap_iterator other) const
        {
            return !(*this == other);
        }

        nodeType& operator*() const
        {
            cds_lfht_node* ht_node = cds_lfht_iter_get_node(
                const_cast<cds_lfht_iter*>(&iter));
            mynode* pnode = caa_container_of(ht_node, struct mynode, node);
            return pnode->kvItem;
        }

        nodeType* operator->()
        {
            mynode* pnode = caa_container_of(cds_lfht_iter_get_node(&iter),
                mynode, node);
            return &pnode->kvItem;
        }
    };

    void insert(const nodeType& kvpair)
    {
        struct mynode* node = (mynode*)malloc(sizeof(*node));
        assert(node);
        cds_lfht_node_init(&node->node);
        node->kvItem = kvpair;
        unsigned long hash = hash_fn(node->kvItem.first);

        rcu_read_lock();
        cds_lfht_add(ht, hash, &node->node);
        rcu_read_unlock();
    }

    void insert(const keyType& __key, const valueType& __val)
    {
        nodeType tmpnode = std::make_pair(__key, __val);
        insert(tmpnode);
    }

    int remove(const keyType& __key)
    {
        cds_lfht_iter iter;
        cds_lfht_node* ht_node;
        bool found = false;
        bool deleted = false;
        int ret;

        unsigned long hash = hash_fn(__key);

        rcu_read_lock();
        cds_lfht_lookup(ht, hash, match, &__key, &iter);
        ht_node = cds_lfht_iter_get_node(&iter);
        if (ht_node) {
            found = true;
            ret = cds_lfht_del(ht, ht_node);
            if (__likely(!ret)) {
                mynode* del_node = caa_container_of(ht_node, mynode, node);
                call_rcu(&del_node->rcu_head, free_node);
                deleted = true;
            } else {
                std::this_thread::yield();
                cds_lfht_node* tmp_node = cds_lfht_iter_get_node(&iter);
                if (!tmp_node)
                    deleted = true;
                else
                    found = false;
            }
        } else {
            found = false;
        }
        rcu_read_unlock();

        if (!found)
            return 0;
        assert(deleted == true);

        return 1;
    }

    hashmap_iterator find(const keyType& __key)
    {
        struct cds_lfht_node* ht_node;
        struct cds_lfht_iter iter;
        struct mynode* node;

        unsigned long hash = hash_fn(__key);

        rcu_read_lock();
        cds_lfht_lookup(ht, hash, match, &__key, &iter);
        ht_node = cds_lfht_iter_get_node(&iter);
        rcu_read_unlock();

        if (!ht_node)
            return end();

        hashmap_iterator itr(ht);
        itr.iter = iter;
        return itr;
    }
   
    valueType& operator[](const keyType& __key)
    {
        auto iter = find(__key);
        return (*iter).second;
    } 

    void for_each(const std::function<void(nodeType*)>& __func)
    {
        struct mynode* node;
        cds_lfht_iter iter;
        cds_lfht_node* ht_node;

        rcu_read_lock();
        cds_lfht_for_each_entry(ht, &iter, node, node) {
            __func(&(node->kvItem));
        }
        rcu_read_unlock();
    }

    void for_each(const std::function<void(nodeType)>& __func)
    {
        struct mynode* node;
        cds_lfht_iter iter;
        cds_lfht_node* ht_node;

        rcu_read_lock();
        cds_lfht_for_each_entry(ht, &iter, node, node) {
            __func(node->kvItem);
        }
        rcu_read_unlock();
    }


    hashmap_iterator begin()
    {
        hashmap_iterator itr(ht);
        cds_lfht_first(ht, &itr.iter);
        return itr;
    }

    hashmap_iterator end()
    {
        hashmap_iterator itr(ht);
        itr.iter.node = NULL;
        return itr;
    }

    unsigned long size()
    {
        long sp_count_bef;
        unsigned long count;
        long sp_count_aft;

        rcu_read_lock();
        cds_lfht_count_nodes(ht, &sp_count_bef, &count, &sp_count_aft);
        rcu_read_unlock();

        return count;
    }

};
