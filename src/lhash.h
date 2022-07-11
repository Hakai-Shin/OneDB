
/**
 * @brief The TableIndex acts like an index of tables existing in the
 * system. Everytime a index is added(removed) to(from) the system, it needs to
 * be added(removed) to(from) the tableIndex. 
 *
 */
class LinHash
{
    int bucketSize,numRecords,numBits,numBuckets;
    vector< map< int,set<int> > > buckets; 
public:
    LinHash(int numBuckets,int bucketSize);
    unsigned int hashfun(int key);
    int getBucketIndex(int key);
    int occupancy();
    void insert2bucket(int key,int pageId);
    void insert(int key,int pageIndex);
    set<int> getPages(int key);
    void updateIndex(int key, set<int> updatedPageSet);
    ~LinHash();
};