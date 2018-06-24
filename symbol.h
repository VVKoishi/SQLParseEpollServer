#ifndef symbol_h
#define symbol_h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
using namespace std;

//常数种别标号1
//标识符表 种别标号2
static char IDentifierTbl[1000][50] = { "" };

//保留字表 种别标号3-16
static char reserveWord[14][20] = {
    "CREATE","DROP","TABLE",
    "INSERT","DELETE","UPDATE","SELECT",
    "INTO","VALUES","FROM","WHERE","SET",
    "AND","OR"
};
//支持小写
static char reserveWord_lower[14][20] = {
    "create","drop","table",
    "insert","delete","update","select",
    "into","values","from","where","set",
    "and","or"
};
//符号表 种别标号17-30
static char operatorOrDelimiter[14][10] = {
    "=","<>","!=","<=","<",">=",">",
    "*","(",")",",",";","\'","\""
};

//查找保留字
int searchReserve(char reserveWord[][20], char s[]){
    for(int i=0; i<14; i++){
        if(strcmp(reserveWord[i], s)==0 || strcmp(reserveWord_lower[i], s)==0){
            return i+3;//若查找成功，返回种别码
        }
    }
    return -1;//若没有找到，返回-1
}
//查找符号
int searchSymbol(char operatorOrDelimiter[][10], char s[]){
    for(int i=0; i<14; i++){
        if(strcmp(s, operatorOrDelimiter[i]) == 0){
            return i+17;//获得种别码
        }
    }
    return -1;
}

//判断是否为字母
bool IsLetter(char letter){
    if ((letter >= 'a'&&letter <= 'z') || (letter >= 'A'&&letter <= 'Z'))// || letter=='_'
        return true;
    else return false;
}
//判断是否为数字
bool IsDigit(char digit){
    if(digit >= '0'&&digit <= '9')
        return true;
    else return false;
}
//判断是否为数字Token
bool IsNumber(char *token){
    for(int i=0; i<strlen(token); i++){
        if((int)token[i]<'0'||(int)token[i]>'9') return false;
    }
    return true;
}
// 判断是否为自定义标识符 语法判断工具函数
bool IsId(char *id){
    for(int i = 0; i<1000; i++){
        // 已在表中
        if(strcmp(IDentifierTbl[i], id) == 0) return true;
        // 查找空间
        if(strcmp(IDentifierTbl[i], "") == 0){
            return false;
        }
    }
    return false;
}

#endif