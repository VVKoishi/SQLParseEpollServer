//给定物流节点信息，物品信息，节点间代价计算方法要求学生完成以下实验内容。
//(1) 根据物品信息综合计算物流物品的优先级别，根据物流优先级别排序物流物品，根据排序结果对物流物品进行逐个发货。
//(2) 根据物流物品的物流条件信息，归类物流物品到物流方案类型，物流方案类型可包括：价格最小物流方案，时间最短物流方案、综合最优方案、航空物流方案等。并运用树型结构存储所有的物流物品到划分的物流方案中。
//(3) 根据给定的物流节点信息，计算各类物流方案下的物流最短路
//(4) 根据物流最短路径，物流方案和物流优先级发送货物
//输入：
//任意自定义物流节点信息；  任意自定义物流物品信息
//输出：
//物流物品信息表；根据优先级的物流物品排序结果；查询获得某类物流条件下所有的物品信息；每个物品的发送路径序列。
#ifndef path_h
#define path_h

#include <iostream>
#include <string>
#include <algorithm>
using namespace std;


struct Item{
    int item_id;//物品ID
    int level;//物品优先级
    int start;
    int end;
    int policy_id;//所属方案
    string path;
    int dis;
};
//物品
class Items{
public:
    int items_num;
    Item *list;//存储所有物品

    Items(int items_num, int node_num);
    ~Items();
};
Items::Items(int items_num, int node_num){
    this->items_num = items_num;
    this->list = new Item[items_num];
    cout << "请输入每个物品ID、优先级、发货地、目的地、所属方案(1.价格最小 2.时间最短 3.综合最优 4.航空物流)：" << endl;
    for(int i=0; i<items_num; i++){
        cin >> list[i].item_id >> list[i].level >> list[i].start >> list[i].end >> list[i].policy_id;
        while(list[i].start<=0||list[i].end<=0||list[i].policy_id<=0||list[i].start>node_num||list[i].end>node_num||list[i].policy_id>4){//数据检查
            cout << "输入错误，请重新输入该物品信息：" << endl;
            cin >> list[i].item_id >> list[i].level >> list[i].start >> list[i].end >> list[i].policy_id;
        }
    }
};
Items::~Items(){
    delete[] list;
};

//保存节点路径、距离、访问状态
struct Dis {
    string path;
    int value;
    bool visit;
    Dis() {
        visit = false;
        value = 0;
        path = "";
    }
};
//图
class Graph{
private:
    int node_num;//节点数
    int edge_num;//边数
    
    int **price;
    int **time;
    int **general;
    int **flight;
    Dis *dis_price;
    Dis *dis_time;
    Dis *dis_general;
    Dis *dis_flight;
public:
    Graph(int node_num, int edge_num);
    ~Graph();
    
