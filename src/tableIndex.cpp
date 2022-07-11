#include "global.h"

TableIndex::TableIndex(IndexingStrategy indexingStrategy,string indexParameter,int pageSize)
{
    logger.log("TableIndex::TableIndex");

    if (indexingStrategy == BTREE)
        {
            int degree = stoi(indexParameter);
            this->bpTreeRoot = new BPTreeNode(degree);
        }
    else if (indexingStrategy == HASH)
        {
            int bucketSize = stoi(indexParameter);
            this->linHashTable = new LinHash(bucketSize,pageSize);
        }
    else if (indexingStrategy == NOTHING)
        {
           // this->index = Hash(indexingStratergyInput);
           cout<<"nothing fun not implemented";
        }
}

/**
 * @brief Function that loads Page indicated by pageIndex. Now the cursor starts
 * reading from the new page.
 *
 * @param pageIndex 
 */
void TableIndex::insertKey(int key,int pageIndex,IndexingStrategy indexingStrategy)
{
    logger.log("TableIndex::insertKey");
    if (indexingStrategy == BTREE)
        {
            this->bpTreeRoot->insert(key,pageIndex);
        }
    else if (indexingStrategy == HASH)
        {
           this->linHashTable->insert(key,pageIndex);
        }
}

set<int> TableIndex::getTargetPages(int key,IndexingStrategy indexingStrategy)
{
    logger.log("TableIndex::insertKey");
    if (indexingStrategy == BTREE)
        {
           // return this->bpTreeRoot->(key,pageId);
        }
    else if (indexingStrategy == HASH)
        {
           return this->linHashTable->getPages(key);
        }
}


void TableIndex::updateTableIndex(int key,set<int> updatedPageSet,IndexingStrategy indexingStrategy)
{
    logger.log("TableIndex::insertKey");
    if (indexingStrategy == BTREE)
        {
           // return this->bpTreeRoot->(key,pageId);
        }
    else if (indexingStrategy == HASH)
        {
           this->linHashTable->updateIndex(key,updatedPageSet);
        }
}