#include "global.h"

/**
 * @brief 
 * SYNTAX: INSERT INTO relation_name VALUES <value1>[,<value2>]*
 * 
 */
bool syntacticParseINSERT()
{
    logger.log("syntacticParseLOAD");
    if (tokenizedQuery.size() < 5 || tokenizedQuery[1] != "INTO" || tokenizedQuery[3] != "VALUES")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = INSERT;
    parsedQuery.insertRelationName = tokenizedQuery[2];
    for(int i=4 ; i < tokenizedQuery.size();i++)
    {
        parsedQuery.insertRow.push_back(stoi(tokenizedQuery[i]));
    }
    return true;
}

bool semanticParseINSERT()
{
    logger.log("semanticParseINSERT");

    if (!tableCatalogue.isTable(parsedQuery.insertRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    return true;
}

void executeINSERT() 
{
    logger.log("executeINSERT");

    Table* table = tableCatalogue.getTable(parsedQuery.insertRelationName);

    vector<int> newRow(table->columnCount, 0);
    for( int i=0; i<parsedQuery.insertRow.size(); i++)
    {
            newRow[i] = parsedQuery.insertRow[i];
    }
    parsedQuery.insertRow.clear();

    if (table->indexed)
    {
        
        cout << "Index Updated." << endl;
    }
    else
    {
        int pageIndex = -1,pageCounter = 0,pageId;
        
        vector<int> row;
        vector<vector<int>> rowsInPage;

        for(int i=0;i<table->rowsPerBlockCount.size();i++)
        {
            if(table->rowsPerBlockCount[i] < table->maxRowsPerBlock)
            {
                pageIndex = i;
                break;
            }
        }
        if(pageIndex!=-1)
        {
          pageId = table->pageOrder[pageIndex];
          Cursor cursor(table->tableName, pageId);
          row = cursor.page.getRow(cursor.pagePointer);
          cursor.pagePointer++;
          rowsInPage.clear();
          while (!row.empty())
          {    
            rowsInPage.push_back(row);
            pageCounter++;
            row = cursor.page.getRow(cursor.pagePointer);
            cursor.pagePointer++;
          }
          rowsInPage.emplace_back(newRow);
          pageCounter++;
          bufferManager.writePage(table->tableName, pageId, rowsInPage, pageCounter);
          bufferManager.updatePool(table->tableName, pageId, rowsInPage, pageCounter,"update");
          table->rowsPerBlockCount[pageIndex] = pageCounter;
        }
        else
        {
            pageId = (*max_element(table->pageOrder.begin(), table->pageOrder.end())) + 1;
            table->pageOrder.push_back(pageId);
            pageIndex = table->blockCount;
            table->blockCount = table->blockCount + 1;
            rowsInPage.emplace_back(newRow);
            pageCounter++;
            bufferManager.writePage(table->tableName, pageId, rowsInPage, pageCounter);
            bufferManager.updatePool(table->tableName, pageId, rowsInPage, pageCounter,"insert");
            table->rowsPerBlockCount.push_back(pageCounter);
        }
        table->rowCount++;
        
    }
    
    cout << "Row inserted into Table." << " Row Count: " << table->rowCount << endl;
    return;
}