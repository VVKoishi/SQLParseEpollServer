#include "syntax.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/writer.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sstream>
#include <vector>

#define MAXCONN 4096
#define p(x) sem_wait(&x)
#define v(x) sem_post(&x)
using namespace rapidjson;
using namespace std;

Document d;//DOM根
sem_t sem;

void commit(){
    //文件写入
    FILE* fp2 = fopen("soft_base_sql.json", "w");
    char writeBuffer[65536];
    FileWriteStream os(fp2, writeBuffer, sizeof(writeBuffer));
    Writer<FileWriteStream> writer(os);
    d.Accept(writer);
    fclose(fp2);
}
void parse(char *buff){
    //将buff存入sql文件
    FILE* fp_sql = fopen("temp.sql", "w+");
    fprintf(fp_sql, buff);
    fclose(fp_sql);

    //开始词法解析和文法解析
    LexMain();
    ParseMain();

}

string num2str(int i)
{
    stringstream ss;
    ss<<i;
    return ss.str();
}

//查找unit是否在list中
bool searchList(const char *unit, const char columns_list[][20], int num_columns){
    for(int i=0; i<num_columns; i++){
        if(strcmp(columns_list[i], unit)==0){
            return true;
        }
    }
    return false;
}




//表查询
void my_select(int fd, const char *table_name){
    string str;
    for (auto& x : d.GetArray()){
        //匹配对应的TABLE
        if (strcmp( x["type"].GetString(), "table")==0 && strcmp( x["name"].GetString(), table_name)==0) {
            //TABLE中DATA的轮询
            for (auto & y : x["data"].GetArray()){
                //输出流
                for (auto& z : y.GetObject()){
                    for (auto& c : (string)z.name.GetString()) str.push_back(c);
                    str.push_back(':');
                    str.push_back(' ');
                    for (auto& c : (string)z.value.GetString()) str.push_back(c);
                    str.push_back('\t');
                } 
                // printf("\n");
                str.push_back('\n');
            }
           
        }
    }
    const char *mail = str.c_str();
    // printf("%s", mail);
    write(fd, mail, strlen(mail));
    return;
}
//点查询 未实现ANDOR
void my_select(int fd, const char *table_name, const char *where_column, const char *where_value){
    string str;
    for (auto& x : d.GetArray()){
        //匹配对应的TABLE
        if (strcmp( x["type"].GetString(), "table")==0 && strcmp( x["name"].GetString(), table_name)==0) {
            //TABLE中DATA的轮询
            for (auto & y : x["data"].GetArray()){
                //每一行数据中每个数据轮询
                for (auto& z : y.GetObject()){
                    //匹配COLUMN
                    if (strcmp( z.name.GetString(), where_column)==0 && strcmp(z.value.GetString(), where_value)==0) {
                        //匹配成功，输出该行
                        for (auto& m : y.GetObject()){
                            // printf("%s: %d\t", m.name.GetString(), m.value.GetInt());
                            for (auto& c : (string)m.name.GetString()) str.push_back(c);
                            str.push_back(':');
                            str.push_back(' ');
                            for (auto& c : (string)m.value.GetString()) str.push_back(c);
                            str.push_back('\t');
                        }
                        // printf("\n");
                        str.push_back('\n');
                    }
                }
            }
        }
    }
    const char *mail = str.c_str();
    write(fd, mail, strlen(mail));
    return;
}
//表查询
void my_select(int fd, const char *table_name, const char columns_list[][20], int num_columns){
    string str;
    for (auto& x : d.GetArray()){
        //匹配对应的TABLE
        if (strcmp( x["type"].GetString(), "table")==0 && strcmp( x["name"].GetString(), table_name)==0) {
            //TABLE中DATA的轮询
            for (auto & y : x["data"].GetArray()){
                //输出流
                for (auto& z : y.GetObject()){
                    // printf("%s: %d\t", z.name.GetString(), z.value.GetInt());
                    //若不在视图中，则跳过
                    if(!searchList(z.name.GetString(), columns_list, num_columns)) continue;
                    for (auto& c : (string)z.name.GetString()) str.push_back(c);
                    str.push_back(':');
                    str.push_back(' ');
                    for (auto& c : (string)z.value.GetString()) str.push_back(c);
                    str.push_back('\t');
                } 
                str.push_back('\n');
            }
        }
    }
    const char *mail = str.c_str();
    write(fd, mail, strlen(mail));
    return;
}
//点查询 未实现ANDOR
void my_select(int fd, const char *table_name, const char columns_list[][20], int num_columns, const char *where_column, const char *where_value){
    string str;
    for (auto& x : d.GetArray()){
        //匹配对应的TABLE
        if (strcmp( x["type"].GetString(), "table")==0 && strcmp( x["name"].GetString(), table_name)==0) {
            //TABLE中DATA的轮询
            for (auto & y : x["data"].GetArray()){
                //每一行数据中每个数据轮询
                for (auto& z : y.GetObject()){
                    //匹配COLUMN
                    if (strcmp( z.name.GetString(), where_column)==0 && strcmp(z.value.GetString(), where_value)==0) {
                        //匹配成功，输出该行
                        for (auto& m : y.GetObject()){
                            // printf("%s: %d\t", m.name.GetString(), m.value.GetInt());
                            //若不在视图中，则跳过
                            if(!searchList(m.name.GetString(), columns_list, num_columns)) continue;
                            for (auto& c : (string)m.name.GetString()) str.push_back(c);
                            str.push_back(':');
                            str.push_back(' ');
                            for (auto& c : (string)m.value.GetString()) str.push_back(c);
                            str.push_back('\t');
                        }
                        // printf("\n");
                        str.push_back('\n');
                    }
                }
            }
        }
    }
    const char *mail = str.c_str();
    // printf("%s", mail);
    write(fd, mail, strlen(mail));
    return;
}
//行插入 未实现id值唯一等条件
void my_insert(int fd, const char *table_name, const char values_list[][20], int num_values){
    for (auto& x : d.GetArray()){
        //匹配对应的TABLE
        if (strcmp( x["type"].GetString(), "table")==0 && strcmp( x["name"].GetString(), table_name)==0) {
            //TABLE中插入DATA
            Value item(kObjectType);
            char *columns_list[20];
            int num_columns=0;
            
            for (auto& y : x["columns"].GetArray()){
                // printf("%s\n", string(y.GetString(), y.GetStringLength()).c_str());
                // columns_list[num_columns] = string(y.GetString(), y.GetStringLength()).c_str();
                ++num_columns;
            }

            if(num_columns!=num_values) {
                char mail[50] = "Wrong number of values.\n";
                write(fd, mail, sizeof(mail));
                return;
            }
            
            // write(fd, columns_list[0], sizeof(columns_list[0]));

            int i=0;
            for (auto& y : x["columns"].GetArray()){
                item.AddMember(y, values_list[i], d.GetAllocator());
                i++;
            }
            
            x["data"].PushBack(item, d.GetAllocator());
            char mail[50] = "Finished.\n";
            write(fd, mail, sizeof(mail));
            return;
        }
    }
    return;
}
//行插入 未实现id值唯一等条件
void my_insert(const char *table_name, const char columns_list[][20], const char values_list[][20], int num_columns){
    for (auto& x : d.GetArray()){
        //匹配对应的TABLE
        if (strcmp( x["type"].GetString(), "table")==0 && strcmp( x["name"].GetString(), table_name)==0) {
            //TABLE中插入DATA
            Value item(kObjectType);
            
            for(int i=0; i<num_columns; i++){
                item.AddMember(columns_list[i], values_list[i], d.GetAllocator());
            }
            
            x["data"].PushBack(item, d.GetAllocator());
            return;
        }
    }
    return;
}
//点修改 修改所有匹配到的值的行的点
void my_update(const char *table, const char *where_column, const char *where_value, const char change_columns[][20], const char change_values[][20], int num_columns){
    for (auto& x : d.GetArray()){
        //匹配对应的TABLE
        if (strcmp( x["type"].GetString(), "table")==0 && strcmp( x["name"].GetString(), table)==0) {
            //TABLE中DATA的轮询
            Document::ConstValueIterator pos=x["data"].Begin();
            for (auto & y : x["data"].GetArray()){
                //每一行数据中每个数据轮询
                for (auto& z : y.GetObject()){
                    //匹配COLUMN
                    if (strcmp( z.name.GetString(), where_column)==0 && strcmp(z.value.GetString(), where_value)==0) {
                        //匹配成功，修改该行
                        // y[change_column] = change_val;
                        for(int i=0; i<num_columns; i++){
                            y[change_columns[i]].SetString(change_values[i], strlen(change_values[i]), d.GetAllocator());
                        }

                    }
                }
                pos++;
            }
        }
    }
    return;
}
//点删除 删除所有匹配到的值的行
void my_delete(const char *table, const char *where_column, const char *where_value){
    for (auto& x : d.GetArray()){
        //匹配对应的TABLE
        if (strcmp( x["type"].GetString(), "table")==0 && strcmp( x["name"].GetString(), table)==0) {
            //TABLE中DATA的轮询
            Document::ConstValueIterator pos=x["data"].Begin();
            for (auto & y : x["data"].GetArray()){
                //每一行数据中每个数据轮询
                for (auto& z : y.GetObject()){
                    //匹配COLUMN
                    if (strcmp( z.name.GetString(), where_column)==0 && strcmp(z.value.GetString(),where_value)==0) {
                        //匹配成功，删除该行
                        x["data"].Erase(pos);
                        pos--;//迭代器回退
                    }
                }
                pos++;
            }
        }
    }
    return;
}

