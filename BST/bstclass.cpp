// bstree.hpp
// ----by wenqzheng
//-----------------------------------------------------------------------------

#include "../utility.hpp"
#include "../utility/smart_ptr_wrapper.hpp"
#include <algorithm>
#include <atomic>
#include <cstdint>
#include <climits>

constexpr std::size_t inf1 = SIZE_MAX-1;
constexpr std::size_t inf2 = SIZE_MAX;

template<typename dataType, class compare = std::less<dataType>>
class bstree
{
    class relinfo;
    class treenode;

    class alignas(2 * sizeof(shared_ptr_wrapper<void>)) updateflag
    {
    public:
        std::atomic_bool isDirty;
        shared_ptr_wrapper<relinfo> info;
    };

    class alignas(__power2(sizeof(dataType))) entity
    {
    public:
        dataType data;
        treenode node;
    
        entity(const entity& __entity):
            data(__entity.data), node(__entity.node)
        {}

        entity(entity&& __entity):
            data(__entity.data), node(__entity.node)
        {}

        entity(dataType __data, treenode __node):
            data(__data), node(__node)
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
            isLeaf(__node.isLeaf),
            left(__node.left),
            right(__node.right),
            update(__node.update)
        {}

        treenode(treenode&& __node):
            isLeaf(__node.isLeaf.load()),
            left(__node.left.load()),
            right(__node.right.load()),
            update(__node.update.load())
        {}

        treenode(std::atomic_bool __isLeaf = true,
            shared_ptr_wrapper<entity> __left = nullptr,
            shared_ptr_wrapper<entity> __right = nullptr,
            shared_ptr_wrapper<updateflag> __update = nullptr):
            isLeaf(__isLeaf.load()),
            left(__left.load()),
            right(__right.load()),
            update(__update.load())
        {}
    };

    class alignas(__CACHE_LINE_SIZE) relinfo
    {
    public:
        shared_ptr_wrapper<entity> parent;
        shared_ptr_wrapper<entity> leaf;
        shared_ptr_wrapper<entity> subtree;

        relinfo(const relinfo& __record):
            parent(__record.parent.load()),
            leaf(__record.leaf.load()),
            subtree(__record.subtree.load())
        {}
        relinfo(relinfo&& __record):
            parent(__record.parent.load()),
            leaf(__record.leaf.load()),
            subtree(__record.subtree.load())
        {}

        relinfo(shared_ptr_wrapper<entity> __parent = nullptr,
            shared_ptr_wrapper<entity> __leaf = nullptr,
            shared_ptr_wrapper<entity> __subtree = nullptr):
            parent(__parent.load()),
            leaf(__leaf.load()),
            subtree(__subtree.load())
        {}
    };

    class searchresult
    {
    public:
        shared_ptr_wrapper<entity> parent;
        shared_ptr_wrapper<entity> self;
        shared_ptr_wrapper<updateflag> pupdate;

        searchresult(const searchresult& __search):
            parent(__search.parent.load()),
            self(__search.self.load()),
            pupdate(__search.pupdate.load())
        {}
       searchresult(searchresult&& __search):
            parent(__search.parent.load()),
            self(__search.self.load()),
            pupdate(__search.pupdate.load())
        {}

        searchresult(shared_ptr_wrapper<entity> __parent = nullptr,
            shared_ptr_wrapper<entity> __self = nullptr,
            shared_ptr_wrapper<updateflag> __pupdate = NULL):
            parent(__parent.load()),
            self(__self.load()),
            pupdate(__pupdate.load())
        {}
    };

    shared_ptr_wrapper<entity> root;

public:
    bstree()
    {
        shared_ptr_wrapper<entity> rleft =
            std::make_shared<entity>(inf1, treenode(true));
        shared_ptr_wrapper<entity> rright =
            std::make_shared<entity>(inf2, treenode(true));
        root = std::make_shared<entity>(inf2, treenode(false, rleft, rright));
    }

    shared_ptr_wrapper<searchresult> lookup(dataType& __data)
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
        } while (!(self->node.isLeaf));

        return std::make_shared<searchresult>(parent, self, pupdate);
    }

    bool insert(dataType&& __data)
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
                        treenode(false, newentity, sibling));
                else
                    newinternal = std::make_shared<entity>(
                        std::max(__data, self->data, compare()),
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
            tochange = &(parent->node.left);
        else
            tochange = &(parent->node.right);
        tochange->cas_strong(leaf, subtree);

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
}
