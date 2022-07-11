#include "global.h"
/**
 * @brief 
 * SYNTAX: R <- GROUP BY <grouping_attribute> FROM <table_name> RETURN MAX|MIN|SUM|AVG(<attribute>)
 */

bool syntacticParseGROUPBY()
{
    logger.log("syntacticParseGROUPBY");
    if (tokenizedQuery.size() != 9 || tokenizedQuery[3] != "BY" || tokenizedQuery[5] != "FROM" || tokenizedQuery[7] != "RETURN")
    {
        cout << "SYNTAC ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = GROUPBY;
    parsedQuery.groupbyResultRelationName = tokenizedQuery[0];
    parsedQuery.groupbyGroupingColumnName = tokenizedQuery[4];
    parsedQuery.groupbyRelationName = tokenizedQuery[6];
    string aggOperation = tokenizedQuery[8];

    vector<string> tokenizedaggOperation;
    regex delim("[^()]+");
    auto words_begin = std::sregex_iterator(aggOperation.begin(), aggOperation.end(), delim);
    auto words_end = std::sregex_iterator();
    for (std::sregex_iterator i = words_begin; i != words_end; ++i)
            tokenizedaggOperation.emplace_back((*i).str());

    string aggregateOperator = tokenizedaggOperation[0];
    parsedQuery.groupbyAggregateColumnName = tokenizedaggOperation[1];
    
    if (aggregateOperator == "MAX")
    {
        parsedQuery.groupbyAggregateOperator = MAX;
        parsedQuery.groupbyAggregateOperatorstring = aggregateOperator;
    }      
    else if (aggregateOperator == "MIN")
    {
        parsedQuery.groupbyAggregateOperator = MIN;
        parsedQuery.groupbyAggregateOperatorstring = aggregateOperator;
    } 
    else if (aggregateOperator == "SUM")
    {
        parsedQuery.groupbyAggregateOperator = SUM;
        parsedQuery.groupbyAggregateOperatorstring = aggregateOperator;
    } 
    else if (aggregateOperator == "AVG")
    {
        parsedQuery.groupbyAggregateOperator = AVG;
        parsedQuery.groupbyAggregateOperatorstring = aggregateOperator;
    } 
    else
    {
        cout << "SYNTAC ERROR: operator not defined" << endl;
        return false;
    }
    return true;
}

bool semanticParseGROUPBY()
{
    logger.log("semanticParseGROUPBY");

    if (tableCatalogue.isTable(parsedQuery.groupbyResultRelationName))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.groupbyRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.groupbyGroupingColumnName, parsedQuery.groupbyRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.groupbyAggregateColumnName, parsedQuery.groupbyRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }
    
    return true;
}
/*
void SUM_AVG(int grpcolpos, int aggcolpos, Table *t, string agg_operator){
    vector<int> row;
    map<int, int> mp, avgmp;
    for(int i=0; i<t->blockCount; i++){
        Cursor cursor(t->tableName, i);
        row = cursor.getNext();
        while(!row.empty()){            
            mp[row[grpcolpos]]=0;
            avgmp[row[grpcolpos]]=0;
            row = cursor.getNext();
        }
    }
    int mapsize=mp.size();
    for(int i=0; i<t->blockCount; i++){
        Cursor cursor(t->tableName, i);
        map<int, vector<vector<int>>> umap;
        row = cursor.getNext();
        int rowCount =0;
        while(!row.empty() && rowCount< t->maxRowsPerBlock){
            rowCount++;
            umap[row[grpcolpos]].push_back(row);            
            row = cursor.getNext();
        }
        int rcount=0;
        for(auto arr: umap){
            rcount++;
            int f = arr.first;
            vector<vector<int>> temp = arr.second; 
            int mn=INT_MAX, ri;
            vector<int> rp;
            for(int r=0; r<temp.size(); r++){                
                mp[temp[r][grpcolpos]]+=temp[r][aggcolpos];
                avgmp[temp[r][grpcolpos]]+=1;
            }
        }                
    }
    vector<vector<int>> res;
    for(auto m: mp){
        if(agg_operator == "SUM")
            res.push_back({m.first, m.second});
        else if(agg_operator == "AVG")
            res.push_back({m.first, (m.second/avgmp[m.first])});
    }
    Table *resultTable = new Table(tokenizedQuery[0], {t->columns[grpcolpos], t->columns[aggcolpos]});
    for(auto rw: res){
        resultTable->writeRow<int>(rw);
    }
    resultTable->blockify();
    tableCatalogue.insertTable(resultTable);
}

void MIN_MAX(int grpcolpos, int agg_column_index, Table *t, string agg_operator){
    vector<int> row;
    map<int, int> mp;

  
    for(int i=0; i<t->blockCount; i++){
        Cursor cursor(t->tableName, i);
        row = cursor.getNext();
        while(!row.empty()){
            if(agg_operator == "MIN")
                mp[row[grpcolpos]]=INT_MAX;            
            else 
                mp[row[grpcolpos]]=INT_MIN;
            row = cursor.getNext();
        }
    }
    int mapsize=mp.size();
    for(int i=0; i<t->blockCount; i++){
        Cursor cursor(t->tableName, i);
        map<int, vector<vector<int>>> umap;
        row = cursor.getNext();
        int rowcount =0;
        while(!row.empty() && rowcount < t->maxRowsPerBlock){
            rowcount++;
            umap[row[grpcolpos]].push_back(row);            
            row = cursor.getNext();
        }
        int rcount=0;
        for(auto arr: umap){
            rcount++;
            int f = arr.first;
            vector<vector<int>> temp = arr.second; 
            int mn=INT_MAX, ri;
            vector<int> rp;
            if(agg_operator == "MIN"){
                for(int r=0; r<temp.size(); r++){                
                    if(mp[temp[r][grpcolpos]] >= temp[r][agg_column_index]){
                        mp[temp[r][grpcolpos]]=temp[r][agg_column_index];
                        ri=r;
                    }
                }
            }
            else if(agg_operator == "MAX"){
                for(int r=0; r<temp.size(); r++){                
                    if(mp[temp[r][grpcolpos]] <= temp[r][agg_column_index]){
                        mp[temp[r][grpcolpos]]=temp[r][agg_column_index];
                        ri=r;
                    }
                }
            }
        }                
    }
    vector<vector<int>> res;
    for(auto m: mp){
        res.push_back({m.first, m.second});
    }
    Table *resultTable = new Table(tokenizedQuery[0], {t->columns[grpcolpos], t->columns[agg_column_index]});
    for(auto rw: res){
        resultTable->writeRow<int>(rw);
    }
    resultTable->blockify();
    tableCatalogue.insertTable(resultTable);
}
*/