void ActionMain(int fd){
    FILE *fp_err, *fp_dys, *fp_err2;

    //错误返回
    fp_err = fopen("temp.err", "r");
    char errInfo[100][100]={0};
    int ith=0;
    while(!feof(fp_err)){
        fgets(errInfo[ith++], 100, fp_err);
    }
    fclose(fp_err);
    fp_err2 = fopen("temp.err", "w");//清空已读取
    fclose(fp_err2);
    if(strcmp(errInfo[0], "")!=0) {
        ith = 0;
        while(strcmp(errInfo[ith], "")!=0){
            write(fd, errInfo[ith], 100);
            ith++;
        }
        return;//***
    }

    //读取dys
    fp_dys = fopen("temp.dys", "r");
    char dysInfo[1000][20];
    int dindex=0;
    while(!feof(fp_dys)){
        fgets(dysInfo[dindex], 20, fp_dys);
        strtok(dysInfo[dindex], "\n");
        dindex++;
    }
    fclose(fp_dys);
    int maxindex = dindex;
    dindex=0;
    //分支
    while(dindex<maxindex-1){//-1是因为最后多了一行回车
        if(strcmp(dysInfo[dindex], "SELECT")==0){
            dindex++;//select
            if(strcmp(dysInfo[dindex], "*")==0){
                dindex++;//*
                dindex++;//from
                const char *table_name = dysInfo[dindex++];
                if(strcmp(dysInfo[dindex], ";")==0) {
                    dindex++;//;
                    my_select(fd, table_name);
                    continue;
                }
                else{
                    dindex++; //where
                    const char *where_column = dysInfo[dindex++];
                    dindex++; //=
                    const char *where_values = dysInfo[dindex++];
                    dindex++; //;
                    my_select(fd, table_name, where_column, where_values);
                    continue;
                }
            }
            else {
                char columns_list[20][20];
                int num_columns=0;
                while(strcmp(dysInfo[dindex], "FROM")!=0){
                    strcpy(columns_list[num_columns], dysInfo[dindex]);
                    dindex++;
                    if(strcmp(dysInfo[dindex], ",")==0) dindex++;//,
                    num_columns++;
                }
                dindex++;//from
                const char *table_name = dysInfo[dindex++];
                if(strcmp(dysInfo[dindex], ";")==0) {
                    dindex++;
                    my_select(fd, table_name, columns_list, num_columns);
                    continue;
                }
                else{
                    dindex++; //where
                    const char *where_column = dysInfo[dindex++];
                    dindex++; //=
                    const char *where_values = dysInfo[dindex++];
                    dindex++; //;
                    my_select(fd, table_name, columns_list, num_columns, where_column, where_values);
                    continue;
                }
            }
        }
        else if(strcmp(dysInfo[dindex], "INSERT")==0){
            dindex++;//insert
            dindex++;//into
            const char *table_name = dysInfo[dindex++];
            if(strcmp(dysInfo[dindex], "VALUES")==0){
                dindex++;//values
                dindex++;//(
                char values_list[20][20];
                int num_values=0;
                while(strcmp(dysInfo[dindex], ")")!=0){
                    strcpy(values_list[num_values], dysInfo[dindex]);
                    dindex++;
                    if(strcmp(dysInfo[dindex], ",")==0) dindex++;//,
                    num_values++;
                }
                dindex++;//)
                dindex++;//;
                my_insert(fd, table_name, values_list, num_values);
                continue;
            }
            else;//插部分值没有写，这样插值太过危险
        }
        else if(strcmp(dysInfo[dindex], "UPDATE")==0){
            dindex++;//update
            const char *table_name = dysInfo[dindex++];
            dindex++;//set
            char change_columns[20][20];
            char change_values[20][20];
            int num_columns=0;
            while(strcmp(dysInfo[dindex], "WHERE")!=0){
                strcpy(change_columns[num_columns], dysInfo[dindex]);
                dindex++;
                dindex++;//=
                strcpy(change_values[num_columns], dysInfo[dindex]);
                dindex++;
                if(strcmp(dysInfo[dindex], ",")==0) dindex++;//,
                num_columns++;
            }
            dindex++;//where
            const char *where_column = dysInfo[dindex++];
            dindex++;//=
            const char *where_value = dysInfo[dindex++];
            dindex++;//;
            my_update(table_name, where_column, where_value, change_columns, change_values, num_columns);
            continue;
        }
        else if(strcmp(dysInfo[dindex], "DELETE")==0){}
        else if(strcmp(dysInfo[dindex], "CREATE")==0){}
        else if(strcmp(dysInfo[dindex], "DROP")==0){}
        // printf("%d,%d",dindex,maxindex);
    }
    

    // write(fd, "ok\n", 3);


    commit();
}