    bool check_edge(int start, int end, int price_weight, int time_weight);
    void createGraph();
    void Dijkstra(Item *item);
};
Graph::Graph(int node_num, int edge_num){
    //初始化顶点数和边数
    this->node_num = node_num;
    this->edge_num = edge_num;
    //为邻接矩阵开辟空间和赋初值
    price = new int*[this->node_num];
    dis_price = new Dis[this->node_num];
    time = new int*[this->node_num];
    dis_time = new Dis[this->node_num];
    general = new int*[this->node_num];
    dis_general = new Dis[this->node_num];
    flight = new int*[this->node_num];
    dis_flight = new Dis[this->node_num];
    for (int i=0; i<this->node_num; i++) {
        price[i] = new int[this->node_num];
        time[i] = new int[this->node_num];
        general[i] = new int[this->node_num];
        flight[i] = new int[this->node_num];
        for (int j = 0; j < this->node_num; j++) {
            //邻接矩阵初始化为无穷大
            price[i][j] = INT_MAX;
            time[i][j] = INT_MAX;
            general[i][j] = INT_MAX;
            flight[i][j] = INT_MAX;
        }
    }
};
Graph::~Graph(){
    delete[] dis_price;
    delete[] dis_time;
    delete[] dis_general;
    delete[] dis_flight;
    for (int i = 0; i < this->node_num; i++) {
        delete[] this->price[i];
        delete[] this->time[i];
        delete[] this->general[i];
        delete[] this->flight[i];
    }
    delete[] price;
    delete[] time;
    delete[] general;
    delete[] flight;
};
bool Graph::check_edge(int start, int end, int price_weight, int time_weight){
    if(start<1 || end<1 || start>node_num || end>node_num || price_weight<0 || time_weight<0){
        return false;
    }
    return true;
};
void Graph::createGraph(){
    cout << "请输入每条边的起点、终点、运费、时间（顶点编号从1开始）：" << endl;
    int start;
    int end;
    int price_weight;
    int time_weight;
    int count = 0;
    while(count!=edge_num){
        cin >> start >> end >> price_weight >> time_weight;
        while (!this->check_edge(start, end, price_weight, time_weight)) {//数据检查
            cout << "输入的边的信息不合法，请重新输入：" << endl;
            cin >> start >> end >> price_weight >> time_weight;
        }
        price[start-1][end-1] = price_weight;
        time[start-1][end-1] = time_weight;
        general[start-1][end-1] = price_weight*time_weight;
        int temp = time_weight<=3?1:time_weight-3;
        flight[start-1][end-1] = (price_weight+3)*temp;
        //无向图需反过来再添加一遍！
        count++;
    }
};
void Graph::Dijkstra(Item *item){
    int begin = item->start;
    int policy_id = item->policy_id;
    int end = item->end;
    if(policy_id == 1){
        //初始化所有数据
        for (int i=0; i<node_num; i++) {
            dis_price[i].visit = false;
            dis_price[i].value = 0;
            dis_price[i].path = "";
        }
        //首先初始化我们的dis数组
        int i;
        for (i = 0; i < this->node_num; i++) {
            //设置当前的路径
            dis_price[i].path = to_string(begin) + "->" + to_string(i + 1);
            dis_price[i].value = price[begin - 1][i];
        }
        //设置起点的到起点的路径为0
        dis_price[begin - 1].value = 0;
        dis_price[begin - 1].visit = true;
        
        int count = 1;
        //计算剩余的顶点的最短路径（剩余this->node_num-1个顶点）
        while (count != this->node_num) {
            //temp用于保存当前dis数组中最小的那个下标
            //min记录的当前的最小值
            int temp=0;
            int min = INT_MAX;
            for (i = 0; i < this->node_num; i++) {
                if (!dis_price[i].visit && dis_price[i].value<min) {
                    min = dis_price[i].value;
                    temp = i;
                }
            }
            
            //把temp对应的顶点加入到已经找到的最短路径的集合中
            dis_price[temp].visit = true;
            ++count;
            for (i = 0; i < this->node_num; i++) {
                //注意这里的条件arc[temp][i]!=INT_MAX必须加，不然会出现溢出，从而造成程序异常
                if (!dis_price[i].visit && price[temp][i]!=INT_MAX && (dis_price[temp].value + price[temp][i]) < dis_price[i].value) {
                    //如果新得到的边可以影响其他为访问的顶点，那就更新它的最短路径和长度
                    dis_price[i].value = dis_price[temp].value + price[temp][i];
                    dis_price[i].path = dis_price[temp].path + "->" + to_string(i + 1);
                }
            }
        }
        if(dis_price[end-1].value!=INT_MAX&&dis_price[end-1].value>=0){
            item->path = dis_price[end - 1].path;
            item->dis = dis_price[end - 1].value;
        }
        else {
            item->path = "There is no path";
            item->dis = -1;
        }
    }
    else if(policy_id == 2){
        //初始化所有数据
        for (int i=0; i<node_num; i++) {
            dis_time[i].visit = false;
            dis_time[i].value = 0;
            dis_time[i].path = "";
        }
        //首先初始化我们的dis数组
        int i;
        for (i = 0; i < this->node_num; i++) {
            //设置当前的路径
            dis_time[i].path = to_string(begin) + "->" + to_string(i + 1);
            dis_time[i].value = time[begin - 1][i];
        }
        //设置起点的到起点的路径为0
        dis_time[begin - 1].value = 0;
        dis_time[begin - 1].visit = true;
        
        int count = 1;
        //计算剩余的顶点的最短路径（剩余this->node_num-1个顶点）
        while (count != this->node_num) {
            //temp用于保存当前dis数组中最小的那个下标
            //min记录的当前的最小值
            int temp=0;
            int min = INT_MAX;
            for (i = 0; i < this->node_num; i++) {
                if (!dis_time[i].visit && dis_time[i].value<min) {
                    min = dis_time[i].value;
                    temp = i;
                }
            }
            
            //把temp对应的顶点加入到已经找到的最短路径的集合中
            dis_time[temp].visit = true;
            ++count;
            for (i = 0; i < this->node_num; i++) {
                //注意这里的条件arc[temp][i]!=INT_MAX必须加，不然会出现溢出，从而造成程序异常
                if (!dis_time[i].visit && time[temp][i]!=INT_MAX && (dis_time[temp].value + time[temp][i]) < dis_time[i].value) {
                    //如果新得到的边可以影响其他为访问的顶点，那就更新它的最短路径和长度
                    dis_time[i].value = dis_time[temp].value + time[temp][i];
                    dis_time[i].path = dis_time[temp].path + "->" + to_string(i + 1);
                }
            }
        }
        if(dis_time[end-1].value!=INT_MAX&&dis_time[end-1].value>=0){
            item->path = dis_time[end - 1].path;
            item->dis = dis_time[end - 1].value;
        }
        else {
            item->path = "There is no path";
            item->dis = -1;
        }
    }
    else if(policy_id == 3){
        //初始化所有数据
        for (int i=0; i<node_num; i++) {
            dis_general[i].visit = false;
            dis_general[i].value = 0;
            dis_general[i].path = "";
        }
        //首先初始化我们的dis数组
        int i;
        for (i = 0; i < this->node_num; i++) {
            //设置当前的路径
            dis_general[i].path = to_string(begin) + "->" + to_string(i + 1);
            dis_general[i].value = general[begin - 1][i];
        }
        //设置起点的到起点的路径为0
        dis_general[begin - 1].value = 0;
        dis_general[begin - 1].visit = true;
        
        int count = 1;
        //计算剩余的顶点的最短路径（剩余this->node_num-1个顶点）
        while (count != this->node_num) {
            //temp用于保存当前dis数组中最小的那个下标
            //min记录的当前的最小值
            int temp=0;
            int min = INT_MAX;
            for (i = 0; i < this->node_num; i++) {
                if (!dis_general[i].visit && dis_general[i].value<min) {
                    min = dis_general[i].value;
                    temp = i;
                }
            }
            
            //把temp对应的顶点加入到已经找到的最短路径的集合中
            dis_general[temp].visit = true;
            ++count;
            for (i = 0; i < this->node_num; i++) {
                //注意这里的条件arc[temp][i]!=INT_MAX必须加，不然会出现溢出，从而造成程序异常
                if (!dis_general[i].visit && general[temp][i]!=INT_MAX && (dis_general[temp].value + general[temp][i]) < dis_general[i].value) {
                    //如果新得到的边可以影响其他为访问的顶点，那就更新它的最短路径和长度
                    dis_general[i].value = dis_general[temp].value + general[temp][i];
                    dis_general[i].path = dis_general[temp].path + "->" + to_string(i + 1);
                }
            }
        }
        if(dis_general[end-1].value!=INT_MAX&&dis_general[end-1].value>=0){
            item->path = dis_general[end - 1].path;
            item->dis = dis_general[end - 1].value;
        }
        else {
            item->path = "There is no path";
            item->dis = -1;
        }
    }
    else if(policy_id == 4){
        //初始化所有数据
        for (int i=0; i<node_num; i++) {
            dis_flight[i].visit = false;
            dis_flight[i].value = 0;
            dis_flight[i].path = "";
        }
        //首先初始化我们的dis数组
        int i;
        for (i = 0; i < this->node_num; i++) {
            //设置当前的路径
            dis_flight[i].path = to_string(begin) + "->" + to_string(i + 1);
            dis_flight[i].value = flight[begin - 1][i];
        }
        //设置起点的到起点的路径为0
        dis_flight[begin - 1].value = 0;
        dis_flight[begin - 1].visit = true;
        
        int count = 1;
        //计算剩余的顶点的最短路径（剩余this->node_num-1个顶点）
        while (count != this->node_num) {
            //temp用于保存当前dis数组中最小的那个下标
            //min记录的当前的最小值
            int temp=0;
            int min = INT_MAX;
            for (i = 0; i < this->node_num; i++) {
                if (!dis_flight[i].visit && dis_flight[i].value<min) {
                    min = dis_flight[i].value;
                    temp = i;
                }
            }
            
            //把temp对应的顶点加入到已经找到的最短路径的集合中
            dis_flight[temp].visit = true;
            ++count;
            for (i = 0; i < this->node_num; i++) {
                //注意这里的条件arc[temp][i]!=INT_MAX必须加，不然会出现溢出，从而造成程序异常
                if (!dis_flight[i].visit && flight[temp][i]!=INT_MAX && (dis_flight[temp].value + flight[temp][i]) < dis_flight[i].value) {
                    //如果新得到的边可以影响其他为访问的顶点，那就更新它的最短路径和长度
                    dis_flight[i].value = dis_flight[temp].value + flight[temp][i];
                    dis_flight[i].path = dis_flight[temp].path + "->" + to_string(i + 1);
                }
            }
        }
        if(dis_flight[end-1].value!=INT_MAX&&dis_flight[end-1].value>=0){
            item->path = dis_flight[end - 1].path;
            item->dis = dis_flight[end - 1].value;
        }
        else {
            item->path = "There is no path";
            item->dis = -1;
        }
    }
};

