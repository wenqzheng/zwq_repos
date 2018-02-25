
//Binary Search Tree Program

#include <iostream>
#include <cstdlib>
#include <climits>
#include <vector>
#include <algorithm>
#include "BSTclass.h"
using namespace std;

void SequentialBST::insert(int k)
{
  searchResult * s = search(k);
  atomic<treeNode*> newInternal;
  newInternal.store(s->l);
  if(newInternal.load()->data == k){
    return;
  }
  treeNode * newNode = new treeNode(k, true);
  treeNode * newSibling = new treeNode(newInternal.load()->data, true);

  newInternal.load()->data = max(k, newInternal.load()->data);
  newInternal.load()->isLeaf = false;
  if(k < newInternal.load()->data){
    newInternal.load()->left.store(newNode);
    newInternal.load()->right.store(newSibling);
  }
  else{
    newInternal.load()->left.store(newSibling);
    newInternal.load()->right.store(newNode);
  }
}