//接收回传
int do_use_fd(int fd)
{
    int n;
    char buff[1024];//缓冲区
    
    bzero(buff, sizeof(buff));
    n = read(fd, buff, 1024);//读缓冲区
    if(n == -1)
    {
        if(errno != EAGAIN)
        {
            fprintf(stderr,"read error:%s\n",strerror(errno));
            close(fd);
            return 1;
        }
    }
    else if(n == 0)
        close(fd);
    else
    {
        p(sem);
        parse(buff);
        ActionMain(fd);
        v(sem);
        // if(buff[0]=='a')
        // {
        //     p(sem);
        //     const char *table_name = "items";
        //     const char columns_list[2][20] = {"id","policy_id"};
        //     int num_columns = 2;
        //     my_select(fd, table_name, columns_list, num_columns);
        //     v(sem);
        // }
        // else if(buff[0]=='b')
        // {
        //     p(sem);
        //     const char *table_name = "items";
        //     char columns_list[3][20] = {"temp","all","heyhey"};
        //     char values_list[3][20] = {"ok","12","ma"};
        //     int num_columns = 3;
        //     my_insert(table_name, columns_list, values_list, num_columns);
        //     write(fd, "Finished.\n",10);
        //     v(sem);
        // }
        // else if(buff[0]=='c')
        // {
        //     p(sem);
        //     // my_update(&d, "items", "id", 3, "policy_id", 5);
        //     v(sem);
        // }
        // else if(buff[0]=='d')
        // {
        //     p(sem);
        //     // my_delete(&d, "items", "id", 1);
        //     v(sem);
        // }

    }    
    return 0;
}




