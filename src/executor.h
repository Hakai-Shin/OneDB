#include"semanticParser.h"

void executeCommand();

void executeALTER();
void executeBULK_INSERT();
void executeCLEAR();
void executeCROSS();
void executeDELETE();
void executeDISTINCT();
void executeEXPORT();
void executeGROUPBY();
void executeINDEX();
void executeINSERT();
void executeJOIN();
void executeLIST();
void executeLOAD();
void executeLOADMATRIX();
void executePRINT();
void executePROJECTION();
void executeRENAME();
void executeSELECTION();
void executeSORT();
void executeSOURCE();

bool evaluateBinOp(int value1, int value2, BinaryOperator binaryOperator);
void printRowCount(int rowCount);