void MIN_MAX(int grpcolpos, int agg_column_index, Table *table,Table *resultTable, string agg_operator){
    vector<int> row;
    map<int, int> mp;

    int totalval = (agg_operator=="MAX")?INT_MIN:INT_MAX;
    int key,value,prevkey;
    vector<int> resultantRow;

    int pageid = table->pageOrder[0]; 
    Cursor cursor(table->tableName, pageid);
    row = cursor.page.getRow(cursor.pagePointer);
    prevkey = row[grpcolpos];

    for(int page_index=0; page_index<table->blockCount; page_index++)
        {
             int pageid = table->pageOrder[page_index]; //page_index;  TODO replace page_index with table->pageorder[page_index]
             Cursor cursor(table->tableName, pageid);
             row = cursor.page.getRow(cursor.pagePointer);
             cursor.pagePointer++;
             while (!row.empty())
             {

                key = row[grpcolpos];
                value = row[agg_column_index];

                if(key==prevkey)
                {
                    totalval = (agg_operator=="MAX")?(max(totalval,value)):(min(totalval,value));
                }
                else
                {
                    resultantRow.push_back(prevkey);
                    resultantRow.push_back(totalval);
                    resultTable->writeRow<int>(resultantRow);
                    prevkey = key;
                    totalval = value;
                    resultantRow.clear();
                }
                
        
                row = cursor.page.getRow(cursor.pagePointer);
                cursor.pagePointer++;
             }
        }
    resultantRow.push_back(key);
    resultantRow.push_back(totalval);
    resultTable->writeRow<int>(resultantRow);
    
    resultTable->blockify();
    tableCatalogue.insertTable(resultTable);
}

