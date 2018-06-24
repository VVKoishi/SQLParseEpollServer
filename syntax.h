#ifndef syntax_h
#define syntax_h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
using namespace std;

#include "symbol.h"



// 错误处理函数
void error(FILE *fp_err, int *line){
    fprintf(fp_err, "***LINE:%d  Syntax error.\n", *line);
}

// 收集
void collect(int *sym, char dydToken[][20], int *cindex, char *collection){
    for(int i=0; i<20; i++){
        if(dydToken[*sym][i]=='\0') break;
        collection[*cindex] = dydToken[*sym][i];
        ++*cindex;
    }
    collection[*cindex] = '\n';
    ++*cindex;
}

// 过程advance:匹配并把输入串指针后移一位
void advance(const char *t, int *sym, char dydToken[][20], int *cindex, char *collection){
    collect(sym, dydToken, cindex, collection);
    // printf("%s", dydToken[*sym]);调试用
    if(strcmp(dydToken[*sym], t)==0) ++*sym;
    else printf("Judgement error\n");
}



// 语法递归函数
// column,column,...,column
bool COLUMN_LIST(int *sym, char dydToken[][20], FILE *fp_err, int *line, char *collection, int *cindex){
    if(IsId(dydToken[*sym])){
        advance(dydToken[*sym], sym, dydToken, cindex, collection);
        if(strcmp(dydToken[*sym],",")==0){
            advance(",", sym, dydToken, cindex, collection);
            if(!COLUMN_LIST(sym, dydToken, fp_err, line, collection, cindex)) return false;
        }
        return true;
    }
    else error(fp_err, line);
    return false;
}
// 12,"a",'b',...
bool VALUES_LIST(int *sym, char dydToken[][20], FILE *fp_err, int *line, char *collection, int *cindex){
    if(IsNumber(dydToken[*sym]) || strcmp(dydToken[*sym], "\"")==0 || strcmp(dydToken[*sym], "\'")==0){
        if(IsNumber(dydToken[*sym])){
            advance(dydToken[*sym], sym, dydToken, cindex, collection);
            if(strcmp(dydToken[*sym],",")==0){
                advance(",", sym, dydToken, cindex, collection);
                if(!VALUES_LIST(sym, dydToken, fp_err, line, collection, cindex)) return false;
            }
            return true;
        }
        else{
            bool singlequote=false;
            bool doublequote=false;
            if(strcmp(dydToken[*sym],"\'")==0){
                singlequote=true;
                advance("\'", sym, dydToken, cindex, collection);
            }
            else if(strcmp(dydToken[*sym], "\"")==0){
                doublequote=true;
                advance("\"", sym, dydToken, cindex, collection);
            }
            else error(fp_err, line);
            if(IsId(dydToken[*sym]) || IsNumber(dydToken[*sym])){
                advance(dydToken[*sym], sym, dydToken, cindex, collection);
                if(singlequote || doublequote){
                    if(singlequote && strcmp(dydToken[*sym],"\'")==0) advance("\'", sym, dydToken, cindex, collection);
                    else if(doublequote && strcmp(dydToken[*sym],"\"")==0) advance("\"", sym, dydToken, cindex, collection);
                    else error(fp_err, line);
                }
                if(strcmp(dydToken[*sym],",")==0){
                    advance(",", sym, dydToken, cindex, collection);
                    if(!VALUES_LIST(sym, dydToken, fp_err, line, collection, cindex)) return false;
                }
                return true;
            }
            else error(fp_err, line);
        }
    }
    else error(fp_err, line);
    return false;
}
// column=0,column='a',...column="b"
bool COLUMN_SET_LIST(int *sym, char dydToken[][20], FILE *fp_err, int *line, char *collection, int *cindex){
    if(IsId(dydToken[*sym])){
        advance(dydToken[*sym], sym, dydToken, cindex, collection);
        if(strcmp(dydToken[*sym],"=")==0){
            bool singlequote=false;
            bool doublequote=false;
            advance("=", sym, dydToken, cindex, collection);
            if(strcmp(dydToken[*sym],"\'")==0){
                singlequote=true;
                advance("\'", sym, dydToken, cindex, collection);
            }
            else if(strcmp(dydToken[*sym],"\"")==0){
                doublequote=true;
                advance("\"", sym, dydToken, cindex, collection);
            }
            if(IsId(dydToken[*sym]) || IsNumber(dydToken[*sym])){
                advance(dydToken[*sym], sym, dydToken, cindex, collection);
                if(singlequote || doublequote){
                    if(singlequote && strcmp(dydToken[*sym],"\'")==0) advance("\'", sym, dydToken, cindex, collection);
                    else if(doublequote && strcmp(dydToken[*sym],"\"")==0) advance("\"", sym, dydToken, cindex, collection);
                    else error(fp_err, line);
                }
                if(strcmp(dydToken[*sym],",")==0){
                    advance(",", sym, dydToken, cindex, collection);
                    if(!COLUMN_SET_LIST(sym, dydToken, fp_err, line, collection, cindex)) return false;
                }
                return true;
            }
            else error(fp_err, line);
        }
        else error(fp_err, line);
    }
    else error(fp_err, line);
    return false;
}
// column=0 AND column='a' OR...AND column="b"
bool COLUMN_ANDOR_LIST(int *sym, char dydToken[][20], FILE *fp_err, int *line, char *collection, int *cindex){
    if(IsId(dydToken[*sym])){
        advance(dydToken[*sym], sym, dydToken, cindex, collection);
        if(strcmp(dydToken[*sym],"=")==0){
            bool singlequote=false;
            bool doublequote=false;
            advance("=", sym, dydToken, cindex, collection);
            if(strcmp(dydToken[*sym],"\'")==0){
                singlequote=true;
                advance("\'", sym, dydToken, cindex, collection);
            }
            else if(strcmp(dydToken[*sym],"\"")==0){
                doublequote=true;
                advance("\"", sym, dydToken, cindex, collection);
            }
            if(IsId(dydToken[*sym]) || IsNumber(dydToken[*sym])){
                advance(dydToken[*sym], sym, dydToken, cindex, collection);
                if(singlequote || doublequote){
                    if(singlequote && strcmp(dydToken[*sym],"\'")==0) advance("\'", sym, dydToken, cindex, collection);
                    else if(doublequote && strcmp(dydToken[*sym],"\"")==0) advance("\"", sym, dydToken, cindex, collection);
                    else error(fp_err, line);
                }
                if(strcmp(dydToken[*sym],"AND")==0 || strcmp(dydToken[*sym],"OR")==0){
                    if(strcmp(dydToken[*sym],"AND")==0) {
                        advance("AND", sym, dydToken, cindex, collection);
                    }
                    else if(strcmp(dydToken[*sym],"OR")==0){
                        advance("OR", sym, dydToken, cindex, collection);
                    }
                    if(!COLUMN_ANDOR_LIST(sym, dydToken, fp_err, line, collection, cindex)) return false;
                }
                return true;
            }
            else error(fp_err, line);
        }
        else error(fp_err, line);
    }
    else error(fp_err, line);
    return false;
}

