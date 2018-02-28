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
        std::atomic_uint64_t state;
        shared_ptr_wrapper<entityinfo> info;

        updateflag(const updateflag& __record):
            state(__record.state.load()),
            info(__record.info)
        {}

        updateflag(std::atomic_uint64_t __state = 0x1,
            const shared_ptr_wrapper<entityinfo>& __info = nullptr):
            state(__state.load()),
            info(__info)
        {}

        void setClean()
        {
            state.store(0x1);
        }

        bool isClean()
        {
            return state.load() & 0x1;
        }

        void setDflag()
        {
            state.store(0x10);
        }

        bool isDflag()
        {
            return state.load() & 0x10;
        }

        void setIflag()
        {
            state.store(0x100);
        }

        bool isIflag()
        {
            return state.load() & 0x100;
        }

        void setMark()
        {
            state.store(0x1000);
        }

        bool isMark()
        {
            return state.load() & 0x1000;
        }
    };

    class entity
    {
    public:
        virtual bool isLeaf() = 0;
    };

    class leafnode:entity
    {
    public:
        __VAR data;

        leafnode(const leafnode& __leaf):
            data(__leaf.data)
        {}

        leafnode(const __VAR& __var = __VAR()):
            data(__var)
        {}

        leafnode(const dataType& __data):
            data(__VAR(__data))
        {}

        leafnode(const Inf0<dataType>& __inf0):
            data(__VAR(__inf0))
        {}

        leafnode(const Inf1<dataType>& __inf1):
            data(__VAR(__inf1))
        {}

        bool isLeaf()
        {
            return true;
        }
    };

    class alignas(__CACHE_LINE_SIZE) internalnode:entity
    {
    public:
        __VAR data;
        shared_ptr_wrapper<entity> left;
        shared_ptr_wrapper<entity> right;
        shared_ptr_wrapper<updateflag> update;

        internalnode(const internalnode& __node):
            data(__node.data),
            left(__node.left),
            right(__node.right),
            update(__node.update)
        {}

        internalnode(const __VAR>& __data = __VAR(),
            const shared_ptr_wrapper<entity>& __left = nullptr,
            const shared_ptr_wrapper<entity>& __right = nullptr,
            const shared_ptr_wrapper<updateflag>& __update =
                std::make_shared<updateflag>()):
            data(__data),
            left(__left),
            right(__right),
            update(__update)
        {}

        bool isLeaf()
        {
            return false;
        }
    };
   
    class entityinfo
    {};

    class insertinfo:entityinfo
    {
        shared_ptr_wrapper<internalnode> parent;
        shared_ptr_wrapper<leafnode> leaf;
        shared_ptr_wrapper<internalnode> newinternal;

        insertinfo(const insertinfo& __iinfo):
            parent(__iinfo.parent),
            leaf(__iiinfo.leaf),
            newinternal(__iinfo.newinternal)
        {}

        insertinfo(const shared_ptr_wrapper<internalnode>& __parent = nullptr,
            const shared_ptr_wrapper<leafnode> __leaf = nullptr,
            const shared_ptr_wrapper<internalnode> __newinternal = nullptr):
            parent(__parent),
            leaf(__leaf),
            newinternal(__newinternal)
        {}
    };

    class alignas(__CACHE_LINE_SIZE) deleteinfo:entityinfo
    {
        shared_ptr_wrapper<internalnode> grandpa;
        shared_ptr_wrapper<internalnode> parent;
        shared_ptr_wrapper<leafnode> leaf;
        shared_ptr_wrapper<updateflag> pupdate;

        deleteinfo(const deleteinfo& __delinfo):
            grandpa(__delinfo.grandpa),
            parent(__delinfo.parent),
            leaf(__delinfo.leaf),
            pupdate(__delinfo.pupdate)
        {}
    }

    class alignas(__CACHE_LINE_SIZE) searchresult
    {
    public:
        shared_ptr_wrapper<entity> grandpa;
        shared_ptr_wrapper<entity> parent;
        shared_ptr_wrapper<entity> leaf;
        shared_ptr_wrapper<updateflag> pupdate;
        shared_ptr_wrapper<updateflag> gpupdate;

        searchresult(const searchresult& __search):
            grandpa(__search.grandpa),
            parent(__search.parent),
            leaf(__search.leaf),
            pupdate(__search.pupdate),
            gpupdate(__search.gpupdate)
        {}

        searchresult(const shared_ptr_wrapper<entity>& __grandpa = nullptr,
            const shared_ptr_wrapper<entity>& __parent = nullptr,
            const shared_ptr_wrapper<entity>& __leaf = nullptr,
            const shared_ptr_wrapper<updateflag>& __pupdate =
                std::make_shared<updateflag>(),
            const shared_ptr_wrapper<updateflag>& __gpupdate =
                std::make_shared<updateflag>()):
            grandpa(__grandpa),
            parent(__parent),
            leaf(__leaf),
            pupdate(__pupdate),
            gpupdate(__gpupdate)
        {}
    };

    shared_ptr_wrapper<entity> root;