void SUM_AVG(int grpcolpos, int agg_column_index, Table *table,Table *resultTable, string agg_operator){
    vector<int> row;
    map<int, int> mp;

    int totalval = 0,rowcount=0;
    int key,value,prevkey;
    vector<int> resultantRow;

    int pageid = table->pageOrder[0]; 
    Cursor cursor(table->tableName, pageid);
    row = cursor.page.getRow(cursor.pagePointer);
    prevkey = row[grpcolpos];

    for(int page_index=0; page_index<table->blockCount; page_index++)
        {
             int pageid = table->pageOrder[page_index]; //page_index;  TODO replace page_index with table->pageorder[page_index]
             Cursor cursor(table->tableName, pageid);
             row = cursor.page.getRow(cursor.pagePointer);
             cursor.pagePointer++;
             while (!row.empty())
             {

                key = row[grpcolpos];
                value = row[agg_column_index];

                if(key==prevkey)
                {
                    totalval = totalval + value;
                    rowcount++;
                }
                else
                {
                    totalval = (agg_operator=="AVG")?(totalval/rowcount):(totalval);
                    resultantRow.push_back(prevkey);
                    resultantRow.push_back(totalval);
                    resultTable->writeRow<int>(resultantRow);
                    prevkey = key;
                    totalval = value;
                    rowcount=1;
                    resultantRow.clear();
                }
                
        
                row = cursor.page.getRow(cursor.pagePointer);
                cursor.pagePointer++;
             }
        }
    totalval = (agg_operator=="AVG")?(totalval/rowcount):(totalval);
    resultantRow.push_back(key);
    resultantRow.push_back(totalval);
    resultTable->writeRow<int>(resultantRow);
    
    resultTable->blockify();
    tableCatalogue.insertTable(resultTable);
}


void executeGROUPBY()
{
    logger.log("executeGROUPBY");

    Table *table = tableCatalogue.getTable(parsedQuery.groupbyRelationName);
    string aggregateColumn = parsedQuery.groupbyAggregateOperatorstring + parsedQuery.groupbyAggregateColumnName;
    vector<string> resultantTableColumns{parsedQuery.groupbyGroupingColumnName, aggregateColumn};
    Table* resultantTable = new Table(parsedQuery.groupbyResultRelationName, resultantTableColumns);

    if(table->rowCount==0)
    {
        return;
    }
    table->tableSort(parsedQuery.groupbyGroupingColumnName,ASC,BLOCK_COUNT);

    map<int, std::vector<std::vector<int> >> mp;
     logger.log("executeGROUPBY");  
    
    int groupcolumnindex = find(table->columns.begin(),table->columns.end(), parsedQuery.groupbyGroupingColumnName)-table->columns.begin();    
   
    int aggcolumnindex = find(table->columns.begin(),table->columns.end(),parsedQuery.groupbyAggregateColumnName )-table->columns.begin();
    if(parsedQuery.groupbyAggregateOperatorstring == "MIN" || parsedQuery.groupbyAggregateOperatorstring=="MAX"){
        //MIN_MAX(groupcolumnindex, aggcolumnindex, table, parsedQuery.groupbyAggregateOperatorstring);
        MIN_MAX(groupcolumnindex, aggcolumnindex, table,resultantTable, parsedQuery.groupbyAggregateOperatorstring);
    }    
    else if(parsedQuery.groupbyAggregateOperatorstring == "SUM" || parsedQuery.groupbyAggregateOperatorstring=="AVG"){
        //SUM_AVG(groupcolumnindex, aggcolumnindex, table, parsedQuery.groupbyAggregateOperatorstring);
        SUM_AVG(groupcolumnindex, aggcolumnindex, table,resultantTable, parsedQuery.groupbyAggregateOperatorstring);
    }    

    
    return;
}