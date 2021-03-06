#include "global.h"
/**
 * @brief 
 * SYNTAX: INDEX ON column_name FROM relation_name USING indexing_strategy [FANOUT <number_of_children_pointers>|
BUCKETS <bucket_count>]
 * indexing_strategy: BTREE | HASH | NOTHING
 */
bool syntacticParseINDEX()
{
    logger.log("syntacticParseINDEX");
    if (tokenizedQuery.size() < 7 || tokenizedQuery.size() > 9 || tokenizedQuery[1] != "ON" || tokenizedQuery[3] != "FROM" || tokenizedQuery[5] != "USING")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    if ( tokenizedQuery.size() == 7 && tokenizedQuery[6] != "NOTHING" )
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    if ( tokenizedQuery.size() == 9 && ( (tokenizedQuery[6] == "BTREE" && tokenizedQuery[7] != "FANOUT") || (tokenizedQuery[6] == "HASH" && tokenizedQuery[7] != "BUCKETS") ))
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = INDEX;
    parsedQuery.indexColumnName = tokenizedQuery[2];
    parsedQuery.indexRelationName = tokenizedQuery[4];
    string indexingStrategy = tokenizedQuery[6];
    if (indexingStrategy == "BTREE")
        {
            parsedQuery.indexingStrategy = BTREE;
            parsedQuery.indexParameter = tokenizedQuery[8];
        }
    else if (indexingStrategy == "HASH")
        {
            parsedQuery.indexingStrategy = HASH;
            parsedQuery.indexParameter = tokenizedQuery[8];
        }
    else if (indexingStrategy == "NOTHING")
        parsedQuery.indexingStrategy = NOTHING;
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    return true;
}

bool semanticParseINDEX()
{
    logger.log("semanticParseINDEX");
    if (!tableCatalogue.isTable(parsedQuery.indexRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    if (!tableCatalogue.isColumnFromTable(parsedQuery.indexColumnName, parsedQuery.indexRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }
    Table* table = tableCatalogue.getTable(parsedQuery.indexRelationName);
    if(table->indexed){
        cout << "SEMANTIC ERROR: Table already indexed" << endl;
        return false;
    }
    return true;
}

void executeINDEX()
{
    logger.log("executeINDEX");
    Table* table = tableCatalogue.getTable(parsedQuery.indexRelationName);
    table->tableSort(parsedQuery.indexColumnName,ASC,BLOCK_COUNT);
    //table->createIndex(parsedQuery.indexColumnName,parsedQuery.indexingStrategy,parsedQuery.indexParameter);
    //table->indexed = true;
    return;
}