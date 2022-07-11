#include "tableCatalogue.h"

using namespace std;

enum QueryType
{
    ALTER,
    BULK_INSERT,
    CLEAR,
    CROSS,
    DELETE,
    DISTINCT,
    EXPORT,
    GROUPBY,
    INDEX,
    INSERT,
    JOIN,
    LIST,
    LOAD,
    LOADMATRIX,
    PRINT,
    PROJECTION,
    RENAME,
    SELECTION,
    SORT,
    SOURCE,
    UNDETERMINED
};

enum BinaryOperator
{
    LESS_THAN,
    GREATER_THAN,
    LEQ,
    GEQ,
    EQUAL,
    NOT_EQUAL,
    NO_BINOP_CLAUSE
};

enum aggregateOperator
{
    MAX,
    MIN,
    AVG,
    SUM
};


enum SelectType
{
    COLUMN,
    INT_LITERAL,
    NO_SELECT_CLAUSE
};

class ParsedQuery
{

public:
    QueryType queryType = UNDETERMINED;

    string clearRelationName = "";

    string alterRelationName = "";
    string alterColumnName = "";
    string alterMethod = "";

    string bulkInsertResultRelationName = "";
    string bulkInsertCSVName = "";

    string crossResultRelationName = "";
    string crossFirstRelationName = "";
    string crossSecondRelationName = "";

    string deleteRelationName = "";
    vector<int> deleteRow;

    string distinctResultRelationName = "";
    string distinctRelationName = "";

    string exportRelationName = "";

    aggregateOperator groupbyAggregateOperator;
    string groupbyResultRelationName = "";
    string groupbyRelationName = "";
    string groupbyGroupingColumnName = "";
    string groupbyAggregateColumnName = "";
    string groupbyAggregateOperatorstring = "";

    IndexingStrategy indexingStrategy = NOTHING;
    string indexParameter = "";
    string indexColumnName = "";
    string indexRelationName = "";

    string insertRelationName = "";
    vector<int> insertRow;

    BinaryOperator joinBinaryOperator = NO_BINOP_CLAUSE;
    string joinResultRelationName = "";
    string joinFirstRelationName = "";
    string joinSecondRelationName = "";
    string joinFirstColumnName = "";
    string joinSecondColumnName = "";

    string loadRelationName = "";

    string printRelationName = "";

    string projectionResultRelationName = "";
    vector<string> projectionColumnList;
    string projectionRelationName = "";

    string renameFromColumnName = "";
    string renameToColumnName = "";
    string renameRelationName = "";

    SelectType selectType = NO_SELECT_CLAUSE;
    BinaryOperator selectionBinaryOperator = NO_BINOP_CLAUSE;
    string selectionResultRelationName = "";
    string selectionRelationName = "";
    string selectionFirstColumnName = "";
    string selectionSecondColumnName = "";
    int selectionIntLiteral = 0;

    SortingStrategy sortingStrategy = NO_SORT_CLAUSE;
    string sortResultRelationName = "";
    string sortColumnName = "";
    string sortRelationName = "";
    int bufferSize; 

    string sourceFileName = "";

    ParsedQuery();
    void clear();
};

bool syntacticParse();
bool syntacticParseALTER();
bool syntacticParseBULK_INSERT();
bool syntacticParseCLEAR();
bool syntacticParseCROSS();
bool syntacticParseDELETE();
bool syntacticParseDISTINCT();
bool syntacticParseEXPORT();
bool syntacticParseGROUPBY();
bool syntacticParseINDEX();
bool syntacticParseINSERT();
bool syntacticParseJOIN();
bool syntacticParseLIST();
bool syntacticParseLOAD();
bool syntacticParseLOADMATRIX();
bool syntacticParsePRINT();
bool syntacticParsePROJECTION();
bool syntacticParseRENAME();
bool syntacticParseSELECTION();
bool syntacticParseSORT();
bool syntacticParseSOURCE();

bool isFileExists(string tableName);
bool isQueryFile(string fileName);
