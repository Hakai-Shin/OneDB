#include "global.h"

/**
 * @brief 
 * SYNTAX: DELETE FROM relation_name VALUES <value1>[,<value2>]*
 * 
 */
bool syntacticParseDELETE()
{
    logger.log("syntacticParseDELETE");
    if (tokenizedQuery.size() < 5 || tokenizedQuery[1] != "FROM" || tokenizedQuery[3] != "VALUES")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = DELETE;
    parsedQuery.deleteRelationName = tokenizedQuery[2];
    for(int i=4 ; i < tokenizedQuery.size();i++)
    {
        parsedQuery.deleteRow.push_back(stoi(tokenizedQuery[i]));
    }
    return true;
}

bool semanticParseDELETE()
{
    logger.log("semanticParseDelete");

    if (!tableCatalogue.isTable(parsedQuery.deleteRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    return true;
}

void deleteEmptyPages(Table* table,vector<int> pageIndexes)
{
    if(pageIndexes.empty())
    {
        return;
    }
    for(int i=0;i<pageIndexes.size();i++)
    {
    vector<vector<int>> rowsInPage;
    int pageCounter;
    int pageIndex = pageIndexes[i];
    int pageId = table->pageOrder[pageIndex];
    table->rowsPerBlockCount.erase(table->rowsPerBlockCount.begin()+pageIndex);
    table->pageOrder.erase(table->pageOrder.begin()+pageIndex);
    table->blockCount--;
    bufferManager.updatePool(table->tableName, pageId, rowsInPage, pageCounter,"delete");
    bufferManager.deleteFile(table->tableName,pageId);
    }
    return;
}

void deleteRecordFromPages(Table *table, set<int> targetPages,vector<int> targetRow)
{       vector<int> row,emptyPageIndexes;
        vector<vector<int>> rowsInPage; 
        set<int>::iterator itr;
        for(itr = targetPages.begin();itr != targetPages.end(); ++itr)
        {
                int pageIndex = *itr;
                int pageCounter = 0,removedPageCount=0;
                int pageId = table->pageOrder[pageIndex];
                Cursor cursor(table->tableName, pageId);
                row = cursor.page.getRow(cursor.pagePointer);
                cursor.pagePointer++;
                rowsInPage.clear();
                while (!row.empty())
                {
                        if(row!=targetRow)
                        {
                            rowsInPage.push_back(row);
                            pageCounter++;
                        }
                        else
                        {
                            removedPageCount++;
                        }
                        row = cursor.page.getRow(cursor.pagePointer);
                        cursor.pagePointer++;
                }
                if(rowsInPage.empty())
                {
                   emptyPageIndexes.push_back(pageIndex); 
                }
                else
                {
                bufferManager.writePage(table->tableName, pageId, rowsInPage, pageCounter);
                bufferManager.updatePool(table->tableName, pageId, rowsInPage, pageCounter,"update");
                table->rowsPerBlockCount[pageIndex] = pageCounter; 
                }
                
                table->rowCount -= removedPageCount; 
        }
           deleteEmptyPages(table,emptyPageIndexes);
    return;
}

set<int> deleteRecordFromIndexedPages(Table *table, set<int> targetPages,vector<int> targetRow,int targetColumn,int key)
{       vector<int> row,emptyPageIndexes;
        set<int> updatedPageSet;
        vector<vector<int>> rowsInPage; 
        set<int>::iterator itr;
        for(itr = targetPages.begin();itr != targetPages.end(); ++itr)
        {
                int pageIndex = *itr;
                int pageCounter = 0,removedPageCount=0;
                int pageId = table->pageOrder[pageIndex];
                Cursor cursor(table->tableName, pageId);
                row = cursor.page.getRow(cursor.pagePointer);
                cursor.pagePointer++;
                rowsInPage.clear();
                while (!row.empty())
                {
                        if(row[targetColumn]==key)
                        {
                            if(row!=targetRow)
                            {
                                rowsInPage.push_back(row);
                                updatedPageSet.insert(pageIndex);
                                pageCounter++;
                             }
                            else
                            {
                                removedPageCount++;
                            }
                        }
                        else
                        {
                          rowsInPage.push_back(row);
                            pageCounter++;  
                        }                  
                        row = cursor.page.getRow(cursor.pagePointer);
                        cursor.pagePointer++;
                }
                if(rowsInPage.empty())
                {
                   emptyPageIndexes.push_back(pageIndex); 
                }
                else
                {
                bufferManager.writePage(table->tableName, pageId, rowsInPage, pageCounter);
                bufferManager.updatePool(table->tableName, pageId, rowsInPage, pageCounter,"update");
                table->rowsPerBlockCount[pageIndex] = pageCounter; 
                }
                
                table->rowCount -= removedPageCount; 
        }
        deleteEmptyPages(table,emptyPageIndexes);
    return updatedPageSet;
}


void executeDELETE()
{
    logger.log("executeDELETE");

    Table* table = tableCatalogue.getTable(parsedQuery.deleteRelationName);

    vector<int> targetRow(table->columnCount, 0);
    for( int i=0; i<parsedQuery.deleteRow.size(); i++)
        {
            targetRow[i] = parsedQuery.deleteRow[i];
        }
    parsedQuery.deleteRow.clear();

    if (table->indexed)
    {
        int targetColumn = table->getColumnIndex(table->indexedColumnName);
        int key = targetRow[targetColumn];
        set<int> targetPages,updatedPageSet;
        targetPages = table->tableIndex->getTargetPages(key,table->indexingStrategy);
        if(!targetPages.empty())
        {
        updatedPageSet = deleteRecordFromIndexedPages(table,targetPages,targetRow,targetColumn,key);
        table->tableIndex->updateTableIndex(key,updatedPageSet,table->indexingStrategy);
        cout << "Index Updated." << endl;
        }
        cout << "Records with given value deleted from Table." << " Row Count: " << table->rowCount << endl;
    }
    else
    {
        int pageCounter = 0;
        set<int> targetPages;
        
        vector<int> row;
        vector<vector<int>> rowsInPage;
        bool rowFound = false;

        

        //Page page = bufferManager.getPage(parsedQuery.deleteRelationName,0);

        Cursor cursor = table->getCursor();
        row = cursor.getNext();

        while (!row.empty())
        {
            if(row==targetRow)
                {
                    targetPages.insert(cursor.pageIndex);
                    rowFound = true;
                }
            row = cursor.getNext();
        }

        if(!rowFound)
        {
            cout<< "Record not present in the table."<<endl;
            return;
        }
        else
        {
           deleteRecordFromPages(table,targetPages,targetRow);
           cout << "Records with given value deleted from Table." << " Row Count: " << table->rowCount << endl;
           if(table->rowCount==0)
           {
               tableCatalogue.deleteTable(table->tableName);
               cout << "Table is empty, table deleted"<< endl;
           }
        }
        

    }
    
    return;
}