#include "global.h"

/**
 * @brief 
 * SYNTAX: ALTER TABLE <table_name> ADD|DELETE COLUMN <column_name>
 */

bool syntacticParseALTER()
{
    logger.log("syntacticParseALTER");
    if (tokenizedQuery.size() != 6 || tokenizedQuery[1] != "TABLE" || tokenizedQuery[4] != "COLUMN")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    parsedQuery.queryType = ALTER;
    parsedQuery.alterRelationName = tokenizedQuery[2];
    parsedQuery.alterColumnName = tokenizedQuery[5];

    string method = tokenizedQuery[3];
    if(method == "ADD")
    {
        parsedQuery.alterMethod = "ADD";
    }
    else if(method == "DELETE")
    {
        parsedQuery.alterMethod = "DELETE";
    }
    else
    {
      cout << "SYNTAX ERROR" << endl;
      return false;  
    }
    return true;
}

bool semanticParseALTER()
{
    logger.log("semanticParseALTER");
    if (!tableCatalogue.isTable(parsedQuery.alterRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    if ( (parsedQuery.alterMethod == "DELETE") && !tableCatalogue.isColumnFromTable(parsedQuery.alterColumnName, parsedQuery.alterRelationName))
    {
            cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
            return false;
    }
    if ( (parsedQuery.alterMethod == "ADD") && tableCatalogue.isColumnFromTable(parsedQuery.alterColumnName, parsedQuery.alterRelationName))
    {
            cout << "SEMANTIC ERROR: Column exist in relation" << endl;
            return false;
    }
    return true;
}


void executeALTER()
{
Table* table = tableCatalogue.getTable(parsedQuery.alterRelationName);
vector<string> columnList = table->columns;
int page_index;
int pageid;
vector<int> row;
if(!table->isPermanent())
{
    bufferManager.deleteFile(table->sourceFileName);
}
if(parsedQuery.alterMethod == "ADD")
    {
        columnList.push_back(parsedQuery.alterColumnName);
        Table* resultantTable = new Table(table->tableName, columnList);
        
        for(int page_index=0; page_index<table->blockCount; page_index++)
        {
          pageid = table->pageOrder[page_index]; //page_index;  TODO replace page_index with table->pageorder[page_index]
          Cursor cursor(table->tableName, pageid);
          row = cursor.page.getRow(cursor.pagePointer);
          cursor.pagePointer++;
          while(!row.empty())
          {
              row.push_back(0);
              resultantTable->writeRow<int>(row);
              row = cursor.page.getRow(cursor.pagePointer);
              cursor.pagePointer++;
          }
        }
        tableCatalogue.deleteTempTable(table->tableName);
        resultantTable->blockify();
        tableCatalogue.insertTable(resultantTable);
        cout<<"Target column added to table"<<endl;
    }
else if(parsedQuery.alterMethod == "DELETE")
    {
        if(table->columns.size()==1)
        {
            tableCatalogue.deleteTable(table->tableName);
            cout<<"Target column deleted. Table has no more columns , so table deleted."<<endl;
            return;
        }
        vector<int> columnIndices;
        int targetColumnIndex = table->getColumnIndex(parsedQuery.alterColumnName);
        columnList.erase(columnList.begin()+targetColumnIndex);
        Table* resultantTable = new Table(table->tableName, columnList);
        for (int columnCounter = 0; columnCounter < columnList.size(); columnCounter++)
        {
             columnIndices.emplace_back(table->getColumnIndex(columnList[columnCounter]));
        }

        vector<int> resultantRow(columnList.size(), 0);

        for(page_index=0; page_index<table->blockCount; page_index++)
        {
             pageid = table->pageOrder[page_index]; //page_index;  TODO replace page_index with table->pageorder[page_index]
             Cursor cursor(table->tableName, pageid);
             row = cursor.page.getRow(cursor.pagePointer);
             cursor.pagePointer++;
             while (!row.empty())
             {

                for (int columnCounter = 0; columnCounter < columnIndices.size(); columnCounter++)
                {
                    resultantRow[columnCounter] = row[columnIndices[columnCounter]];
                }
                resultantTable->writeRow<int>(resultantRow);
                row = cursor.page.getRow(cursor.pagePointer);
                cursor.pagePointer++;
             }
        }

        tableCatalogue.deleteTempTable(table->tableName);
        resultantTable->blockify();
        tableCatalogue.insertTable(resultantTable);
        cout<<"Target column deleted from table"<<endl;
    }
return;
}

/*
void executeALTER()
{
    logger.log("executeALTER");
    Table* table = tableCatalogue.getTable(parsedQuery.alterRelationName);
    // cout<<parsedQuery.alterRelationName<<endl;
    // cout<<parsedQuery.alterColumnName<<endl;
    // cout<<parsedQuery.alterMethod<<endl;
    int page_index;
    int pageid;
    int pageCounter = 0;
    vector<vector<int>> rowsInPage;
    vector<int> row;
    if(parsedQuery.alterMethod == "ADD")
    {
        table->columnCount++;
        table->columns.push_back(parsedQuery.alterColumnName);

        for(page_index=0; page_index<table->blockCount; page_index++)
        {
          pageid = table->pageOrder[page_index]; //page_index;  TODO replace page_index with table->pageorder[page_index]
          Cursor cursor(table->tableName, pageid);
          row = cursor.page.getRow(cursor.pagePointer);
          
          while(!row.empty())
          {
              pageCounter++;
              row.push_back(0);
              rowsInPage.emplace_back(row);
              row = cursor.page.getRow(cursor.pagePointer);
              cursor.pagePointer++;
          }
          bufferManager.writePage(table->tableName, pageid, rowsInPage, pageCounter);
          bufferManager.updatePool(table->tableName, pageid, rowsInPage, pageCounter,"update");       
        }
    }
    else
    {
        
        table->columnCount--;
        auto it = find(table->columns.begin(),table->columns.end(),parsedQuery.alterColumnName );
        

        vector<int> columnIndices;
        for (int columnCounter = 0; columnCounter < table->columns.size(); columnCounter++)
        {
            if(columnCounter != it-table->columns.begin())
             columnIndices.emplace_back(table->getColumnIndex(table->columns[columnCounter]));
        }
        table->columns.erase(it);
        vector<int> resultantRow(columnIndices.size(), 0);

        for(page_index=0; page_index<table->blockCount; page_index++)
        {
             pageid = table->pageOrder[page_index]; //page_index;  TODO replace page_index with table->pageorder[page_index]
             Cursor cursor(table->tableName, pageid);
             row = cursor.page.getRow(cursor.pagePointer);
             while (!row.empty())
             {

                for (int columnCounter = 0; columnCounter < columnIndices.size(); columnCounter++)
                {
                    resultantRow[columnCounter] = row[columnIndices[columnCounter]];
                }
                pageCounter++;
                rowsInPage.emplace_back(resultantRow);
                row = cursor.page.getRow(cursor.pagePointer);
                cursor.pagePointer++;
             }

             bufferManager.writePage(table->tableName, pageid, rowsInPage, pageCounter);
             bufferManager.updatePool(table->tableName, pageid, rowsInPage, pageCounter,"update") ;
            
        }

    }
    
    return;
} 
*/