public:
    bstree()
    {
        shared_ptr_wrapper<leafnode> rleft =
            std::make_shared<leafnode>(Inf0<dataType>());
        shared_ptr_wrapper<leafnode> rright =
            std::make_shared<leafnode>(Inf1<dataType>());
        root = std::make_shared<internalnode>(Inf1<dataType>(),  rleft, rright);
    }

    shared_ptr_wrapper<searchresult> search(const dataType& __data)
    {
        shared_ptr_wrapper<internalnode> grandpa;
        shared_ptr_wrapper<internalnode> parent;
        shared_ptr_wrapper<entity> leaf;
        shared_ptr_wrapper<updateflag> pupdate;
        shared_ptr_wrapper<updateflag> gpupdate;

        leaf = root;
        do {
            grandpa = parent;
            parent = leaf;
            gpupdate = pupdate;
            pupdate = parent->node.update;

            if (__lessexp<dataType>()(__data, leaf->data))
                leaf = parent->node.left;
            else
                leaf = parent->node.right;
        } while (!leaf->isLeaf());

        return std::make_shared<searchresult>(
            grandpa, parent, self, pupdate, gpupdate);
    }

    shared_pre_wrapper<leafnode> find(const dataType& __data)
    {
        shared_ptr_wrapper<leafnode> __leaf = search(__data)->leaf;
        if (__equalexp<dataType>()(__data, leaf->data))
            return __search->leaf;
        else
            return nullptr;
    }

    bool insert(const dataType& __data)
    {
        shared_ptr_wrapper<internalnode> parent;
        shared_ptr_wrapper<internalnode> newinternal;

        shared_ptr_wrapper<leafnode> leaf;
        shared_ptr_wrapper<leafnode> newsibling;

        shared_ptr_wrapper<insertinfo> insertrecord;
        
        shared_ptr_wrapper<updateflag> pupdate;
        shared_ptr_wrapper<updateflag> result;

        shared_ptr_wrapper<leafnode> newleaf =
            std::make_shared<leafnode>(__data);


        while (true) {
            shared_ptr_wrapper<searchresult> __search = search(__data);
            parent = __search->parent;
            leaf = __search->leaf;
            pupdate = __search->pupdate;

            if (__equalexp<dataType>()(leaf->data, __data))
                return false;
            if (!pupdate->isClean())
                help(pupdate);
            else {
                newsibling = std::make_shared<leafnode>(__leaf->data);
                if (__lessexp<dataType>()(newleaf->data, newsibling->data))
                    newinternal = std::make_shared<internalnode>(
                        std::max(__VAR(__data), leaf->data,
                            __lessexp<dataType>()),
                        newleaf, newsibling);
                else
                    newinternal = std::make_shared<internalnode>(
                        std::max(__VAR(__data), leaf->data,
                            __lessexp<dataType>()),
                        newsibling, newleaf);

                insertrecord =
                    std::make_shared<insertinfo>(parent, leaf, newinternal);

                result = std::make_shared<updateflag>(0x100, insertrecord);

                if (parent->update.cas_strong(pupdate, result)) {
                    helpinsert(insertrecord);
                    return true;
                } else
                    help(result);
            }
        }
    }

    void helpinsert(const shared_ptr_wrapper<insertinfo>& __insertinfo)
    {
        shared_ptr_wrapper<insertinfo> __insertrecord = __insertinfo;
        shared_ptr_wrapper<updateflag> __iflag;
        shared_ptr_wrapper<updateflag> __cleanflag;
        
        do {
            __iflag = std::make_shared<updateflag>(0x100, __insertrecord);
            __cleanflag = std::make_shared<updateflag>(0x1, __insertrecord);
            cas_child(__insertrecord->parent,
                __insertrecord->leaf,
                __insertrecord->newinternal);
        } while (__insertrecord->parent->update.cas_weak(__iflag, __cleanflag));
    }

    bool remove(const dataType& __data)
    {
        shared_ptr_wrapper<internalnode> grandpa;
        shared_ptr_wrapper<internalnode> parent;
        shared_ptr_wrapper<leafnode> leaf;

        shared_ptr_wrapper<updateflag> pupdate;
        shared_ptr_wrapper<updateflag> gpupdate;
        shared_ptr_wrapper<updateflag> result;

        shared_ptr_wrapper<deleteinfo> deleterecord;

        while (true) {
            shared_ptr_wrapper<searchresult> __search = search(__data);
            grandpa = __search->grandpa;
            parent = __search->parent;
            leaf = __search->leaf;
            pupdate = __search->pupdate;
            gpupdate = __search->gpupdate;

            if (!__equalexp<dataType>()(leaf->data, __data))
                return false;
            if (!gpupdate->isClean())
                return help(gpupdate);
            else if (!pupdate->isClean())
                return help(pupdata);
            else {
                __deleterecord = std::make_shared<deleteinfo>(
                    grandpa, parent, leaf, pupdate);

                result = std::make_shared<updateflag>(0x10, __deleterecord);

                if (grandpa->update.cas_strong(gpupdate, result)) {
                    helpdelete(__deleterecord);
                    return true;
                } else
                    help(result);
            }
        }
    }

    bool helpdelete(shared_ptr_wrapper<deleteinfo>& __deleteinfo)
    {
        shared_ptr_wrapper<deleteinfo> __deleterecord = __deleteinfo;
        shared_ptr_wrapper<updateflag> __dflag;
        shared_ptr_wrapper<updateflag> __cleanflag;

        shared_ptr_wrapper<updateflag> result;

        __deleterecord->parent->update.cas_strong()

        do {
            result = std::make_shared<updateflag>(0x1000, __deleterecord);
        } while (__deleterecord->parent->update, __deleterecord->pupdate, result);

        do {
            result = std::make_shared<updateflag>(0x10, __deleterecord);
        } while (__deleterecord->parent->update,
            __deleterecord->pupdate, result);

    }

    void helpmarked(shared_ptr_wrapper<deleteinfo> __deleteinfo)
    {
        shared_ptr_wrapper<deleteinfo> __deleterecord = __deleteinfo;
        shared_ptr_wrapper<entity>* tomarked;

        do {
            if (__equalexp<dataType>()(__deleterecord->parent->right->data,
                __deleterecord->leaf->data))
                tomarked = &(__deleterecord->parent->left);
            else
                tomarked = &(__deleterecord->parent->right);

            cas_child(__deleterecord->grandpa, __deleterecord->parent, *other);

            shared_ptr_wrapper<updateflag> __dflag =
                std::make_shared<updateflag>(0x10, __deleterecord);
            shared_ptr_wrapper<updateflag> __cleanflag =
                std::make_shared<updateflag>(0x1, __deleterecord);
        } while (__deleterecord->grandpa->update.cas_weak(__dflag, __cleanflag);
    }

    void help(const shared_ptr_wrapper<updateflag>& update)
    {
        if (update->isIflag())
            helpinsert(update->info);
        else if (update->isMark())
            helpmarked(update->info);
        else if (update->isDflag())
            helpdelete(update->info);
    }

    void cas_child(const shared_ptr_wrapper<entity>& parent,
        const shared_ptr_wrapper<entity>& oldentity,
        const shared_ptr_wrapper<entity>& newentity)
    {
        shared_ptr_wrapper<entity> __parent = nullptr;
        shared_ptr_wrapper<entity> __old = nullptr;
        shared_ptr_wrapper<entity> __new = nullptr;
        shared_ptr_wrapper<entity>* tochange = nullptr;

        do {
            __parent = parent;
            __old = oldentity;
            __new = newentity;

            if (__lessexp<dataType>()(__new->data, __parent->data))
                tochange = &(__parent->left);
            else
                tochange = &(__parent->right);
        } while (!tochange->cas_weak(__old, __new));
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
    std::cout << bst.insert(7) << std::endl;
    std::cout << *reinterpret_cast<int*>(&(bst.lookup(6)->self->data)) << std::endl;
    return 0;
}
