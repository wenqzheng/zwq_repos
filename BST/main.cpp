/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: devesh
 *
 * Created on 14 April, 2016, 12:53 AM
 */

#include <cassert>
#include <iostream>
#include <string>
#include <thread>
#include "BSTclass.h"
using namespace std;

/*
 * 
 */

void testSearch(BST *b, int val, bool expected)
{
  cout << "  Looking for " << val << "." << endl;
  cout << "  (Expected to " << (expected ? "" : "not ") << "be in the tree.)" << endl;
  searchResult * res = b->search(val);
  assert((res->l.load()->data == val) == expected);
  cout << "  " << val << (expected ? " found." : " not found.") << endl << endl;
}

void populateSeqTree(SequentialBST *b)
{
  int vals[6] = {6, 10, 7, 9, 14, 3};
  for (int i = 0; i < 6; i++) {
    b->insert(vals[i]);
    cout << "  Inserted " << vals[i] << endl;
  }
}

void populateNBTree(NonBlockingBST *b)
{
  int vals[6] = {6, 10, 7, 9, 14, 3};
  for (int i = 0; i < 6; i++) {
    b->insert(vals[i]);
    cout << "  Inserted " << vals[i] << endl;
  }
}

void testSequential()
{
  cout << "Testing sequential implementation." << endl;
  cout << "----------------------------------" << endl;
  cout << "Initializing new tree." << endl;
  SequentialBST b;
  b.print_preorder();
  testSearch(&b, INT_MAX, true);
  cout << "Populating tree." << endl;
  populateSeqTree(&b);
  b.print_preorder();
  cout << "Testing search." << endl;
  testSearch(&b, 9, true);
  testSearch(&b, 16, false);
  cout << endl;
}

void testNonblocking()
{
  cout << "Testing nonblocking implementation." << endl;
  cout << "-----------------------------------" << endl;
  cout << "Initializing new tree." << endl;
  NonBlockingBST b;
  b.print_preorder();
  cout << "Testing nonblocking implementation." << endl;
  testSearch(&b, INT_MAX, true);
  cout << "Populating tree." << endl;
  populateNBTree(&b);
  b.print_preorder();
  cout << "Testing search." << endl;
  testSearch(&b, 9, true);
  testSearch(&b, 16, false);
  cout << endl;
}

void threadNBTreeFunc(NonBlockingBST *b){
	int j=rand()%10+1;
	for(int i=0;i<j;i++){
		int k = rand() % 1000 + 1;
		b->insert(k);
	}
}

void test(int numthreads,int times){

	float sum = 0;
	clock_t begin_time,end_time;
	for(int i=0;i<times;i++){
		NonBlockingBST b;
		vector<thread> myThreads;
		begin_time = clock();
   		for (int i=0; i<numthreads; i++){
      			myThreads.push_back(thread(threadNBTreeFunc, &b));
   		}
   		for (int i=0; i<numthreads; i++){
			myThreads.at(i).join();
    		}
   		end_time = clock();
    		sum += float(end_time - begin_time);
	}
	sum /= times;
	cout<<"Time taken with "<<numthreads<<" threads is "<<sum<<endl; 
}

int main(int argc, char* argv[]) {
  if (argc == 2) {
    string s("test");
    string t(argv[1]);
    if (s != t) {
      cout << "Usage: ./BST test" << endl
           << "   or  ./BST time <numThreads> <Times>" << endl;
      return 1;
    }
    // Run BST test
    testSequential();
    testNonblocking();
  }
  else if (argc == 4) {
    string s("time");
    string t(argv[1]);
    if (s != t) {
      cout << "Usage: ./BST test" << endl
           << "   or  ./BST time <numThreads> <Times>" << endl;
      return 1;
    }
    int numthreads = atoi(argv[2]);
    int times = atoi(argv[3]);
    test(numthreads,times);

  }
  else {
    cout << "Usage: ./BST test" << endl
         << "   or  ./BST time <numThreads> <Times>" << endl;
    return 1;
  }
  return 0;
}

