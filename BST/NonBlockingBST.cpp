/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   NonBlockingBST.cpp
 * Author: devesh
 * 
 * Created on 14 April, 2016, 12:37 AM
 */

#include <atomic>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include "BSTclass.h"

using namespace std;

bool NonBlockingBST::insert(int k)
{
  treeNode *p, *newInternal, *l, *newSibling, *newNode;
  infoRecord * op;

  newNode = new treeNode(k, true);

  while(true) {
    searchResult * s = search(k);
    p = s->p.load();
    l = s->l.load();
    updateRecord &pupdate = s->pupdate;

    if (l->data == k) return false; // data already found
    if (pupdate.isDirty)
      // help whoever else is trying to insert
      helpInsert(pupdate.info);
    else {
      newSibling = new treeNode(l->data, true);
      if (newNode->data < newSibling->data)
        newInternal = new treeNode(max(k, l->data), false, newNode, newSibling);
      else
        newInternal = new treeNode(max(k, l->data), false, newSibling, newNode);
      op = new infoRecord(p, l, newInternal);
      
      updateRecord dirty = {true, op};
      bool iflag = p->update.compare_exchange_strong(pupdate, dirty);
      if (iflag) {
        helpInsert(op);
        return true;
      }
      else
        helpInsert(pupdate.info);
    }
  }
}

void NonBlockingBST::helpInsert(infoRecord* op)
{
  treeNode *p, *l, *s;
  p = op->parent.load();
  l = op->leaf.load();
  s = op->subtree.load();

  updateRecord dirty = {true, op},
               clean = {false, op};

  CASChild(p, l, s); 
  op->parent.load()->update.
      compare_exchange_strong(dirty, clean);
}

bool NonBlockingBST::CASChild(treeNode *parent, treeNode *oldNode, treeNode *newNode)
{
  atomic<treeNode*> *childToChange;
  if (newNode->data < parent->data)
    childToChange = &(parent->left);
  else
    childToChange = &(parent->right);
  
  return childToChange->compare_exchange_strong(oldNode, newNode);
}
