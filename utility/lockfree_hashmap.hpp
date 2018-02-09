// lockfree_hashmap.hpp
// ----by wenqzheng
//-----------------------------------------------------------------------------

#pragma once

#include "shared_ptr_wrapper.hpp"
#include <cstddef>
#include <iterator>
#include <utility>
#include <atomic>
#include <urcu-qsbr.h>
#include <urcu/rculfhash.h>
#include <functional>

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
        const keyType* key = (const keyType*)(_key);

        return *(const_cast<keyType*>(key)) == node->kvItem.first;
    }

    static void free_node(struct rcu_head* head)
    {
        struct mynode* node = caa_container_of(head, struct, rcu_head);
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
            NULL)),
        mapsize(0)
    {}

    ~lockfree_hashmap();
    {
        cds_lfht_destroy(ht, NULL);
    }

    class iterator:public std::iterator<std::input_iterator_tag, valueType, std::ptrdiff_t, valueType*. valueType&>
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

        bool iterator==(iterator other) const
        {
            return cds_lfht_iter_get_node(const_cast<cds_lfht_iter*>(&iter)) ==
                cds_lfht_iter_get_node(const_cast<cds_lfht_iter*>(&(other.iter)));
        }

        bool iterator!=(iterator other) const
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

    

    void insert(const keyType& __key, const valueType& __value);
    void remove(const keyType& __key);
    valueType find(const keyType& __key);
    void clear();
    unsigned long size();
};


