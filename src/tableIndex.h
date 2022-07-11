#include "bptree.h"
#include "lhash.h"

enum IndexingStrategy
{
    BTREE,
    HASH,
    NOTHING
};
enum SortingStrategy
{
    ASC,
    DESC,
    NO_SORT_CLAUSE
};
/**
 * @brief The TableIndex acts like an index of tables existing in the
 * system. Everytime a index is added(removed) to(from) the system, it needs to
 * be added(removed) to(from) the tableIndex. 
 *
 */
class TableIndex
{
    BPTreeNode* bpTreeRoot;
    LinHash* linHashTable;

public:
    TableIndex(IndexingStrategy indexingStrategy,string indexingStratergyInput,int pageSize);
    void insertKey(int key,int pageIndex,IndexingStrategy indexingStrategy);
    set<int> getTargetPages(int key,IndexingStrategy indexingStrategy);
    void updateTableIndex(int key,set<int> updatedPageSet,IndexingStrategy indexingStrategy);
    //~TableIndex();
};