// WHERE->
bool WHERE(int *sym, char dydToken[][20], FILE *fp_err, int *line, char *collection, int *cindex){
    if(strcmp(dydToken[*sym],"WHERE")==0){
        advance("WHERE", sym, dydToken, cindex, collection);
        if(COLUMN_ANDOR_LIST(sym, dydToken, fp_err, line, collection, cindex)) return true;
    }
    else error(fp_err, line);
    return false;
}
// SELECT->
bool SELECT(int *sym, char dydToken[][20], FILE *fp_err, int *line, char *collection, int *cindex){
    advance("SELECT", sym, dydToken, cindex, collection);
    if(strcmp(dydToken[*sym],"*")==0 || IsId(dydToken[*sym])){
        if(strcmp(dydToken[*sym],"*")==0) {
            advance("*", sym, dydToken, cindex, collection);
        }
        else if(IsId(dydToken[*sym])){
            if(!COLUMN_LIST(sym, dydToken, fp_err, line, collection, cindex)) return false;
        }
        else error(fp_err, line);
        if(strcmp(dydToken[*sym],"FROM")==0){
            advance("FROM", sym, dydToken, cindex, collection);
            if(IsId(dydToken[*sym])) {
                advance(dydToken[*sym], sym, dydToken, cindex, collection);
                if(strcmp(dydToken[*sym],"WHERE")==0) {
                    if(!WHERE(sym, dydToken, fp_err, line, collection, cindex)) return false;
                }
                if(strcmp(dydToken[*sym],";")==0) {
                    advance(";", sym, dydToken, cindex, collection);
                    return true;
                }
                else error(fp_err, line);
            }
            else error(fp_err, line);
        }
        else error(fp_err, line);
    }
    else error(fp_err, line);
    return false;
}
// INSERT->
bool INSERT(int *sym, char dydToken[][20], FILE *fp_err, int *line, char *collection, int *cindex){
    advance("INSERT", sym, dydToken, cindex, collection);
    if(strcmp(dydToken[*sym], "INTO")==0){
        advance("INTO", sym, dydToken, cindex, collection);
        if(IsId(dydToken[*sym])){
            advance(dydToken[*sym], sym, dydToken, cindex, collection);
            if(strcmp(dydToken[*sym], "VALUES")==0){
                advance("VALUES", sym, dydToken, cindex, collection);
                if(strcmp(dydToken[*sym], "(")==0){
                    advance("(", sym, dydToken, cindex, collection);
                    if(IsNumber(dydToken[*sym]) || strcmp(dydToken[*sym], "\"")==0 || strcmp(dydToken[*sym], "\'")==0){
                        if(VALUES_LIST(sym, dydToken, fp_err, line, collection, cindex)){
                            if(strcmp(dydToken[*sym], ")")==0){
                                advance(")", sym, dydToken, cindex, collection);
                                if(strcmp(dydToken[*sym], ";")==0) {
                                    advance(";", sym, dydToken, cindex, collection);
                                    return true;
                                }
                                else error(fp_err, line);
                            }
                            else error(fp_err, line);
                        }
                        else error(fp_err, line); 
                    }
                    else error(fp_err, line);
                }
                else error(fp_err, line);
            }
            else error(fp_err, line);
        }
        else error(fp_err, line);
    }
    else error(fp_err, line);
    return false;
}
// UPDATE->
bool UPDATE(int *sym, char dydToken[][20], FILE *fp_err, int *line, char *collection, int *cindex){
    advance("UPDATE", sym, dydToken, cindex, collection);
    if(IsId(dydToken[*sym])){
        advance(dydToken[*sym], sym, dydToken, cindex, collection);
        if(strcmp(dydToken[*sym], "SET")==0){
            advance("SET", sym, dydToken, cindex, collection);
            if(!COLUMN_SET_LIST(sym, dydToken, fp_err, line, collection, cindex)) return false;
            if(!WHERE(sym, dydToken, fp_err, line, collection, cindex)) return false;
            if(strcmp(dydToken[*sym], ";")==0) {
                advance(";", sym, dydToken, cindex, collection);
                return true;
            }
            else error(fp_err, line);
        }
        else error(fp_err, line);
    }
    else error(fp_err, line);
    return false;
}
// DELETE->
bool DELETE(int *sym, char dydToken[][20], FILE *fp_err, int *line, char *collection, int *cindex){
    advance("DELETE", sym, dydToken, cindex, collection);
    if(strcmp(dydToken[*sym],"FROM")==0){
        advance("FROM", sym, dydToken, cindex, collection);
        if(IsId(dydToken[*sym])){
            advance(dydToken[*sym], sym, dydToken, cindex, collection);
            if(strcmp(dydToken[*sym],"WHERE")==0) WHERE(sym, dydToken, fp_err, line, collection, cindex);
            else error(fp_err, line);
            if(strcmp(dydToken[*sym],";")==0) {
                advance(";", sym, dydToken, cindex, collection);
                return true;
            }
            else error(fp_err, line);
        }
        else error(fp_err, line);
    }
    else error(fp_err, line);
    return false;
}
// CREATE->
bool CREATE(int *sym, char dydToken[][20], FILE *fp_err, int *line, char *collection, int *cindex){
    advance("CREATE", sym, dydToken, cindex, collection);
    if(strcmp(dydToken[*sym],"TABLE")==0){
        advance("TABLE", sym, dydToken, cindex, collection);
        if(IsId(dydToken[*sym])){
            advance(dydToken[*sym], sym, dydToken, cindex, collection);
            if(strcmp(dydToken[*sym], "(")==0){
                advance("(", sym, dydToken, cindex, collection);
                if(IsId(dydToken[*sym])){
                    if(!COLUMN_LIST(sym, dydToken, fp_err, line, collection, cindex)) return false;
                    if(strcmp(dydToken[*sym], ")")==0){
                        advance(")", sym, dydToken, cindex, collection);
                        if(strcmp(dydToken[*sym], ";")==0) {
                            advance(";", sym, dydToken, cindex, collection);
                            return true;
                        }
                        else error(fp_err, line);
                    }
                    else error(fp_err, line);
                }
                else error(fp_err, line);
            }
            else error(fp_err, line);
        }
        else error(fp_err, line);
    }
    else error(fp_err, line);
    return false;
}
// DROP
bool DROP(int *sym, char dydToken[][20], FILE *fp_err, int *line, char *collection, int *cindex){
    advance("DROP", sym, dydToken, cindex, collection);
    if(strcmp(dydToken[*sym],"TABLE")==0){
        advance("TABLE", sym, dydToken, cindex, collection);
        if(IsId(dydToken[*sym])){
            advance(dydToken[*sym], sym, dydToken, cindex, collection);
            if(strcmp(dydToken[*sym], ";")==0) {
                advance(";", sym, dydToken, cindex, collection);
                return true;
            }
            else error(fp_err, line);
        }
        else error(fp_err, line);
    }
    else error(fp_err, line);
    return false;
}

