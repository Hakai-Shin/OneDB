#include"global.h"

bool semanticParse(){
    logger.log("semanticParse");
    switch(parsedQuery.queryType){
        case ALTER: return semanticParseALTER();
        case BULK_INSERT: return semanticParseBULK_INSERT();
        case CLEAR: return semanticParseCLEAR();
        case CROSS: return semanticParseCROSS();
        case DELETE: return semanticParseDELETE();
        case DISTINCT: return semanticParseDISTINCT();
        case EXPORT: return semanticParseEXPORT();
        case GROUPBY: return semanticParseGROUPBY();
        case INDEX: return semanticParseINDEX();
        case INSERT: return semanticParseINSERT();
        case JOIN: return semanticParseJOIN();
        case LIST: return semanticParseLIST();
        case LOAD: return semanticParseLOAD();
        case LOADMATRIX: return semanticParseLOADMATRIX();
        case PRINT: return semanticParsePRINT();
        case PROJECTION: return semanticParsePROJECTION();
        case RENAME: return semanticParseRENAME();
        case SELECTION: return semanticParseSELECTION();
        case SORT: return semanticParseSORT();
        case SOURCE: return semanticParseSOURCE();
        default: cout<<"SEMANTIC ERROR"<<endl;
    }

    return false;
}