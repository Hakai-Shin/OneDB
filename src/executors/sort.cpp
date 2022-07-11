#include"global.h"
/**
 * @brief File contains method to process SORT commands.
 * 
 * syntax:
 * R <- SORT relation_name BY column_name IN sorting_order BUFFER buffer_size
 * 
 * sorting_order = ASC | DESC 
 */
bool syntacticParseSORT(){
    logger.log("syntacticParseSORT");
    if (tokenizedQuery.size() < 8 || tokenizedQuery.size() == 9 || tokenizedQuery.size() > 10 || tokenizedQuery[4] != "BY" || tokenizedQuery[6] != "IN")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    if ( tokenizedQuery.size() == 10 && tokenizedQuery[8] != "BUFFER" )
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    if(tokenizedQuery.size() == 10 && tokenizedQuery[8] == "BUFFER" )
    {
        parsedQuery.bufferSize = stoi(tokenizedQuery[9]);
    }
    else
    {
        parsedQuery.bufferSize = BLOCK_COUNT;
    }
    parsedQuery.queryType = SORT;
    parsedQuery.sortResultRelationName = tokenizedQuery[0];
    parsedQuery.sortColumnName = tokenizedQuery[5];
    parsedQuery.sortRelationName = tokenizedQuery[3];
    string sortingStrategy = tokenizedQuery[7];
    if(sortingStrategy == "ASC")
        parsedQuery.sortingStrategy = ASC;
    else if(sortingStrategy == "DESC")
        parsedQuery.sortingStrategy = DESC;
    else{
        cout<<"SYNTAX ERROR"<<endl;
        return false;
    }
    return true;
}

bool semanticParseSORT(){
    logger.log("semanticParseSORT");

    if(tableCatalogue.isTable(parsedQuery.sortResultRelationName)){
        cout<<"SEMANTIC ERROR: Resultant relation already exists"<<endl;
        return false;
    }

    if(!tableCatalogue.isTable(parsedQuery.sortRelationName)){
        cout<<"SEMANTIC ERROR: Relation doesn't exist"<<endl;
        return false;
    }

    if(!tableCatalogue.isColumnFromTable(parsedQuery.sortColumnName, parsedQuery.sortRelationName)){
        cout<<"SEMANTIC ERROR: Column doesn't exist in relation"<<endl;
        return false;
    }

    return true;
}

void executeSORT(){
    logger.log("executeSORT");

    Table table = *tableCatalogue.getTable(parsedQuery.sortRelationName);
    Table* resultantTable = new Table(parsedQuery.sortResultRelationName, table.columns);
    Cursor cursor = table.getCursor();
    vector<int> row = cursor.getNext();
    vector<int> resultantRow;

    while (!row.empty())
    {

        resultantRow = row;
        resultantTable->writeRow<int>(resultantRow);
        row = cursor.getNext();
    }

    resultantTable->blockify();
    tableCatalogue.insertTable(resultantTable);

    if(!table.indexed || table.indexedColumnName != parsedQuery.sortColumnName){
        resultantTable->tableSort(parsedQuery.sortColumnName,parsedQuery.sortingStrategy,parsedQuery.bufferSize);
        cout << "Table sorted on column "<< parsedQuery.sortColumnName<< endl;
    }

    return;
}