//方案
class Policy{
private:
    int policy_id;//方案ID
    int policy_items_num;//方案下物品数量
    Item *list;//保存物品的数组
public:
    Policy(int policy_id, int policy_items_num, Item *items_list, int total_num);
    ~Policy();
    void showTable(Graph *graph);//打印所有物品信息
    void sortItems();//优先级排序
};
Policy::Policy(int policy_id, int policy_items_num, Item *items_list, int total_num){
    this->policy_id = policy_id;
    this->policy_items_num = policy_items_num;
    this->list = new Item[policy_items_num];
    int index=0;
    for(int i=0; i<total_num; i++){
        if(items_list[i].policy_id == this->policy_id) list[index++] = items_list[i];//筛选对应方案的物品放进物品数组
    }
};
Policy::~Policy(){
    delete[] this->list;
};
void Policy::showTable(Graph *graph){
    for(int i=0; i<policy_items_num; i++){
        graph->Dijkstra(&list[i]);//调用Dijkstra算法更改list中每个物品的路径和消耗的信息
    }
    //打印结果
    if(this->policy_id==1) cout << "ID 优先级 起 终 方案 路径 费用" << endl;
    else if(this->policy_id==2) cout << "ID 优先级 起 终 方案 路径 耗时" << endl;
    else if(this->policy_id==3) cout << "ID 优先级 起 终 方案 路径 费用*耗时" << endl;
    else if(this->policy_id==4) cout << "ID 优先级 起 终 方案 路径 (费用+3)*(耗时-3)  *（耗时-3）最小为1" << endl;
    for(int i=0; i<policy_items_num; i++){
        cout << list[i].item_id << "\t" << list[i].level << "\t" << list[i].start << "\t" << list[i].end << "\t" << list[i].policy_id << "\t" << list[i].path << "\t" << list[i].dis << endl;
    }
};
bool cmp(Item first, Item second){//比较函数
    if(first.level > second.level) return true;
    return false;
};
void Policy::sortItems(){
    sort(list, (list+policy_items_num), cmp);
};


