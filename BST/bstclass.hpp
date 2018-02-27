// bstree.hpp
// ----by wenqzheng
//-----------------------------------------------------------------------------

#pragma once

#include "utility.hpp"
#include "smart_ptr_wrapper.hpp"
#include <algorithm>
#include <atomic>
#include <cstdint>
#include <climits>

template<typename dataType, class compare = std::less<datatype>>
class bstree
{
    constexpr std::size_t inf1 = SIZE_MAX-1;
    constexpr std::size_t inf2 = SIZE_MAX;

    struct relinfo;
    struct treenode;

    struct alignas(2 * sizeof(shared_ptr_wrapper<void>)) updateflag
    {
        std::atomic_bool isDirty;
        shared_ptr_wrapper<relinfo> info;

        updateflag(std::atomic_bool __isDirty = false,
            shared_ptr_wrapper<relinfo> __info = nullptr):
            isDirty(__isDirty),
            info(__info)
        {}
    };

    struct alignas(__power2(sizeof(dataType))) entity
    {
        dataType data;
        treenode node;
    
        entity(const dataType& __data, const treenode& __node):
            data(__data), node(__node)
        {}
    };

    struct alignas(__CACHE_LINE_SIZE) treenode
    {
        std::atomic_bool isLeaf;
        shared_ptr_wrapper<entity> left;
        shared_ptr_wrapper<entity> right;
        shared_ptr_wrapper<updateflag> update;

        treenode(std::atomic_bool __isLeaf = true,
            shared_ptr_wrapper<entity> __left = nullptr,
            shared_ptr_wrapper<entity> __right = nullptr,
            shared_ptr_wrapper<updateflag> __update =
                std::make_shared<updateflag>()):
            isLeaf(__isLeaf)
            left(__left),
            right(__right),
            update(__update)
        {}
    };

    struct alignas(__CACHE_LINE_SIZE) relinfo
    {
        shared_ptr_wrapper<entity> parent;
        shared_ptr_wrapper<entity> leaf;
        shared_ptr_wrapper<entity> subtree;

        relinfo(shared_ptr_wrapper<entity> __parent = nullptr,
            shared_ptr_wrapper<entity> __leaf = nullptr,
            shared_ptr_wrapper<entity> __subtree = nullptr):
            parent(__parent),
            leaf(__leaf),
            subtree(__subtree)
        {}
    };

    struct searchresult
    {
        shared_ptr_wrapper<entity> parent;
        shared_ptr_wrapper<entity> self;
        shared_ptrwrapper<updateflag> pupdate;

        searchresult(const shared_ptr_wrapper<entity>& __parent = nullptr,
            const shared_ptr_wrapper<entity>& __self = nullptr,
            const shared_ptr_wrapper<updateflag> __pupdate) = NULL:
            parent(__parent), self(__self), pupdate(__pupdate)
        {}
    };

    shared_ptr_wrapper<entity> root;

public:
    bstree()
    {
        shared_ptr_wrapper<entity> rleft(
            std::make_shared<entity>(inf1, treenode()));
        shared_ptr_wrapper<entity> rright(
            std::make_shared<entity>(inf2, treenode()));
        root = std::make_shared<entity>(inf2, treenode(false, rleft, rright));
    }

    shared_ptr_wrapper<searchresult> lookup(const dataType& __data)
    {
        shared_ptr_wrapper<entity> parent;
        shared_ptr_wrapper<entity> self;
        shared_ptr_wrapper<updateflag> pupdate;

        self = root;
        do {
            parent = self;
            pupdate = self->node.update;

            if (__data < self->data)
                self = parent->node.left;
            else
                self = parent->node.right;
        } while (!(self->node.isLeaf))

        return std::make_shared<searchresult>(parent, self, pupdate);
    }

    bool insert(const dateType& __data)
    {
        shared_ptr_wrapper<entity> parent;
        shared_ptr_wrapper<entity> self;
        shared_ptr_wrapper<entity> sibling;
        shared_ptr_wrapper<entity> newentity;
        shared_ptr_wrapper<entity> newinternal;

        shared_ptr_wrapper<relinfo> record;

        newentity = std::make_shared<entity>(__data, treenode(true));

        while (true) {
            shared_ptr_wrapper<searchresult> search = lookup(__data);
            parent = search->parent;
            self = search->self;
            shared_ptr_wrapper<updateflag> pupdate = search->pupdate;

            if (self->data == __data)
                return false;
            if (pupdate->isDirty)
                helpinsert(pupdate->info);
            else {
                sibling = std::make_shared<entity>(self->data, treenode(true));
                if (newentity->data < sibling->data)
                    newinternal = std::make_shared<entity>(
                        std::max(__data, self->data, compare()),
                        treenode(false, newnode,sibling));
                else
                    newinternal = std::make_shared<entity>(
                        std::max(__data, self->data, compare()),
                        treenode(false, sibling, newnode));
                record = std::make_shared<relinfo>(parent, self, newinternal);

                shared_ptr_wrapper<updateflag> dirty =
                    std::make_shared<updateflag>(true, record);
                
                if (parent->update.cas_strong(pupdate, dirty)) {
                    helpinsert(record);
                    return true;
                } else
                    helpinsert(pupdate->info);
            }
        }
    }

    void helpinsert(const shared_ptr_wrapper<relinfo>& __record)
    {
        shared_ptr_wrapper<entity> parent;
        shared_ptr_wrapper<entity> leaf;
        shared_ptr_wrapper<entity> subtree;

        parent = __record->parent;
        leaf = __record->leaf;
        subtree = __record->subtree;

        shared_ptr_wrapper<updateflag> dirty =
            std::make_shared<updateflag>(true, __record);
        shared_ptr_wrapper<updateflag> clean =
            std::make_shared<updateflag>(false, __record);

        shared_ptr_wrapper<entity>* tochange;
        if (subtree->data < parent->data)
            tochange = &(parent->left);
        else
            tochange = &(parent->right);
        tochange->cas_strong(leaf, subtree);

        __record->parent->node.update.cas_strong(dirty, clean);
    }
};

