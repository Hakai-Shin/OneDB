#include "global.h"

/**
 * @brief Construct a new Table:: Table object
 *
 */
Table::Table()
{
    logger.log("Table::Table");
}

/**
 * @brief Construct a new Table:: Table object used in the case where the data
 * file is available and LOAD command has been called. This command should be
 * followed by calling the load function;
 *
 * @param tableName 
 */
Table::Table(string tableName)
{
    logger.log("Table::Table");
    this->sourceFileName = "../data/" + tableName + ".csv";
    this->tableName = tableName;
}

/**
 * @brief Construct a new Table:: Table object used when an assignment command
 * is encountered. To create the table object both the table name and the
 * columns the table holds should be specified.
 *
 * @param tableName 
 * @param columns 
 */
Table::Table(string tableName, vector<string> columns)
{
    logger.log("Table::Table");
    this->sourceFileName = "../data/temp/" + tableName + ".csv";
    this->tableName = tableName;
    this->columns = columns;
    this->columnCount = columns.size();
    this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (32 * columnCount));
    this->writeRow<string>(columns);
}

/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks and updates table
 * statistics.
 *
 * @return true if the table has been successfully loaded 
 * @return false if an error occurred 
 */
bool Table::load()
{
    logger.log("Table::load");
    fstream fin(this->sourceFileName, ios::in);
    string line;
    if (getline(fin, line))
    {
        fin.close();
        if (this->extractColumnNames(line))
            if (this->blockify())
                return true;
    }
    fin.close();
    return false;
}

/**
 * @brief Function extracts column names from the header line of the .csv data
 * file. 
 *
 * @param line 
 * @return true if column names successfully extracted (i.e. no column name
 * repeats)
 * @return false otherwise
 */
bool Table::extractColumnNames(string firstLine)
{
    logger.log("Table::extractColumnNames");
    unordered_set<string> columnNames;
    string word;
    stringstream s(firstLine);
    while (getline(s, word, ','))
    {
        word.erase(std::remove_if(word.begin(), word.end(), ::isspace), word.end());
        if (columnNames.count(word))
            return false;
        columnNames.insert(word);
        this->columns.emplace_back(word);
    }
    this->columnCount = this->columns.size();
    this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (32 * this->columnCount));
    return true;
}

/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size. 
 *
 * @return true if successfully blockified
 * @return false otherwise
 */
bool Table::blockify()
{
    logger.log("Table::blockify");
    ifstream fin(this->sourceFileName, ios::in);
    string line, word;
    vector<int> row(this->columnCount, 0);
    vector<vector<int>> rowsInPage(this->maxRowsPerBlock, row);
    int pageCounter = 0;
    unordered_set<int> dummy;
    dummy.clear();
    this->distinctValuesInColumns.assign(this->columnCount, dummy);
    this->distinctValuesPerColumnCount.assign(this->columnCount, 0);
    getline(fin, line);
    while (getline(fin, line))
    {
        stringstream s(line);
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            if (!getline(s, word, ','))
                return false;
            row[columnCounter] = stoi(word);
            rowsInPage[pageCounter][columnCounter] = row[columnCounter];
        }
        pageCounter++;
        this->updateStatistics(row);
        if (pageCounter == this->maxRowsPerBlock)
        {
            bufferManager.writePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
            bufferManager.updatePool(this->tableName, this->blockCount, rowsInPage, pageCounter,"update");
            this->pageOrder.push_back(this->blockCount);
            this->blockCount++;
            this->rowsPerBlockCount.emplace_back(pageCounter);
            pageCounter = 0;
        }
    }
    if (pageCounter)
    {
        bufferManager.writePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
        this->pageOrder.push_back(this->blockCount);
        this->blockCount++;
        this->rowsPerBlockCount.emplace_back(pageCounter);
        pageCounter = 0;
    }

    if (this->rowCount == 0)
        return false;
    this->distinctValuesInColumns.clear();
    return true;
}

/**
 * @brief Given a row of values, this function will update the statistics it
 * stores i.e. it updates the number of rows that are present in the column and
 * the number of distinct values present in each column. These statistics are to
 * be used during optimisation.
 *
 * @param row 
 */
void Table::updateStatistics(vector<int> row)
{
    this->rowCount++;
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (!this->distinctValuesInColumns[columnCounter].count(row[columnCounter]))
        {
            this->distinctValuesInColumns[columnCounter].insert(row[columnCounter]);
            this->distinctValuesPerColumnCount[columnCounter]++;
        }
    }
}

/**
 * @brief Checks if the given column is present in this table.
 *
 * @param columnName 
 * @return true 
 * @return false 
 */
