// lockfree_hashmap.hpp
// ----by wenqzheng
//-----------------------------------------------------------------------------

#pragma once

#include "shared_ptr_wrapper.hpp"
#include <cassert>
#include <cstddef>
#include <iterator>
#include <utility>
#include <atomic>
#include <urcu-bp.h>
#include <urcu/rculfhash.h>


template<typename keyType, typename valueType,
    class hashFunc = std::hash<keyType>>
class lockfree_hashmap
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
    lockfree_hashmap(unsigned long init_size = 1,
        unsigned long min_nr_alloc_buckets = 1,
        unsigned long max_nr_buckets = 0)
        :ht(cds_lfht_new(init_size,
            min_nr_alloc_buckets,
            max_nr_buckets,
            CDS_LFHT_AUTO_RESIZE|CDS_LFHT_ACCOUNTING,
            NULL))
    {}

    ~lockfree_hashmap()
    {
        cds_lfht_destroy(ht, NULL);
    }

    class iterator:public std::iterator<std::input_iterator_tag, valueType,
        std::ptrdiff_t, valueType*, valueType&>
    {
        struct mynode* pos;
        cds_lfht* ht;

    public:
        cds_lfht_iter iter;

        explicit iterator(cds_lfht* ht_)
            :ht(ht_)
        {}

        iterator& operator++()
        {
            cds_lfht_next(ht, &iter);
            return *this;
        }

        iterator operator++(int)
        {
            iterator retval = *this;
            ++(*this);
            return retval;
        }

        bool operator==(iterator other) const
        {
            return cds_lfht_iter_get_node(const_cast<cds_lfht_iter*>(&iter)) ==
                cds_lfht_iter_get_node(const_cast<cds_lfht_iter*>(&(other.iter)));
        }

        bool operator!=(iterator other) const
        {
            return !(*this == other);
        }

        nodeType& operator*() const
        {
            cds_lfht_node* ht_node = cds_lfht_iter_get_node(
                const_cast<cds_lfht_iter*>(&iter));
            mynode* node = caa_container_of(ht_node, struct mynode, node);
            return node->kvItem;
        }

        nodeType* operator->() const
        {
            mynode* pnode = caa_container_of(cds_lfht_iter_get_node(&iter),
                mynode, node);
            return pnode->kvItem;
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
            if (!ret) {
                mynode* del_node = caa_container_of(ht_node, mynode, node);
                call_rcu(&del_node->rcu_head, free_node);
                deleted = true;
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

    iterator find(const keyType& __key)
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

        iterator itr(ht);
        itr.iter = iter;
        return itr;
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

    iterator begin()
    {
        iterator itr(ht);
        cds_lfht_first(ht, &itr.iter);
        return itr;
    }

    iterator end()
    {
        iterator itr(ht);
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


