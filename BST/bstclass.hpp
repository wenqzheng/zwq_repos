/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BSTclass.h
 * Author: devesh
 *
 * Created on 13 April, 2016, 7:37 PM
 */
#ifndef BSTCLASS_H
#define BSTCLASS_H

#include "utility.hpp"
#include "smart_ptr_wrapper.hpp"
#include <atomic>
#include <cstdint>
#include <climits>
#include <vector>

constexpr std::size_t inf1 = SIZE_MAX-1;
constexpr std::size_t inf2 = SIZE_MAX;


struct relinfo;

struct alignas(32) updateflag
{
    std::atomic_bool isDirty;
    shared_ptr_wrapper<relinfo> info;

    updateflag(std::atomic_bool __isDirty = false,
        shared_ptr_wrapper<relinfo> __info = nullptr):
        isDirty(__isDirty),
        info(__info)
    {}
};

struct treenode;

template<class dataType, class Compare = std::less<dataType>>
struct entity
{
    dataType data;
    treenode node;
}

struct alignas(__CACHE_LINE_SIZE) treenode
{
    std::atomic_bool isLeaf;
    shared_ptr_wrapper<treenode> left;
    shared_ptr_wrapper<treenode> right;
    shared_ptr_wrapper<updateflag> update;

    treenode(std::atomic_bool __isLeaf = false,
        shared_ptr_wrapper<treenode> __left = nullptr,
        shared_ptr_wrapper<treenode> __right = nullptr,
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
    shared_ptr_wrapper<treenode> parent;
    shared_ptr_wrapper<treenode> leaf;
    shared_ptr_wrapper<treenode> subtree;

    relinfo(shared_ptr_wrapper<treenode> __parent = nullptr,
        shared_ptr_wrapper<treenode> __leaf = nullptr,
        shared_ptr_wrapper<treenode> __subtree = nullptr):
        parent(parent),
        leaf(leaf),
        subtree(subtree)
    {}
};

struct searchResult;

template<class dataType, class Compare = std::less<dataType>>
struct alignas(__CACHE_LINE_SIZE) objectItem
{
    dataType data;
    treenode;
}

shared_ptr_wrapper<treenode> root;


    
class bstree
{
public:

    bstree():
        root(std::make_shared<treenode>())
    BST() {
      treeNode * rleft  = new treeNode(inf1, true);
      treeNode * rright = new treeNode(inf2, true);
      root.store(new treeNode(inf2, false, rleft, rright));
    };

    void print_preorder();
    void preorder(treeNode *);
    struct searchResult * search(int);



};

typedef struct BST::searchResult {
  atomic<treeNode *> p, l;
  updateRecord pupdate;

  searchResult(treeNode *pp, treeNode *ll, updateRecord uu)
  : pupdate(uu) 
  {
    p.store(pp);
    l.store(ll);
  }
} searchResult;

class SequentialBST: public BST
{
  public:
    void insert(int);    
};

class NonBlockingBST: public BST
{
  private:
    void helpInsert(infoRecord *);
    bool CASChild(treeNode *parent, treeNode *oldNode, treeNode *newNode);
  public:
    bool insert(int);
};
#endif /* BSTCLASS_H */