void jump2other(int *sym, char dydToken[][20]){
    while(strcmp(dydToken[*sym], "EOF")!=0){
        if (strcmp(dydToken[*sym], "EOLN")==0){
            break;
        }
        else if (strcmp(dydToken[*sym], ";")==0){
            ++*sym;
            break;
        }
        ++*sym;
    }
}


void ParseMain(){
    char dydToken[10000][20];
    int sym = 0; // 变量sym:输入指针指向的符号
    int line = 1;
    char collection[200]; //合法指令收集
    int cindex = 0;
    FILE *fp, *fp_err, *fp_dys;

    // 打开dyd文件
    if((fp = fopen("temp.dyd", "r")) == NULL){
        printf("Can't open temp.dyd\n");
        exit(0);
    }
    // 打开错误文件
    if((fp_err = fopen("temp.err", "a+")) == NULL){
        printf("Can't open temp.err\n");
        exit(0);
    }
    // 打开dys文件
    if((fp_dys = fopen("temp.dys", "w+")) == NULL){
        printf("Can't open temp.dys\n");
        exit(0);
    }

    // 读取二元组内容
    while(!feof(fp)){
        fgets(dydToken[sym], 20, fp);
        strtok(dydToken[sym], " ");
        sym++;
    }
    fclose(fp);


    sym = 0;// 初始化指针

    // 扫描
    while(strcmp(dydToken[sym], "EOF")!=0){
        bzero(collection, 200);
        cindex = 0;
        //换行
        if (strcmp(dydToken[sym], "EOLN")==0){
            line++;
            sym++;
        }
        //调用SELECT文法
        else if(strcmp(dydToken[sym], "SELECT")==0){
            if(!SELECT(&sym, dydToken, fp_err, &line, collection, &cindex)) jump2other(&sym, dydToken);
            else fprintf(fp_dys, collection);
        }
        //调用INSERT文法
        else if(strcmp(dydToken[sym], "INSERT")==0){
            if(!INSERT(&sym, dydToken, fp_err, &line, collection, &cindex)) jump2other(&sym, dydToken);
            else fprintf(fp_dys, collection);
        }
        //调用UPDATE文法
        else if(strcmp(dydToken[sym], "UPDATE")==0){
            if(!UPDATE(&sym, dydToken, fp_err, &line, collection, &cindex)) jump2other(&sym, dydToken);
            else fprintf(fp_dys, collection);
        }
        //调用DELETE文法
        else if(strcmp(dydToken[sym], "DELETE")==0){
            if(!DELETE(&sym, dydToken, fp_err, &line, collection, &cindex)) jump2other(&sym, dydToken);
            else fprintf(fp_dys, collection);
        }
        //调用CREATE文法
        else if(strcmp(dydToken[sym], "CREATE")==0){
            if(!CREATE(&sym, dydToken, fp_err, &line, collection, &cindex)) jump2other(&sym, dydToken);
            else fprintf(fp_dys, collection);
        }
        //调用DROP文法
        else if(strcmp(dydToken[sym], "DROP")==0){
            if(!DROP(&sym, dydToken, fp_err, &line, collection, &cindex)) jump2other(&sym, dydToken);
            else fprintf(fp_dys, collection);
        }
        else {
            error(fp_err, &line);
            jump2other(&sym, dydToken);
        }
    }
    




    fclose(fp_err);
    fclose(fp_dys);
}








