#include <iostream>
#include <climits>
#include <vector>
#include <thread>

using namespace std;

const int inf1 = INT_MAX-1;
const int inf2 = INT_MAX;
class BST
{
    public:
        struct tree_node
        {
           tree_node* left;
           tree_node* right;
           bool isLeaf;
           int data;
        };
        tree_node* root;
            
        BST()
        {
           root = NULL;
        }
       
        void print_preorder();
        void preorder(tree_node*);
        vector<tree_node*> search(int);
	void insert(int);
        void initializetree();
};

typedef BST::tree_node* tnp;

void BST::initializetree(){
 
    tree_node* t = new tree_node;
    tree_node* rootleft = new tree_node;
    tree_node* rootright = new tree_node;
    
    rootleft->data = inf1;
    rootleft->left = NULL;
    rootleft->right = NULL;
    rootleft->isLeaf = true;
    
    rootright->data = inf2;
    rootright->left = NULL;
    rootright->right = NULL;
    rootright->isLeaf = true;
    
    t->data = inf2;
    t->left = rootleft;
    t->right = rootright;
    t->isLeaf = false;
   
    root = t;
}


vector<tnp> __attribute__((transaction_pure)) BST::search(int k)
{
    vector<tnp> vec;
    tnp gp = NULL;
    tnp p = NULL;
    tnp l = root;
    __transaction_atomic{
    	do{
       		gp = p;
        	p = l;
        	if (k<l->data){
            		l = p->left;
        	}
        	else{
           		l = p->right;
		}
        
    	}while(l->isLeaf != true); 
    
    	vec.push_back(gp);
    	vec.push_back(p);
    	vec.push_back(l);
    }
    return vec;
}

void __attribute__((transaction_pure)) BST::insert(int k)
{
	 __transaction_atomic{
		vector<tnp> vec = search(k);
		tnp newinternal = vec[2];
		if(newinternal->data == k){
		    return;
		}
		tnp newnode = new tree_node;
		newnode->data = k;
		newnode->left = NULL;
		newnode->right = NULL;
		newnode->isLeaf = true;
		
		tnp newsibling = new tree_node;
		newsibling->data = newinternal->data;
		newsibling->left = NULL;
		newsibling->right = NULL;
		newsibling->isLeaf = true;
		
		newinternal->data = max(k,newinternal->data);
		newinternal->isLeaf = false;
		if(k<newinternal->data){
		    newinternal->left = newnode;
		    newinternal->right = newsibling;
		}
		else{
		    newinternal->left = newsibling;
		    newinternal->right = newnode;
		}
	}	
}

void __attribute__((transaction_pure)) BST::print_preorder()
{
    preorder(root);
}

void __attribute__((transaction_pure)) BST::preorder(tree_node* p)
{
    if(p != NULL)
    {
     //	if(p->isLeaf){
		cout<<" "<<p->data<<" ";
	//}	
        if(p->left) preorder(p->left);
        if(p->right) preorder(p->right);
    }
    else return;
}

void exec(BST b){
	int j=10;//rand()%10+1;
	for(int i=0;i<j;i++){
		int k = rand() % 1000 + 1;
		b.insert(k);
	}
}

void testSearch(BST *b, int val, bool expected)
{
  cout << "  Looking for " << val << "." << endl;
  cout << "  (Expected to " << (expected ? "" : "not ") << "be in the tree.)" << endl;
  vector<tnp> vec = b->search(val);
  tnp newinternal = vec[2];
  if(newinternal->data == val){
	  cout << "  " << val << (expected ? " found." : " not found.") << endl << endl;
  }
  
}

void populateSTMTree(BST *b)
{
  int vals[6] = {6, 10, 7, 9, 14, 3};
  for (int i = 0; i < 6; i++) {
    b->insert(vals[i]);
    cout << "  Inserted " << vals[i] << endl;
  }
}

void testSTM()
{
  cout << "Testing STM implementation." << endl;
  cout << "-----------------------------------" << endl;
  cout << "Initializing new tree." << endl;
  BST b;
  b.initializetree();
  b.print_preorder();
  cout << "Testing nonblocking implementation." << endl;
  testSearch(&b, INT_MAX, true);
  cout << "Populating tree." << endl;
  populateSTMTree(&b);
  b.print_preorder();
  cout << "Testing search." << endl;
  testSearch(&b, 9, true);
  testSearch(&b, 16, false);
  cout << endl;
}


int test(int numthreads,int times){

	float sum = 0;
	clock_t begin_time,end_time;
	for(int i=0;i<times;i++){
		BST b;
		b.initializetree();
		vector<thread> myThreads;
		begin_time = clock();
   		for (int i=0; i<numthreads; i++){
      			myThreads.push_back(thread(exec, b));
   		}
   		for (int i=0; i<numthreads; i++){
			myThreads[i].join();
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
      cout << "Usage: ./STMbst test" << endl
           << "   or  ./STMbst time <numThreads> <Times>" << endl;
      return 1;
    }
    // Run BST STM test
    testSTM();
 
  }
  else if (argc == 4) {
    string s("time");
    string t(argv[1]);
    if (s != t) {
      cout << "Usage: ./STMbst test" << endl
           << "   or  ./STMbst time <numThreads> <Times>" << endl;
      return 1;
    }
    int numthreads = atoi(argv[2]);
    int times = atoi(argv[3]);
    test(numthreads,times);

  }
  else {
    cout << "Usage: ./STMbst test" << endl
         << "   or  ./STMbst time <numThreads> <Times>" << endl;
    return 1;
  }
  return 0;
	
}




