#include "table.h"

/**
 * @brief The TableCatalogue acts like an index of tables existing in the
 * system. Everytime a table is added(removed) to(from) the system, it needs to
 * be added(removed) to(from) the tableCatalogue. 
 *
 */
class TableCatalogue
{

    unordered_map<string, Table*> tables;
    unordered_map<string, TableIndex*> indexes;

public:
    TableCatalogue() {}
    void insertTable(Table* table);
    void deleteTable(string tableName);
    void deleteTempTable(string tableName);
    Table* getTable(string tableName);
    bool isTable(string tableName);
    bool isColumnFromTable(string columnName, string tableName);
    void print();
    ~TableCatalogue();
};