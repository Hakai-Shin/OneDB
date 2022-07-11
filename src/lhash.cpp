#include "global.h"

LinHash::LinHash(int numBuckets,int bucketSize)
{
    logger.log("LinHash::LinHash");
    this->bucketSize = bucketSize;
    this->numBuckets = numBuckets;
    this->numRecords = 0;
    this->numBits = ceil(log2((double)numBuckets));
    vector< map< int,set<int> > > hashTable(numBuckets); 
    this->buckets = hashTable;
}

unsigned int LinHash::hashfun(int key) 
{
        unsigned int mod = (1 << this->numBits);
        return (unsigned int)(key % mod + mod) % mod;
}

int LinHash::occupancy() 
{
        double ratio = 1.0 * this->numRecords / (this->numBuckets*this->bucketSize);
        return (int)(100 * ratio);
}

int LinHash::getBucketIndex(int key) 
{
        unsigned int hashvalue = this->hashfun(key);
        if(hashvalue >= this->buckets.size()) {
            hashvalue -= (1 << (this->numBits - 1));
        }
        return hashvalue;
}

set<int> LinHash::getPages(int key)
{
    set<int> pageSet;
    int bucketIndex = this->getBucketIndex(key);
    if( this->buckets[bucketIndex].find(key) != this->buckets[bucketIndex].end() )
    {
            return this->buckets[bucketIndex][key];
    }
    return pageSet;
}

void LinHash::updateIndex(int key, set<int> updatedPageSet)
{
    int bucketIndex = this->getBucketIndex(key);
    if(!updatedPageSet.empty())
    {
    this->buckets[bucketIndex][key].clear();
    this->buckets[bucketIndex][key] = updatedPageSet;
    }
    else
    {
      this->buckets[bucketIndex].erase(key);  
    }
    
    return;
}

void LinHash::insert2bucket(int key,int pageIndex)
{       
        int bucketIndex = this->getBucketIndex(key);

        if( this->buckets[bucketIndex].find(key) != this->buckets[bucketIndex].end() )
        {
            this->buckets[bucketIndex][key].insert(pageIndex);
        }
        else
        {
            set<int> pageIndexSet;
            pageIndexSet.insert(pageIndex);
            this->buckets[bucketIndex].insert ( pair<int,set<int>>(key,pageIndexSet) );
        }
        return;
}


void LinHash::insert(int key,int pageIndex)
{
        this->insert2bucket(key,pageIndex);
        this->numRecords++;

        while(this->occupancy() >= 75) 
        {

            map<int, set<int>> newBucket;
            buckets.push_back(newBucket);
            this->numBuckets++;
            this->numBits = ceil(log2((double)this->numBuckets));

            // split old bucket and rehash
            int k = this->numBuckets - 1 - (1 << (this->numBits - 1));

            map<int,set<int>> tempBucket(this->buckets[k].begin(),this->buckets[k].end());
            this->buckets[k].clear();

            map<int, set<int>>::iterator it; 
            for(it = tempBucket.begin(); it != tempBucket.end(); ++it)
            {
                int key = it->first;
                set<int> pageIndexset(it->second.begin(),it->second.end());
                set<int>::iterator its;
                for (its=pageIndexset.begin(); its!=pageIndexset.end(); ++its)
                {
                    int pageIndex = *its;
                    this->insert2bucket(key,pageIndex);
                }
            }

        }
        return;
}