//词法分析子程序 依据状态转换图
void LexScanner(int *syn, char resourceProject[], char token[], int *pProject, int *line, FILE *fp_err){
    int i, count = 0;//count用来做token[]的指示器，收集有用字符
    char ch;//作为判断使用
    ch = resourceProject[*pProject];
    //过滤空格
    while(ch == ' ') ch = resourceProject[++*pProject];
    //token置空
    for(i=0; i<20; i++) token[i]='\0';
    
    //首字符为字母
    if(IsLetter(resourceProject[*pProject])){
        token[count++] = resourceProject[*pProject];//收集
        ++*pProject;//下移
        //后跟字母或数字或'_'
        while (IsLetter(resourceProject[*pProject]) || IsDigit(resourceProject[*pProject]) || resourceProject[*pProject]=='_'){
            token[count++] = resourceProject[*pProject];//收集
            ++*pProject;//下移
        }
        token[count] = '\0';
        *syn = searchReserve(reserveWord, token);//查表找到种别码
        //若不是保留字则是标识符
        if(*syn == -1){
            *syn = 2;//标识符种别码
        }
        return;
    }
    //首字符为数字
    else if(IsDigit(resourceProject[*pProject])){
        //后跟数字
        while (IsDigit(resourceProject[*pProject])){
            token[count++] = resourceProject[*pProject];//收集
            ++*pProject;
        }
        token[count] = '\0';
        *syn = 1;//常数种别码
        return;
    }
    //首字符为=*(),;'"
    else if(ch=='='||ch=='*'||ch=='('||ch==')'||ch==';'||ch==','||ch=='\''||ch=='\"'){
        token[0] = resourceProject[*pProject];
        token[1] = '\0';//形成单字符串
        //查运算符界符表
        *syn = searchSymbol(operatorOrDelimiter, token);//查表找到种别码
        ++*pProject;//指针下移，为下一扫描做准备
        return;
    }
    //首字符为<,<=,<>
    else if(resourceProject[*pProject] == '<'){
        token[count++] = resourceProject[*pProject];//收集
        ++*pProject;//后移，超前搜索
        if(resourceProject[*pProject] == '=') {
            token[count++] = resourceProject[*pProject];//收集
        }
        else if(resourceProject[*pProject] == '>') {
            token[count++] = resourceProject[*pProject];//收集
        }
        else {
            --*pProject;
        }
        *syn = searchSymbol(operatorOrDelimiter, token);//查表找到种别码
        ++*pProject;//指针下移
        return;
    }
    //首字符为>,>=
    else if(resourceProject[*pProject] == '>'){
        token[count++] = resourceProject[*pProject];//收集
        pProject++;
        if(resourceProject[*pProject] == '=') {
            token[count++] = resourceProject[*pProject];//收集
        }
        else {
            --*pProject;
        }
        *syn = searchSymbol(operatorOrDelimiter, token);//查表找到种别码
        ++*pProject;
        return;
    }
    //首字符为!=
    else if(resourceProject[*pProject] == '!'){
        token[count++] = resourceProject[*pProject];//收集
        ++*pProject;
        if(resourceProject[*pProject] == '=') {
            token[count++] = resourceProject[*pProject];//收集
            *syn = searchSymbol(operatorOrDelimiter, token);//查表找到种别码
        }
        else {
            --*pProject;
            fprintf(fp_err, "***LINE:%d  %c is illegal.\n", *line, ch);
            *syn = -1;//跳过此次syn
        }
        ++*pProject;
        return;
    }
    //回车符\n
    else if(resourceProject[*pProject] == '\n'){
        ++*line;
        ++*pProject;
        *syn = -1;
        return;
    }
    //归位键0d
    else if(resourceProject[*pProject] == 0x0d){
        ++*pProject;
        *syn = -1;
        return;
    }
    //结束符
    else if(resourceProject[*pProject] == '\0'){
        *syn = 0;
        return;
    }
    //不能被以上词法分析识别，则出错。
    else{
        fprintf(fp_err, "***LINE:%d  %c is illegal.\n", *line, ch);
        ++*pProject;
        *syn = -1;
        return;
    }
}