//检验输入边数和顶点数的值是否有效：
//顶点数和边数的关系
bool check_num(int node_num, int edge_num) {
    if (node_num <= 0 || edge_num <= 0 || ((node_num*(node_num - 1)) / 2) < edge_num)
        return false;
    return true;
}


int main(){
    //建立物流节点图
    int node_num, edge_num;
    cout << "输入物流节点数和边数：" << endl;
    cin >> node_num >> edge_num;
    while (!check_num(node_num,edge_num)) {
        cout << "输入数值不合法，请重新输入：" << endl;
        cin >> node_num >> edge_num;
    }
    
    Graph graph(node_num,edge_num);
    graph.createGraph();
    
    
    //建立物流物品树
    int items_num;
    cout << "输入物流物品数量：" << endl;
    cin >> items_num;
    while(items_num<=0){
        cout << "输入错误，请重新输入：" << endl;
        cin >> items_num;
    }
    Items items(items_num, node_num);
    
    //建立策略树
    int price_items_num=0, time_items_num=0, general_items_num=0, flight_items_num=0;
    for(int i=0; i<items_num; i++){
        if(items.list[i].policy_id == 1) price_items_num++;
        else if(items.list[i].policy_id == 2) time_items_num++;
        else if(items.list[i].policy_id == 3) general_items_num++;
        else if(items.list[i].policy_id == 4) flight_items_num++;
    }
    
    Policy pricePolicy(1, price_items_num, items.list, items_num);
    pricePolicy.sortItems();

    Policy timePolicy(2, time_items_num, items.list, items_num);
    timePolicy.sortItems();
    
    Policy generalPolicy(3, general_items_num, items.list, items_num);
    generalPolicy.sortItems();
    
    Policy flightPolicy(4, flight_items_num, items.list, items_num);
    flightPolicy.sortItems();

    
    //输入policy_id获取物品信息
    int select_policy_id;
    while(1){
        cout << "请输入要查询的方案下物流物品信息，1.价格最小，2.时间最短，3.综合最优（费用*时间最小），4.航空物流（（费用+3）*（耗时-3）最小）：" << endl;
        cin.get();//吸收回车
        cin >> select_policy_id;
        if(select_policy_id==1){
            pricePolicy.showTable(&graph);
        }
        else if (select_policy_id==2){
            timePolicy.showTable(&graph);
        }
        else if (select_policy_id==3){
            generalPolicy.showTable(&graph);
        }
        else if (select_policy_id==4){
            flightPolicy.showTable(&graph);
        }
        else {
            cout << "输入错误" << endl;
        }
    }
    
    
    
    
    return 0;
}


#endif