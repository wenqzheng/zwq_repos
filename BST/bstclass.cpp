// bstree.hpp
// ----by wenqzheng
//-----------------------------------------------------------------------------

#include "../utility.hpp"
#include "../utility/smart_ptr_wrapper.hpp"
#include <utility>
#include <algorithm>
#include <atomic>
#include <cstdint>
#include <climits>
#include <variant>
#include <iostream>
#include <typeinfo>

template<typename U>
struct Inf0
{
    U index;
};

template<typename U>
struct Inf1
{
    U index;
};

template<typename U>
struct __lessexp
{
    using __VAR = std::variant<U, Inf0<U>, Inf1<U>>;
    constexpr bool operator()(const __VAR& __v1, const __VAR& __v2) const
    {
        if (__v1.index() != __v2.index())
            return std::less<std::size_t>()(__v1.index(), __v2.index());
        else if (0 == __v1.index())
            return std::less<U>()(*reinterpret_cast<const U*>(&__v1),
                *reinterpret_cast<const U*>(&__v2));
        else
            return false;
    }
};

template<typename U>
struct __equalexp
{
    using __VAR = std::variant<U, Inf0<U>, Inf1<U>>;
    constexpr bool operator()(const __VAR& __v1, const __VAR& __v2) const
    {
        if (__v1.index() != __v2.index())
            return false;
        else if (0 == __v1.index())
            return !std::not_equal_to<U>()(*reinterpret_cast<const U*>(&__v1),
                *reinterpret_cast<const U*>(&__v2));
        else
            return true;
    }
};

template<typename U>
struct __greaterexp
{
    using __VAR = std::variant<U, Inf0<U>, Inf1<U>>;
    constexpr bool operator()(const __VAR& __v1, const __VAR& __v2) const
    {
        if (__v1.index() != __v2.index())
            return std::greater<std::size_t>()(__v1.index(), __v2.index());
        else if (0 == __v1.index())
            return std::greater<U>()(*reinterpret_cast<const U*>(&__v1),
                *reinterpret_cast<const U*>(&__v2));
        else
            return false;
    }
};

template<typename dataType>
class bstree
{
    using __VAR = std::variant<dataType, Inf0<dataType>, Inf1<dataType>>;
    class alignas(__CACHE_LINE_SIZE) relinfo;
    class alignas(__CACHE_LINE_SIZE) entity;

    class alignas(2 * sizeof(shared_ptr_wrapper<void>)) updateflag
    {
    public:
        std::atomic_bool isDirty;
        shared_ptr_wrapper<relinfo> info;

        updateflag(const updateflag& __record):
            isDirty(__record.isDirty.load()),
            info(__record.info)
        {}

        updateflag(std::atomic_bool __isdirty = false,
            const shared_ptr_wrapper<relinfo>& __info = nullptr):
            isDirty(__isdirty.load()),
            info(__info)
        {}
    };

    class alignas(__CACHE_LINE_SIZE) treenode
    {
    public:
        std::atomic_bool isLeaf;
        shared_ptr_wrapper<entity> left;
        shared_ptr_wrapper<entity> right;
        shared_ptr_wrapper<updateflag> update;

        treenode(const treenode& __node):
            isLeaf(__node.isLeaf.load()),
            left(__node.left),
            right(__node.right),
            update(__node.update)
        {}

        treenode(treenode&& __node):
            isLeaf(__node.isLeaf.load()),
            left(__node.left),
            right(__node.right),
            update(__node.update)
        {}

        treenode(std::atomic_bool __isLeaf = true,
            const shared_ptr_wrapper<entity>& __left = nullptr,
            const shared_ptr_wrapper<entity>& __right = nullptr,
            const shared_ptr_wrapper<updateflag>& __update =
                std::make_shared<updateflag>()):
            isLeaf(__isLeaf.load()),
            left(__left),
            right(__right),
            update(__update)
        {}
    };

    class alignas(__CACHE_LINE_SIZE) entity
    {
    public:
        __VAR data;
        treenode node;
    
        entity(const entity& __entity):
            data(__VAR(__entity.data)), node(__entity.node)
        {}

        entity(const __VAR& __var,
            const treenode& __node = treenode()):
            data(__var), node(__node)
        {}

        entity(const dataType& __data = dataType(),
            const treenode& __node = treenode()):
            data(__VAR(__data)), node(__node)
        {}

        entity(const Inf0<dataType>& __data,
            const treenode& __node = treenode()):
            data(__VAR(__data)), node(__node)
        {}

        entity(const Inf1<dataType>& __data,
            const treenode& __node = treenode()):
            data(__VAR(__data)), node(__node)
        {}
    };

    class alignas(__CACHE_LINE_SIZE) relinfo
    {
    public:
        shared_ptr_wrapper<entity> parent;
        shared_ptr_wrapper<entity> leaf;
        shared_ptr_wrapper<entity> subtree;

