#include"global.h"

void executeCommand(){

    switch(parsedQuery.queryType){
        case ALTER: executeALTER(); break;
        case BULK_INSERT: executeBULK_INSERT(); break;
        case CLEAR: executeCLEAR(); break;
        case CROSS: executeCROSS(); break;
        case DELETE: executeDELETE(); break;
        case DISTINCT: executeDISTINCT(); break;
        case EXPORT: executeEXPORT(); break;
        case GROUPBY: executeGROUPBY(); break;
        case INDEX: executeINDEX(); break;
        case INSERT: executeINSERT(); break;
        case JOIN: executeJOIN(); break;
        case LIST: executeLIST(); break;
        case LOAD: executeLOAD(); break;
        case LOADMATRIX: executeLOADMATRIX(); break;
        case PRINT: executePRINT(); break;
        case PROJECTION: executePROJECTION(); break;
        case RENAME: executeRENAME(); break;
        case SELECTION: executeSELECTION(); break;
        case SORT: executeSORT(); break;
        case SOURCE: executeSOURCE(); break;
        default: cout<<"PARSING ERROR"<<endl;
    }

    return;
}

void printRowCount(int rowCount){
    cout<<"\n\nRow Count: "<<rowCount<<endl;
    return;
}