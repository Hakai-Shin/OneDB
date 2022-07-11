#include "global.h"
/**
 * @brief 
 * SYNTAX: LOAD MATRIX relation_name
 */
bool syntacticParseLOADMATRIX()
{
    logger.log("syntacticParseLOADMATRIX");
    if (tokenizedQuery.size() != 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = LOADMATRIX;
    parsedQuery.loadRelationName = tokenizedQuery[2];
    return true;
}

bool semanticParseLOADMATRIX()
{
    logger.log("semanticParseLOADMATRIX");
    // if (tableCatalogue.isTable(parsedQuery.loadRelationName))
    // {
    //     cout << "SEMANTIC ERROR: Relation already exists" << endl;
    //     return false;
    // }

    if (!isFileExists(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Data file doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeLOADMATRIX()
{
    logger.log("executeLOADMATRIX");

    // Table *table = new Table(parsedQuery.loadRelationName);
    // if (table->load())
    // {
    //     tableCatalogue.insertTable(table);
    //     cout << "Loaded Table. Column Count: " << table->columnCount << " Row Count: " << table->rowCount << endl;
    // }
    return;
}