        relinfo(const relinfo& __record):
            parent(__record.parent),
            leaf(__record.leaf),
            subtree(__record.subtree)
        {}

        relinfo(const shared_ptr_wrapper<entity>& __parent = nullptr,
            const shared_ptr_wrapper<entity>& __leaf = nullptr,
            const shared_ptr_wrapper<entity>& __subtree = nullptr):
            parent(__parent),
            leaf(__leaf),
            subtree(__subtree)
        {}
    };

    class alignas(__CACHE_LINE_SIZE) searchresult
    {
    public:
        shared_ptr_wrapper<entity> parent;
        shared_ptr_wrapper<entity> self;
        shared_ptr_wrapper<updateflag> pupdate;

        searchresult(const searchresult& __search):
            parent(__search.parent),
            self(__search.self),
            pupdate(__search.pupdate)
        {}

        searchresult(const shared_ptr_wrapper<entity>& __parent = nullptr,
            const shared_ptr_wrapper<entity>& __self = nullptr,
            const shared_ptr_wrapper<updateflag>& __pupdate =
                std::make_shared<updateflag>()):
            parent(__parent),
            self(__self),
            pupdate(__pupdate)
        {}
    };

    shared_ptr_wrapper<entity> root;

public:
    bstree()
    {
        shared_ptr_wrapper<entity> rleft =
            std::make_shared<entity>(Inf0<dataType>(), treenode(true));
        shared_ptr_wrapper<entity> rright =
            std::make_shared<entity>(Inf1<dataType>(), treenode(true));
        root = std::make_shared<entity>(
            Inf1<dataType>(), treenode(true, rleft, rright));
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

            if (__lessexp<dataType>()(__data, self->data))
                self = parent->node.left;
            else
                self = parent->node.right;
        } while (!self->node.isLeaf);

        return std::make_shared<searchresult>(parent, self, pupdate);
    }

    bool insert(const dataType& __data)
    {
        shared_ptr_wrapper<entity> parent;
        shared_ptr_wrapper<entity> self;
        shared_ptr_wrapper<entity> sibling;
        shared_ptr_wrapper<entity> newentity;
        shared_ptr_wrapper<entity> newinternal;

        newentity = std::make_shared<entity>(__data, treenode(true));
        shared_ptr_wrapper<relinfo> record;

        while (true) {        
            shared_ptr_wrapper<searchresult> search = lookup(__data);
            parent = search->parent;
            self = search->self;
            shared_ptr_wrapper<updateflag> pupdate = search->pupdate;
        
            if (__equalexp<dataType>()(self->data, __data))
                return false;
            if (pupdate->isDirty)
                helpinsert(pupdate->info);
            else {
                sibling = std::make_shared<entity>(self->data, treenode(true));
                if (__lessexp<dataType>()(newentity->data, sibling->data))
                    newinternal = std::make_shared<entity>(
                        std::max(__VAR(__data), self->data,
                            __lessexp<dataType>()),
                        treenode(false, newentity, sibling));
                else
                    newinternal = std::make_shared<entity>(
                        std::max(__VAR(__data), self->data,
                            __lessexp<dataType>()),
                        treenode(false, sibling, newentity));
                
                record = std::make_shared<relinfo>(parent, self, newinternal);

                shared_ptr_wrapper<updateflag> dirty =
                    std::make_shared<updateflag>(true, record);
                
                if (parent->node.update.cas_strong(pupdate, dirty)) {
                    helpinsert(record);
                    return true;
                } else
                    helpinsert(pupdate->info);
            }
            return true;
        }
    }

    void helpinsert(const shared_ptr_wrapper<relinfo>& __record)
    {
        shared_ptr_wrapper<updateflag> dirty =
            std::make_shared<updateflag>(true, __record);
        shared_ptr_wrapper<updateflag> clean =
            std::make_shared<updateflag>(false, __record);

        shared_ptr_wrapper<entity> parent = __record->parent;
        shared_ptr_wrapper<entity> leaf = __record->leaf;
        shared_ptr_wrapper<entity> subtree = __record->subtree;
            
        if (__lessexp<dataType>()(subtree->data, parent->data))
            parent->node.left.cas_strong(leaf, subtree);
        else
            parent->node.right.cas_strong(leaf, subtree);
        __record->parent->node.update.cas_strong(dirty, clean);
    }
};

int main()
{
    bstree<int> bst;
    bst.insert(4);
    bst.insert(8);
    bst.insert(1);
    bst.insert(2);
    bst.insert(7);
    std::cout << __equalexp<int>()(8,8) << std::endl;
    std::cout << typeid(std::atomic_bool).name() << std::endl;
    std::cout << bst.insert(9) << std::endl;
    std::cout << *reinterpret_cast<int*>(&(bst.lookup(-16)->self->data)) << std::endl;
    return 0;
}
