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

#include <atomic>
#include <climits>
#include <vector>

const int inf1 = INT_MAX-1;
const int inf2 = INT_MAX;

using namespace std;

class BST
{
  public:
    struct infoRecord;
    struct updateRecord {
      bool isDirty;
      infoRecord *info;
    };
    struct searchResult;

    struct treeNode
    {
      int data;
      bool isLeaf;
      atomic<treeNode*> left;
      atomic<treeNode*> right;
      atomic<updateRecord> update;

      treeNode(int d, bool b, treeNode *l = nullptr, treeNode *r = nullptr,
               updateRecord *u = NULL)
      : data(d), isLeaf(b)
      {
        left.store(l);
        right.store(r);
        if (u)
          update.store(*u);
        else {
          updateRecord emptyUR = {false, nullptr};
          update.store(emptyUR);
        }
      }
    };

    atomic<treeNode*> root;

    BST() {
      treeNode * rleft  = new treeNode(inf1, true);
      treeNode * rright = new treeNode(inf2, true);
      root.store(new treeNode(inf2, false, rleft, rright));
    };

    void print_preorder();
    void preorder(treeNode *);
    struct searchResult * search(int);

    struct infoRecord {
      atomic<treeNode*> parent;
      atomic<treeNode*> leaf;
      atomic<treeNode*> subtree;

      infoRecord(treeNode *p = nullptr, treeNode *l = nullptr, treeNode *s = nullptr)
      {
        parent.store(p);
        leaf.store(l);
        subtree.store(s);
      }
    };
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