//词法分析主程序
void LexMain(){
    //打开一个文件，读取其中的源程序
    char resourceProject[10000];
    char token[20] = { 0 };
    int syn = -1, i;//初始化
    int pProject = 0;//源程序指针
    int preline = 1;
    int line = 1;//行号
    // char *filename = "temp";
    FILE *fp, *fp_dyd, *fp_err;
    
    //打开源程序
    if((fp = fopen("temp.sql", "r")) == NULL){
        printf("Can't open temp.sql\n");
        exit(0);
    }
    //将源程序读入resourceProject[]数组
    resourceProject[pProject] = fgetc(fp);
    while (resourceProject[pProject] != EOF) resourceProject[++pProject] = fgetc(fp);
    resourceProject[pProject] = '\0';
    fclose(fp);
    
    
    pProject = 0;//从头开始读
    //打开目标文件
    if((fp_dyd = fopen("temp.dyd", "w+")) == NULL){
        exit(0);
    }
    //打开错误文件
    if((fp_err = fopen("temp.err", "w+")) == NULL){
        exit(0);
    }
    
    
    while (syn != 0){
        
        //启动扫描
        LexScanner(&syn, resourceProject, token, &pProject, &line, fp_err);
        //如果扫描到标识符
        if(syn == 2){
            //插入标识符表中
            for(i = 0; i<1000; i++){
                //已在表中
                if(strcmp(IDentifierTbl[i], token) == 0) break;
                //查找空间
                if(strcmp(IDentifierTbl[i], "") == 0){
                    strcpy(IDentifierTbl[i], token);
                    break;
                }
            }
            
            fprintf(fp_dyd, "%s %d\n", token, syn);//打印
        }
        //保留字
        else if(syn >= 3 && syn <= 16){
            fprintf(fp_dyd, "%s %d\n", reserveWord[syn-3], syn);
        }
        //const 常数
        else if(syn == 1){
            fprintf(fp_dyd, "%s %d\n", token, syn);
        }
        //操作符和界符
        else if(syn >= 17 && syn <= 30){
            fprintf(fp_dyd, "%s %d\n", token, syn);
        }
        //如果行号变化
        if(preline!=line){
            fprintf(fp_dyd, "EOLN %d\n", preline);
            preline++;
        }
    }
    //文件结尾
    fprintf(fp_dyd, "EOF %d\n", line);
    
    
    fclose(fp_dyd);
    fclose(fp_err);

}


#endif