int main(){
    sem_init(&sem, 0, 1);
    
    //文件读取
    FILE* fp = fopen("soft_base_sql.json", "r");
    char readBuffer[65536];
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    d.ParseStream(is);
    fclose(fp);
    
    
    
    
    //epoll
    struct sockaddr_in addr;
    int listenfd;
    int epollfd;
    int nfds;
    int i;
    struct epoll_event pev[MAXCONN], ev;
    
    bzero(pev, sizeof(pev));
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd < 0)
    {
        fprintf(stderr,"create socket error:%s\n",strerror(errno));
        return -1;
    }
    if(bind(listenfd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        fprintf(stderr,"bind error:%s\n",strerror(errno));
        return -1;
    }
    if(listen(listenfd, 100) == -1)
    {
        fprintf(stderr,"listen error:%s\n",strerror(errno));
        return -1;
    }
    
    epollfd = epoll_create(MAXCONN);
    if(epollfd == -1)
    {
        fprintf(stderr,"epoll create error:%s\n",strerror(errno));
        return -1;
    }
    ev.events = EPOLLIN;
    ev.data.fd = listenfd;
    if(epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev) < 0)
    {
        fprintf(stderr,"epoll ctl error:%s\n",strerror(errno));
        return -1;
    }

    while(1)
    {
        nfds = epoll_wait(epollfd, pev, MAXCONN, -1);
        for(i=0; i < nfds; i++)
        {
            if(pev[i].data.fd == listenfd)
            {
                int connd;
                struct sockaddr_in remoteAddr;
                socklen_t remoteAddrLen;
                
                remoteAddrLen = sizeof(remoteAddr);
                connd = accept(listenfd, (struct sockaddr*)&remoteAddr, &remoteAddrLen);
                if(connd == -1)
                {
                    if(errno == EWOULDBLOCK || errno == EINTR || errno == ECONNABORTED)
                        continue;
                    fprintf(stderr,"accept error:%s\n",strerror(errno));
                    return -1;
                }
                ev.data.fd = connd;
                ev.events = EPOLLIN; //| EPOLLET;
                if(epoll_ctl(epollfd, EPOLL_CTL_ADD, connd, &ev)==-1)
                {
                    fprintf(stderr,"epoll control error:%s\n",strerror(errno));
                    return -1;
                }
            }
            else if(pev[i].events & (EPOLLHUP | EPOLLERR))
                close(pev[i].data.fd);
            else if(pev[i].events & EPOLLIN)
                do_use_fd(pev[i].data.fd);
            else
                fprintf(stdout,"%d  %d\n",pev[i].data.fd,pev[i].events);
        }
    }
//    my_insert(&d, "items");
//    my_insert(&d, "items");
//    my_insert(&d, "items");
//    my_select(&d, "items");
//    my_insert(&d, "items");
//    my_update(&d, "items", "id", 2, "policy_id", 4);
//    my_select(&d, "items");
//    my_delete(&d, "items", "id", 1);
//    my_select(&d, "items", "id", 1);
//    my_select(&d, "edges", "id", 0);
    
    
    
    
    
    
    
    return 0;
}
