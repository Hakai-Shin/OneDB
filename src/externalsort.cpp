#include "global.h"

void records2file(vector<vector<int>> records,string filename)
{
    logger.log("ExternalSort::records2file");
    ofstream fout;
    fout.open(filename);
    for (int rowCounter = 0; rowCounter < records.size(); rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < records[0].size(); columnCounter++)
        {
            if (columnCounter != 0)
                fout << " ";
            fout << records[rowCounter][columnCounter];
        }
        fout << endl;
    }
    fout.close();
    return;
}

vector<int> line2record(string line)
{
    logger.log("ExternalSort::line2record");
    vector<int> record;
    string word;
    istringstream ss(line);

    while (ss >> word) 
    {
        record.push_back(stoi(word));
    }
    return record;
}

struct HeapNode
{
	vector<int> record;
    int value;
	int ptr;
    bool flag;
};

class Compare
{
public:
    //Ascending order sort
    bool operator() (HeapNode pair1, HeapNode pair2)
    {
        if(pair1.flag)
        return pair1.value >= pair2.value;
        else 
        return pair1.value < pair2.value;
    }
};




// Merges k sorted files. Names of files are assumed to be 1, 2, ... k
void mergeFiles( vector<string> files,string output_file, int idx, int k,bool flag)
{
    logger.log("ExternalSort::mergeFiles");
    vector<vector<int>> records;
    vector<ifstream> ifp(k);


	// Create a min heap with k heap nodes. Every heap node has first
	// element of scratch output file
	HeapNode harr[k];
    //CompareRecord cmp(flag);
	priority_queue<HeapNode, vector<HeapNode>, Compare> Heap;
	for (int i = 0; i < k; i++) {
        ifp[i].open(files[i]);
        string firstline;
        getline(ifp[i],firstline); //first record in the file
        harr[i].record = line2record(firstline);
        harr[i].value = harr[i].record[idx];
        harr[i].ptr = i;
        harr[i].flag = flag;
        Heap.push(harr[i]);  
    }
    

	// Now one by one get the minimum element from min heap and replace
	// it with next element. Run till all filled input files reach EOF
	while (Heap.size() > 0) {

        HeapNode root = Heap.top();
        Heap.pop();

        records.push_back(root.record);

        string nextline;
        if (getline(ifp[root.ptr],nextline)) {
        root.record = line2record(nextline);
        root.value = root.record[idx];
        Heap.push(root);
        }
    }
    
	// close input and output files
	for (int i = 0; i < k; i++) {
        ifp[i].close();
    }
    records2file(records,output_file);
    
	return;
}

string phase2sort(vector<string> files,int bufferSize,int idx,bool flag)
{
    int k = bufferSize;
    string sortdir = "../data/temp/folder";
    vector<string> subset;
    vector<string> out_list;
    bool flip = true;
    string outloc = sortdir + to_string(flip);
    
    while(files.size()!=1)
    {
            int j=0;
            for(int i=0;i<files.size();i=i+k)
            {
                string outfile = outloc + "/" + to_string(j);

                std::vector<string>::iterator it = files.begin() + i,ite;
                ite = ((it+k) < files.end())? (it+k):files.end();

                subset.assign(it, ite);
                mergeFiles(subset,outfile,idx,subset.size(),flag);
                out_list.push_back(outfile);
                subset.clear();
                j=j+1;
            }

            flip = !flip;  
             
            if(flip)
            {
            system("rm -rf ../data/temp/folder1");
            system("mkdir ../data/temp/folder1");
            }
            else
            {
            system("rm -rf ../data/temp/folder0");
            system("mkdir ../data/temp/folder0");  
            }      

            files.clear();
            files.assign(out_list.begin(),out_list.end());
            outloc = sortdir + to_string(flip);
            out_list.clear();
    }
    return files[0];
}


