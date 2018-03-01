// bstree.hpp
// ----by wenqzheng
//-----------------------------------------------------------------------------

#include "../utility.hpp"
#include "../utility/smart_ptr_wrapper.hpp"
#include <utility>
#include <algorithm>
#include <optional>
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
    class entityinfo;
    class entity;

    class updateflag
    {
    public:
        std::atomic_bool cflag;
        std::atomic_bool iflag;
        std::atomic_bool dflag;
        std::atomic_bool mflag;
        shared_ptr_wrapper<entityinfo> info;

        updateflag(const updateflag& __record):
            cflag(__record.cflag.load()),
            iflag(__record.iflag.load()),
            dflag(__record.dflag.load()),
            mflag(__record.mflag.load()),
            info(__record.info)
        {}

        updateflag(std::atomic_bool __cflag = true,
            std::atomic_bool __iflag = false,
            std::atomic_bool __dflag = false,
            std::atomic_bool __mflag = false,
            const shared_ptr_wrapper<entityinfo>& __info = nullptr):
            cflag(__cflag.load()),
            iflag(__iflag.load()),
            dflag(__dflag.load()),
            mflag(__mflag.load()),
            info(__info)
        {}
    };

    class entity
    {
    public:
	virtual bool isLeaf()
	{
	    return false;
	}
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

    class internalnode:entity
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

        internalnode(const __VAR& __data = __VAR(),
            const shared_ptr_wrapper<entity>& __left = nullptr,
            const shared_ptr_wrapper<entity>& __right = nullptr,
            const shared_ptr_wrapper<updateflag>& __update =
                std::make_shared<updateflag>()):
            data(__data),
            left(__left),
            right(__right),
            update(__update)
        {}
    };
   
    class entityinfo
    {};

    class insertinfo:entityinfo
    {
    public:
        shared_ptr_wrapper<internalnode> parent;
        shared_ptr_wrapper<leafnode> leaf;
        shared_ptr_wrapper<internalnode> newinternal;

        insertinfo(const insertinfo& __iinfo):
            parent(__iinfo.parent),
            leaf(__iinfo.leaf),
            newinternal(__iinfo.newinternal)
        {}

        insertinfo(const shared_ptr_wrapper<internalnode>& __parent = nullptr,
            const shared_ptr_wrapper<leafnode>& __leaf = nullptr,
            const shared_ptr_wrapper<internalnode>& __newinternal = nullptr):
            parent(__parent),
            leaf(__leaf),
            newinternal(__newinternal)
        {}
    };

    class deleteinfo:entityinfo
    {
    public:
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

        deleteinfo(const shared_ptr_wrapper<internalnode>& __grandpa = nullptr,
            const shared_ptr_wrapper<internalnode>& __parent = nullptr,
            const shared_ptr_wrapper<leafnode>& __leaf = nullptr,
            const shared_ptr_wrapper<updateflag>& __pupdate =
                std::make_shared<updateflag>()):
            grandpa(__grandpa),
            parent(__parent),
            leaf(__leaf),
            pupdate(__pupdate)
        {}
    };

    class searchresult
    {
    public:
        shared_ptr_wrapper<internalnode> grandpa;
        shared_ptr_wrapper<internalnode> parent;
        shared_ptr_wrapper<leafnode> leaf;
        shared_ptr_wrapper<updateflag> pupdate;
        shared_ptr_wrapper<updateflag> gpupdate;

        searchresult(const searchresult& __search):
            grandpa(__search.grandpa),
            parent(__search.parent),
            leaf(__search.leaf),
            pupdate(__search.pupdate),
            gpupdate(__search.gpupdate)
        {}

        searchresult(const shared_ptr_wrapper<internalnode>& __grandpa = nullptr,
            const shared_ptr_wrapper<internalnode>& __parent = nullptr,
            const shared_ptr_wrapper<leafnode>& __leaf = nullptr,
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
        root = std::make_shared<internalnode>(Inf1<dataType>(), rleft, rright);
    }

    shared_ptr_wrapper<searchresult> search(const dataType& __data)
    {
        shared_ptr_wrapper<internalnode> grandpa = nullptr;
        shared_ptr_wrapper<internalnode> parent = nullptr;
        shared_ptr_wrapper<leafnode> leaf = root;
        shared_ptr_wrapper<updateflag> pupdate = nullptr;
        shared_ptr_wrapper<updateflag> gpupdate = nullptr;

        do {
            grandpa = parent;
            parent = leaf;
            gpupdate = pupdate;
            pupdate = parent->update;

            if (__lessexp<dataType>()(__data, leaf->data))
                leaf = parent->left;
            else
                leaf = parent->right;
        } while (!leaf->isLeaf());

        return std::make_shared<searchresult>(
            grandpa, parent, leaf, pupdate, gpupdate);
    }

    shared_ptr_wrapper<leafnode> find(const dataType& __data)
    {
        shared_ptr_wrapper<leafnode> __leaf = search(__data)->leaf;
        if (__equalexp<dataType>()(__data, __leaf->data))
            return __leaf;
        else
            return nullptr;
    }

    bool insert(const dataType& __data)
    {
        shared_ptr_wrapper<internalnode> parent;
        shared_ptr_wrapper<internalnode> newinternal;
        shared_ptr_wrapper<leafnode> leaf;
        shared_ptr_wrapper<leafnode> newsibling;
        shared_ptr_wrapper<updateflag> pupdate;

        shared_ptr_wrapper<insertinfo> __iinfo;
		shared_ptr_wrapper<updateflag> __iflag;
        shared_ptr_wrapper<leafnode> newleaf =
            std::make_shared<leafnode>(__data);


        while (true) {
            shared_ptr_wrapper<searchresult> __search = search(__data);
            parent = __search->parent;
            leaf = __search->leaf;
            pupdate = __search->pupdate;

            if (__equalexp<dataType>()(leaf->data, __data))
                return false;
            if (!pupdate->cflag)
                help(pupdate);
            else {
                newsibling = std::make_shared<leafnode>(leaf->data);
                if (__lessexp<dataType>()(newleaf->data, newsibling->data))
                    newinternal = std::make_shared<internalnode>(
                        leaf->data, newleaf, newsibling);
                else
                    newinternal = std::make_shared<internalnode>(
                        __data, newsibling, newleaf);

                __iinfo =
                    std::make_shared<insertinfo>(parent, leaf, newinternal);

		        __iflag = std::make_shared<updateflag>(
                    false, true, false, false, __iinfo);
                
                if (parent->update.cas_strong(pupdate, __iflag)) {
                    helpinsert(__iinfo);
                    return true;
                } else
                    help(pupdate);
            }
        }
    }

    bool remove(const dataType& __data)
    {
        shared_ptr_wrapper<internalnode> grandpa;
        shared_ptr_wrapper<internalnode> parent;
        shared_ptr_wrapper<leafnode> leaf;
        shared_ptr_wrapper<updateflag> pupdate;
        shared_ptr_wrapper<updateflag> gpupdate;

		shared_ptr_wrapper<updateflag> __dflag;
        shared_ptr_wrapper<deleteinfo> __dinfo;

        while (true) {
            shared_ptr_wrapper<searchresult> __search = search(__data);
            grandpa = __search->grandpa;
            parent = __search->parent;
            leaf = __search->leaf;
            pupdate = __search->pupdate;
            gpupdate = __search->gpupdate;

            if (!__equalexp<dataType>()(leaf->data, __data))
                return false;
            if (!gpupdate->cflag)
                help(gpupdate);
            else if (!pupdate->cflag)
                help(pupdate);
            else {
                __dinfo = std::make_shared<deleteinfo>(
                    grandpa, parent, leaf, pupdate);

		        __dflag = std::make_shared<updateflag>(
                    false, false, true, false, __dinfo);

                if (grandpa->update.cas_strong(gpupdate, __dflag)) {
                    if (helpdelete(__dinfo))
                        return true;
                } else
                    help(gpupdate);
            }
        }
    }

    void helpinsert(shared_ptr_wrapper<insertinfo> __iinfo)
    {
        shared_ptr_wrapper<insertinfo> __irecord = __iinfo.load();
        cas_child(__irecord->parent, __irecord->leaf, __irecord->newinternal);
	    shared_ptr_wrapper<updateflag> __iflag =
	        std::make_shared<updateflag>(false, true, false, false, __irecord);
	    shared_ptr_wrapper<updateflag> __cflag =
	        std::make_shared<updateflag>(true, false,false, false, __irecord);
        while (!__irecord->parent->update.cas_weak(__iflag, __cflag));
    }

    inline bool helpdelete(shared_ptr_wrapper<deleteinfo> __dinfo)
    {
        shared_ptr_wrapper<deleteinfo> __drecord = __dinfo.load();
        shared_ptr_wrapper<updateflag> __mflag =
	        std::make_shared<updateflag>(false, false, false, true, __drecord);
	    shared_ptr_wrapper<updateflag> __dflag =
	        std::make_shared<updateflag>(false, false, true, false, __drecord);
        shared_ptr_wrapper<updateflag> __cflag =
	        std::make_shared<updateflag>(true, false, false, false, __drecord);

        if (__drecord->parent->update.cas_strong(__drecord->pupdate, __mflag)) {
            helpmarked(__drecord);
            return true;
        } else {
            help(__drecord->pupdate);
            while (!__drecord->grandpa->update.cas_weak(__dflag, __cflag));
            return false;
        }
    }

    inline void helpmarked(shared_ptr_wrapper<deleteinfo> __dinfo)
    {
        shared_ptr_wrapper<deleteinfo> __drecord = __dinfo.load();
        shared_ptr_wrapper<updateflag> __dflag =
            std::make_shared<updateflag>(false, false, true, false, __drecord);
        shared_ptr_wrapper<updateflag> __cflag =
            std::make_shared<updateflag>(true, false, false, false, __drecord);

        if (__drecord->parent->right->isLeaf())
            cas_child(__drecord->grandpa, __drecord->parent,
                __drecord->parent->left);
        else
            cas_child(__drecord->grandpa, __drecord->parent,
                __drecord->parent->right);

        __drecord->grandpa->update.cas_strong(__dflag, __cflag);
    }

    inline void help(shared_ptr_wrapper<updateflag> update)
    {
        shared_ptr_wrapper<updateflag> __uprecord = update.load();
        if (__uprecord->iflag)
            helpinsert(__uprecord->info);
        else if (__uprecord->mflag)
            helpmarked(__uprecord->info);
        else if (__uprecord->dflag)
            helpdelete(__uprecord->info);
    }

    inline void cas_child(shared_ptr_wrapper<internalnode> parent,
        shared_ptr_wrapper<entity> oldentity,
        shared_ptr_wrapper<entity> newentity)
    {
        shared_ptr_wrapper<internalnode> __pa = parent.load();
        shared_ptr_wrapper<leafnode> __old = oldentity.load();
        shared_ptr_wrapper<internalnode> __new = newentity.load();
        if (__lessexp<dataType>()(__new->data, __pa->data))
            __pa->left.cas_strong(__old, __new);
        else
            __pa->right.cas_strong(__old, __new);
    }
};

int main()
{
    bstree<int> bst;
    std::cout << bst.insert(4) << std::endl;
    std::cout << bst.insert(8) << std::endl;
//    bst.remove(4);

    std::cout << bst.insert(1) << std::endl;
    std::cout << bst.insert(2) << std::endl;
//    bst.insert(7);
    if (!bst.search(7)->pupdate->cflag)
        std::cout << "not clean" << std::endl;
    else
        std::cout << "clean" << std::endl;
    std::cout << bst.search(7)->pupdate->iflag << std::endl;
bst.insert(7);
bst.insert(9);
bst.remove(9);
std::cout << bst.insert(9) << std::endl;
std::cout << bst.insert(89) << std::endl;
//    std::cout << __equalexp<int>()(8,8) << std::endl;
//    std::cout << typeid(std::atomic_bool).name() << std::endl;
//    std::cout << bst.insert(9) << std::endl;
//    std::cout << bst.insert(7) << std::endl;
//    std::cout << *reinterpret_cast<int*>(&(bst.search(6)->leaf->data)) << std::endl;
    return 0;
}
