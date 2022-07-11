#include "global.h"

BPTreeNode::BPTreeNode(int fanOut)
{
    logger.log("BPTreeNode::BPTreeNode");
    this->degree = fanOut;
    this->isleaf = true;
    this->keycount = 0;
}

void BPTreeNode::insert(int key,int recordIndex)
{
    return;
}
/*
    std::cout<<"key:"<<key<<" record:"<<recordIndex<<endl;
    logger.log("BPTreeNode::insert");
     //insertion begins from leaf
    
    
    if(this!=NULL)// If tree is not empty 
    { 
        // If root is full, then tree grows in height 
        if (this->keycount == 2*(this->degree) - 1) 
        { 
            // we allocate memory for a new root node 
            BPTreeNode* newroot = new BPTreeNode(this->degree);

            // we make old root as child of new root 
            newroot->child.push_back(this); 

            //we make newroot node non leaf
            newroot->isleaf=false; 
  
            // Split the old root and move 1 key to the new root 
            newroot->splitChild(0, this); 
  
            // New root has two children now.  Decide which of the 
            // two children is going to have new key 
            if (newroot->keys[0] >= key) 
                newroot->child[0]->insert(key);
                 
            else
                newroot->child[1]->insert(value);
  
            // Change root by passing this new root adress to the object in 
            return newroot; 
        } 
        else  // If root is not full, call insertNonFull for root 
           {  
                // Initialize index as index of rightmost element 
                int i=0,temp; 
            
                  // If this is a leaf node 
                if (isleaf == true) 
                { 
                    // The following loop does two things 
                    // a) Finds the location of new key to be inserted 
                    // b) Moves all greater keys to one place ahead 
                    keys[keycount]=keys[keycount-1];
                    keycount=keycount+1;
                    for(i=0;keys[i]<value&&i<keycount-1;)
                        i++;
                    for(;i<keycount;i++)
                    {
                        temp=keys[i];
                        keys[i]=value;
                        value=temp;
                    }
                    //done
                }
                else // If this node is not leaf 
                { 
                    // Find the child which is going to have the new key 
                   
                    for(i=0;i <= keycount-1 && keys[i] < value;)
                        i++;
            
                    // See if the found child is full 
                    if (child[i]->keycount == 2*degree-1) 
                    { 
                        // If the child is full, then split it 
                        splitChild(i, child[i]); 
            
                        // After split, the middle key of C[i] goes up and 
                        // C[i] is splitted into two.  See which of the two 
                        // is going to have the new key 
                        if (keys[i] <= value) 
                          child[i+1]->insert(value); 
                        else 
                           child[i]->insert(value);  
                    } 
                    else
                    {
                            if (keys[i-1] <= value) 
                          child[i]->insert(value); 
                        else 
                           child[i-1]->insert(value);  
                    }
                    
                    
                } 
                return this;
        }  
    } 
     

}

BPTreeNode* btreenode :: insert(int value) 
{ 

   
} 
   


   
  
// A utility function to split the child y of this node 
// Note that oldroot must be full when this function is called 
void btreenode::splitChild(int i, btreenode *oldroot) 
{ 
    int innerloop,childloop;

    // Create a new node which is going to store (degree-1) keys 
    // of oldroot 
    btreenode *newchildnode = new btreenode(); 

    newchildnode->isleaf= oldroot->isleaf;

    newchildnode->keycount = degree - 1; 

    //to split a full node,we copy half of the (key,pointer)pair in full node into new node
    // Reduce the number of keys in oldroot 
    oldroot->keycount = degree - 1; 
  
    //  
    if (oldroot->isleaf != true) 
    { 
            childloop=degree-1;
            while(childloop>=0)
            {
                innerloop=childloop+degree;
                newchildnode->child[childloop] = oldroot->child[innerloop];
                childloop--;
            }
    } 

            childloop=degree-2;
            while(childloop>=0)
            {
                innerloop=childloop+degree;
                newchildnode->keys[childloop] = oldroot->keys[innerloop];
                childloop--;
             }


    // A key of oldroot will move to this node. Find the location of 
    // new key and move all greater keys one space ahead 
    childloop=i;
    while(childloop<=(keycount-1))
    {
         keys[childloop+1] = keys[childloop]; 
        childloop++;
    }

       // Since this invoking node is going to have a new child, 
    // create space of new child 

    childloop=i+1;
    while(childloop<=keycount)
    {
        child[childloop+1] = child[childloop]; 
        childloop++;
    }
  
    // Increment count of keys in this node 
    keycount = keycount + 1;

    // Copy the middle key of y to this node 
    keys[i] = oldroot->keys[degree-1]; 
  
     // Link the new child to this node 
    child[i+1] = newchildnode;  
} 






//---------------------------------------------------------------------------

//btree implementation
//keysearch is done
//main program is done
//binarysearch is done
//nodeclass is done
//
//insert is undone
//traversal is undone
//splitchild is undone


int degree;
class btreenode
{
int keycount;
int *keys;
bool isleaf;
btreenode **child;
public:
btreenode()
{   
    keycount=0;
    keys=new int[2*degree-1];
    child=new btreenode* [2*degree];
    isleaf=true;
}
void maketree(int val)
{

        this->keycount = 1;
        this->keys[0] = val;  
                  
}
int binarysearch(int *keyset,int skey,int left,int right);

//insert value in btree
btreenode* insert(int value);
void splitChild(int i, btreenode *y); 
//search key in btree
btreenode* keySearch(int searchkey)
{ 
    if(this==NULL)
      return NULL;
    int status = binarysearch(keys,searchkey,0,keycount);  
    
    if(status<keycount)
    {
    if(keys[status]==searchkey) 
        return this;
    else if (isleaf == true) 
        return NULL; 
    else 
        return child[status]->keySearch(searchkey); 
    } 
    else if (isleaf == true) 
        return NULL; 
    else 
        return child[status]->keySearch(searchkey); 
}

 void btreetraverse() 
{ 
    /* traversal starts from root, root has ndegree child and nkeycount keys
    we traverse through ndegree child printing keys inside them
    
    int keyloop=0; 
    while(keyloop < this->keycount) 
    { 
        // If this is not leaf, then before printing keys[keyloop], 
        // traverse the subtree rooted with childnode child[keyloop]. 
        if (this->isleaf != true) 
           this->child[keyloop]->btreetraverse(); 
        cout << " " << this->keys[keyloop]; 
     keyloop++;
    } 
  
    // we go to last leaf and print keys in it
    if (this->isleaf == false) 
        this->child[keyloop]->btreetraverse();
}

};



int btreenode :: binarysearch(int *keyset,int searchkey,int l,int r)
{
    int mid;
    while (r > l) 
    { 
        mid = (l+r)/2; 
        if (keyset[mid] == searchkey) 
            return mid; 
        else if (keyset[mid] > searchkey) 
            r=mid;
        else if(keyset[mid] < searchkey)
            l=mid+1;
    } 
    return l;
}






int main() 
{ 
    int numQueries,choice,value;
    cout<<"Enter degree of btree:";
    cin >>degree;

    btreenode* root=NULL;

    cout<<"\nEnter number of queries:";
    cin>>numQueries;
    while(numQueries-->0)
    {
        cout<<"\nEnter choice:";
        cin>>choice;
        
        if(choice==1)
        {
            cout<<"\nEnter key to be inserted:";
            cin>>value;
            if(root==NULL)
            {
                btreenode t;
                t.maketree(value);
                root=&t;
            }
            else
             root=root->insert(value); 
             cout << "Traversal of the constucted tree is "; 
            root->btreetraverse();
        }
        else if(choice==2)
        {
            cout<<"\nEnter key to be searched:";
            cin>>value;
            if(root->keySearch(value) != NULL)
             cout << value <<" is present in B Tree\n";
            else
              cout << value <<" is not present in B Tree\n";
        }
        else if(choice==3)
            {
                //cout<<"\nEnter key to be deleted:";
                //cin>>value;
                //t.delete(value);
                //cout << "Traversal of the constucted tree is "; 
                //t.btreetraverse();
            }
        else if(choice==4)
        {
            cout << "Traversal of the constucted tree is "; 
            root->btreetraverse(); 
            cout<<endl;
        }

    }
  
    return 0; 
} 

*/

