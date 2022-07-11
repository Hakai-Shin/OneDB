#include "global.h"

/**
 * @brief 
 * SYNTAX: BULK_INSERT csv_file INTO relation_name
 */
bool syntacticParseBULK_INSERT()
{
    logger.log("syntacticParseBULK_INSERT");
    if (tokenizedQuery.size() != 4 || tokenizedQuery[2] != "INTO" )
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = BULK_INSERT;
    parsedQuery.bulkInsertResultRelationName = tokenizedQuery[3];
    parsedQuery.bulkInsertCSVName = tokenizedQuery[1];
    return true;
}

bool semanticParseBULK_INSERT()
{
    logger.log("semanticParseBULK_INSERT");
    if (!tableCatalogue.isTable(parsedQuery.bulkInsertResultRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!isFileExists(parsedQuery.bulkInsertCSVName))
    {
        cout << "SEMANTIC ERROR: Data file to be bulk inserted doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeBULK_INSERT()
{
    logger.log("executeBULK_INSERT");

    Table* resultantTable = tableCatalogue.getTable(parsedQuery.bulkInsertResultRelationName);
    Table* temporaryTable = new Table(parsedQuery.bulkInsertCSVName);

    fstream fin(temporaryTable->sourceFileName, ios::in);
    string line;
    if (getline(fin, line))
    {
        fin.close();
        if (temporaryTable->extractColumnNames(line))
            {
                if(temporaryTable->columns!=resultantTable->columns)
                {
                    cout<<"Problem with table to be bulk inserted, columns not comapatible"<<endl;
            return;
                }

            }
        else
        {
            cout<<"Problem with table to be bulk inserted"<<endl;
            return;
        }
        
    }
    else
    {
      cout<<"Problem with table to be bulk inserted"<<endl;  
      return;
    }
    

    
    temporaryTable->blockify();

    int pageId = (*max_element(resultantTable->pageOrder.begin(), resultantTable->pageOrder.end())) + 1;
    resultantTable->blockCount += temporaryTable->blockCount;
    resultantTable->rowCount += temporaryTable->rowCount;

    string resultantPageName = "../data/temp/"+ resultantTable->tableName + "_Page";
    string temporaryPageName = "../data/temp/"+ temporaryTable->tableName + "_Page";

    for(int i=0;i<temporaryTable->blockCount;i++)
    {
        resultantPageName = resultantPageName + to_string(pageId);
        temporaryPageName = temporaryPageName + to_string(i);
        rename(temporaryPageName.c_str(),resultantPageName.c_str());
        resultantTable->pageOrder.push_back(pageId);
        pageId++;
        int rowscount = temporaryTable->rowsPerBlockCount[i];
        resultantTable->rowsPerBlockCount.push_back(rowscount);
    }

    delete temporaryTable;
    return;
}