bool Table::isColumn(string columnName)
{
    logger.log("Table::isColumn");
    for (auto col : this->columns)
    {
        if (col == columnName)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Renames the column indicated by fromColumnName to toColumnName. It is
 * assumed that checks such as the existence of fromColumnName and the non prior
 * existence of toColumnName are done.
 *
 * @param fromColumnName 
 * @param toColumnName 
 */
void Table::renameColumn(string fromColumnName, string toColumnName)
{
    logger.log("Table::renameColumn");
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (columns[columnCounter] == fromColumnName)
        {
            columns[columnCounter] = toColumnName;
            break;
        }
    }
    return;
}

/**
 * @brief Function prints the first few rows of the table. If the table contains
 * more rows than PRINT_COUNT, exactly PRINT_COUNT rows are printed, else all
 * the rows are printed.
 *
 */
void Table::print()
{
    logger.log("Table::print");
    uint count = min((long long)PRINT_COUNT, this->rowCount);

    //print headings
    this->writeRow(this->columns, cout);

    Cursor cursor(this->tableName, 0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < count; rowCounter++)
    {
        row = cursor.getNext();
        this->writeRow(row, cout);
    }
    printRowCount(this->rowCount);
}



/**
 * @brief This function returns one row of the table using the cursor object. It
 * returns an empty row is all rows have been read.
 *
 * @param cursor 
 * @return vector<int> 
 */
void Table::getNextPage(Cursor *cursor)
{
    logger.log("Table::getNext");

        if (cursor->pageIndex < this->blockCount - 1)
        {
            cursor->nextPage(cursor->pageIndex+1,this->pageOrder[cursor->pageIndex+1]); // cursor->pageIndex+1); TODO replace cursor->pageIndex+1 with cursor->pageIndex+1,this->pageOrder[cursor->pageIndex+1]
        }
}



/**
 * @brief called when EXPORT command is invoked to move source file to "data"
 * folder.
 *
 */
void Table::makePermanent()
{
    logger.log("Table::makePermanent");
    if(!this->isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
    string newSourceFile = "../data/" + this->tableName + ".csv";
    ofstream fout(newSourceFile, ios::out);

    //print headings
    this->writeRow(this->columns, fout);

    Cursor cursor(this->tableName, 0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        row = cursor.getNext();
        this->writeRow(row, fout);
    }
    fout.close();
}

/**
 * @brief Function to check if table is already exported
 *
 * @return true if exported
 * @return false otherwise
 */
bool Table::isPermanent()
{
    logger.log("Table::isPermanent");
    if (this->sourceFileName == "../data/" + this->tableName + ".csv")
    return true;
    return false;
}

/**
 * @brief The unload function removes the table from the database by deleting
 * all temporary files created as part of this table
 *
 */
void Table::unload(){
    logger.log("Table::~unload");
    for (int pageCounter = 0; pageCounter < this->blockCount; pageCounter++)
        bufferManager.deleteFile(this->tableName, this->pageOrder[pageCounter]);
    if (!isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
}

/**
 * @brief The unload function removes the table from the database by deleting
 * all temporary files created as part of this table
 *
 */
void Table::unloadTemp(){
    logger.log("Table::~unloadTemp");
    for (int pageCounter = 0; pageCounter < this->blockCount; pageCounter++)
        bufferManager.deleteFile(this->tableName, this->pageOrder[pageCounter]);
}

/**
 * @brief Function that returns a cursor that reads rows from this table
 * 
 * @return Cursor 
 */
Cursor Table::getCursor()
{
    logger.log("Table::getCursor");
    Cursor cursor(this->tableName, 0);
    return cursor;
}
/**
 * @brief Function that returns the index of column indicated by columnName
 * 
 * @param columnName 
 * @return int 
 */
int Table::getColumnIndex(string columnName)
{
    logger.log("Table::getColumnIndex");
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (this->columns[columnCounter] == columnName)
            return columnCounter;
    }
}

/**
 * @brief Function creates and populates index object based on the strategy given,
 * index object is stored in table object.
 *
 * @param string, 
 * @param IndexingStrategy
 * @param string 
 */
void Table::createIndex(string indexColumnName,IndexingStrategy indexingStrategy,string indexParameter)
{
    logger.log("Table::createIndex");

    this->tableIndex = new TableIndex(indexingStrategy,indexParameter,this->maxRowsPerBlock);

    int columnIndex = this->getColumnIndex(indexColumnName);
    this->indexedColumnName = indexColumnName;

    Cursor cursor(this->tableName, 0);
    vector<int> row;
    row = cursor.getNext();
    while (!row.empty())
    {
        this->tableIndex->insertKey(row[columnIndex],cursor.pageIndex,indexingStrategy);
        row = cursor.getNext();
    }
    return;
}


/**
 * @brief Function that sorts in memory pages based on column
 * 
 * @param idx
 * @param startPageIndex 
 * @param strategy 
 * @param finalPageIndex 
 */
vector<vector<int>> Table::blocks2sortedRecords(int idx,int startPageIndex,int finalPageIndex,SortingStrategy strategy)
{       
        logger.log("Table::blocks2sortedRecords");
        vector<vector<int>> records;

        for(int i=startPageIndex;i<=finalPageIndex;i++)
        {
            Cursor cursor(this->tableName, this->pageOrder[i]); //i); TODO  replace i with this->pageorder[i], this is for case when dorting unordered table for indexing
            vector<int> row = cursor.page.getRow(cursor.pagePointer);
            cursor.pagePointer++;
            while (!row.empty())
            {
                records.push_back(row);
                row = cursor.page.getRow(cursor.pagePointer);
                cursor.pagePointer++;
            }
        }
        if(strategy == ASC)
        {
        sort(records.begin(), records.end(),[idx] (const vector<int>& v1,const vector<int>& v2) {return v1[idx] <= v2[idx];}); 
        }
        else if(strategy == DESC)
        {
           sort(records.begin(), records.end(),[idx] (const vector<int>& v1,const vector<int>& v2) {return v1[idx] > v2[idx];});  
        }
        return records;
}


/**
 * @brief Function that sorts the table based on column
 * 
 * @param columnName 
 * @param strategy 
 */
void Table::tableSort(string sortingColumnName,SortingStrategy strategy,int bufferSize)
{
    system("mkdir ../data/temp/folder0");
    system("mkdir ../data/temp/folder1");
    bool flag;
    if(strategy == ASC)
    {
        flag = true;
    }
    else if(strategy == DESC)
    {
        flag = false;
    }
    logger.log("Table::tableSort");
    int idx  = this->getColumnIndex(sortingColumnName);
    
    int numBlocks = this->blockCount;
    
    if(numBlocks <= bufferSize)
    {
       vector<vector<int>> records;
       int startPageIndex = 0;
       int finalPageIndex = this->blockCount - 1; 
       records = this->blocks2sortedRecords(idx,startPageIndex,finalPageIndex,strategy);
       string outputfile = "../data/temp/folder0/" + to_string(0);
       records2file(records,outputfile);
       this->files2blocks(outputfile);
    }
    else
    {
      string ofp;
      vector<vector<int>> records;
      vector<string> files;
      int startPageIndex = 0;
      int filenumber = 0;
      int finalPageIndex = startPageIndex + bufferSize - 1;
      while(startPageIndex < numBlocks)
      {   

        records = this->blocks2sortedRecords(idx,startPageIndex,finalPageIndex,strategy);
        string infile = "../data/temp/folder0/" + to_string(filenumber);
        records2file(records,infile);
        files.push_back(infile);
        filenumber=filenumber+1;

        startPageIndex = finalPageIndex + 1;
        if((startPageIndex + bufferSize - 1) <= (numBlocks - 1))
        {
          finalPageIndex = startPageIndex + bufferSize - 1;  
        }
        else
        {
           finalPageIndex = numBlocks - 1; 
        }     
      }
      logger.log("Table::phase2sort");
      ofp = phase2sort(files,bufferSize,idx,flag);
      this->files2blocks(ofp);
    }
    system("rm -rf ../data/temp/folder1");
    system("rm -rf ../data/temp/folder0");
    
   return; 
}

void Table::records2pages(vector<vector<int>> records,int pageId)
{   
    bufferManager.writePage(this->tableName, pageId, records, this->rowsPerBlockCount[pageId]);
    bufferManager.updatePool(this->tableName, pageId, records, this->rowsPerBlockCount[pageId],"insert");
}


void Table::deletePages(vector<int> pagelist)
{
    vector<vector<int>> rowsInPage;
    int pageCounter;
    for(int i =0;i<pagelist.size();i++)
    {
        int pageId = pagelist[i];
        bufferManager.updatePool(this->tableName, pageId, rowsInPage, pageCounter,"delete");
        bufferManager.deleteFile(this->tableName,pageId);
    }
    return;
}

void Table::files2blocks(string filename)
{
  logger.log("Table::files2blocks");
  ifstream fin(filename, ios::in);
  string line;
  vector<vector<int>> records;
  vector<int> record;
  this->deletePages(this->pageOrder);
  this->rowsPerBlockCount.clear();
  this->pageOrder.clear();
  this->blockCount = ceil((float)this->rowCount/this->maxRowsPerBlock);
  uint totalRowCount = this->rowCount;
  //reverse(rowsperblock.begin(),rowsperblock.end());
  for(int pageId=0;pageId<this->blockCount;pageId++)
    {
        
        uint rowscount = min(this->maxRowsPerBlock,totalRowCount);
        uint rowsperblock = rowscount; 
        totalRowCount -= rowscount;
        while(rowscount-->0)
        {
            getline(fin, line);
            record = line2record(line);
            records.push_back(record);
        } 
        if(!records.empty()) {
            this->rowsPerBlockCount.push_back(rowsperblock);
            this->pageOrder.push_back(pageId);
            this->records2pages(records,pageId);
        }  
        records.clear();
    }
}