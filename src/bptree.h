

/**
 * @brief The TableIndex acts like an index of tables existing in the
 * system. Everytime a index is added(removed) to(from) the system, it needs to
 * be added(removed) to(from) the tableIndex. 
 *
 */
class BPTreeNode
{
    int degree;
    int keycount;
    vector<int> keys;
    vector<int> blockPointers;
    bool isleaf;
    vector<BPTreeNode*> child;
    BPTreeNode* nextNode;
public:
    BPTreeNode(int fanOut);
    void insert(int key,int recordIndex);
    ~BPTreeNode();
};