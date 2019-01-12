#include <bits/stdc++.h>

using namespace std;

map<char, vector<string>> GRAMMAR;  // 文法
map<char, set<char>> FIRST;     // 非终结符的first集合
map<char, set<char>> FOLLOW;    // 非终结符的follow集合
set<char> nonTerminalSymbol;    // 非终结符集合
set<char> terminalSymbol;       //终结符集合
map<char, bool> toEpsilon;      //First集合是否存在空串
bool left_recursive = false; // 文法是否存在左递归
bool judge_LL1_third = false; // 判断是否为LL1文法的第三步

void getFIRST() {
    bool update = true;
    while (update) {
        update = false;
        for (char nonTerminalChar : nonTerminalSymbol) { //遍历非终结符
            int firstSize = FIRST[nonTerminalChar].size();
            for (string rightSide : GRAMMAR[nonTerminalChar]) {  //遍历产生式右部
                for (char ch : rightSide) {     //遍历产生式右部所有字符
                    if (!isupper(ch)) {     //如果是非终结符
                        FIRST[nonTerminalChar].insert(ch);  //将该非终结符加入到 nonTerminalChar 的 First 集合中
                        break;      //退出循环
                    } else {
                        bool flag = false;      //判断 First 集合是否存在空串
                        for (char temp : FIRST[ch]) {   //将 ch 的 First 集合加入到 nonTerminalChar 的 First 集合中
                            if (temp == '@') {
                                flag = true;
                            }
                            FIRST[nonTerminalChar].insert(temp);
                        }
                        if (!flag) {        //如果不存在空串，退出循环
                            break;
                        }
                    }
                }
            }
            if (firstSize != FIRST[nonTerminalChar].size()) {
                update = true;
            }
        }
    }
}

void getFOLLOW() {
    bool update = true;
    while (update) {
        update = false;
        for (char nonTerminalChar : nonTerminalSymbol) { //遍历非终结符
            int followSize = FOLLOW[nonTerminalChar].size();    //记录 Follow 集合初始大小
            for (auto iter = GRAMMAR.begin(); iter != GRAMMAR.end(); iter++) {      //遍历所有文法
                for (string rightSide : iter->second) {      //遍历所有产生式右部
                    int i = 0;
                    while (i < rightSide.length()) {    //遍历产生式右部字符
                        for (; i < rightSide.length(); i++) {
                            if (nonTerminalChar == rightSide[i]) {  //找到与 nonTerminalChar 相同的字符
                                if (i == rightSide.length() - 1) { //判断找到的非终结符是否是右部最后一个字符
                                    for (char ch : FOLLOW[iter->first]) {   //把产生式左部非终结符的 Follow 加入到 nonTerminalChar 的 Follow 集合中
                                        FOLLOW[nonTerminalChar].insert(ch);
                                    }
                                }
                                i++;
                                break;  //找到了就停止
                            }
                        }
                        for (; i < rightSide.length(); i++) {   //遍历后续字符
                            if (!isupper(rightSide[i])) {   //如果是非终结符
                                FOLLOW[nonTerminalChar].insert(
                                        rightSide[i]);   //直接将非终结符加入到 nonTerminalChar 的 Follow 集合中
                                break;      //直接退出
                            } else {        //是终结符
                                for (char ch : FIRST[rightSide[i]]) {   //将该终结符的 First 集合加入到 nonTerminalChar 的 Follow 集合中（除了空串）
                                    if (ch != '@') {
                                        FOLLOW[nonTerminalChar].insert(ch);
                                    }
                                }
                                if (!toEpsilon[rightSide[i]]) {     //如果该终结符的 First 集合中不存在空串，则退出循环
                                    break;
                                } else if (i == rightSide.length() - 1) {        //该终结符的 First 集合中存在空串，且为右部最后一个字符
                                    for (char ch : FOLLOW[iter->first]) {
                                        FOLLOW[nonTerminalChar].insert(
                                                ch);     //将左部非终结符的 Follow 加入到 nonTerminalChar 的 Follow 集合中
                                    }
                                }
                            }
                            if (i == rightSide.length() - 1 && rightSide[i] == nonTerminalChar) { //如果最后一个字符是 nonTerminalChar
                                for (char ch : FOLLOW[iter->first]) {
                                    FOLLOW[nonTerminalChar].insert(
                                            ch); //把产生式左部非终结符的 Follow 加入到 nonTerminalChar 的 Follow 集合中
                                }
                            }
                        }
                    }
                }
            }
            if (followSize != FOLLOW[nonTerminalChar].size()) {
                update = true;
            }
        }
    }
}

void is_left_recursive(char s, string ss) { // 判断非终结符S是否有左递归
    if (left_recursive)
        return;
    if (ss[0] == s) {
        left_recursive = true;
    }
    else if (isupper(ss[0])) {
        for(string str : GRAMMAR[ss[0]]) {
            is_left_recursive(s, str);
        }
    }
}

set<char> getRightFirst(string ss) { // 得到产生式右部的first集合
    set<char> s;
    for (char c : ss) {
        judge_LL1_third = false;
        if (!isupper(c)) {
            s.insert(c);
            break;
        }
        else {
            set<char> s = FIRST[c];
            for (auto j : s) {
                if (j == '@') {
                    judge_LL1_third = true;
                }
                else {
                    s.insert(j);
                }
            }
            if (!judge_LL1_third) {
                break;
            }
        }
    }
    return s;
}

bool has_union(set<char> s, set<char> ss) { // 判断两个集合是否有交集
    for (auto i : s) {
        for (auto j : ss) {
            if (i == j) {
                return true;
            }
        }
    }
    return false;
}

bool is_LL1() {		//判断文法是不是LL1文法
    for (auto i : nonTerminalSymbol) {
        judge_LL1_third = false;
        vector<string> v = GRAMMAR[i]; // 存储产生式

        // 1. 判断是否有左递归
        for(string str : v) {
            is_left_recursive(i, str);
            if (left_recursive) {
                return false;
            }
        }
        // 2. 判断first是否有交集
        for (int i = 0; i < v.size(); i++) {
            for (int j = i + 1; j < v.size(); j++) {
                if (has_union(getRightFirst(v[i]), getRightFirst(v[j]))) {
                    return false;
                }
            }
        }
        // 3. 判断第三步
        if (judge_LL1_third) {
            for (auto s : v) {
                if (has_union(getRightFirst(s), FOLLOW[i]));
            }
        }
    }
    return true;
}

int main(int argc, char **argv) {
//    if(argc <= 1) {
////        cout << "Please input file.";
////        exit(EXIT_FAILURE);
////    }
////    if(!freopen(argv[1], "r", stdin)) {
////        cout << "Fail to open file.";
////        exit(EXIT_FAILURE);
////    }
    ifstream stream;
    stream.open("../in.txt");
    if (!stream.is_open()) {
        printf(" 无法打开文法文件\n");
        exit(1);
    }
    string temp;
    bool isFirstNonTerminalChar = true;
    while (getline(stream, temp)) {
        GRAMMAR[temp[0]].push_back(temp.substr(3));
        if (isFirstNonTerminalChar) {
            FOLLOW[temp[0]].insert('$');
            isFirstNonTerminalChar = false;
        }
        nonTerminalSymbol.insert(temp[0]);      //初始化非终结符集合
        for (int i = 3; i < temp.length(); i++) {
            if (!isupper(temp[i]) && temp[i] != '@') {
                terminalSymbol.insert(temp[i]);     //初始化终结符集合
            }
        }
    }
    getFIRST();     //计算 First 集合
    for (char nonTerminalChar: nonTerminalSymbol) {
        toEpsilon[nonTerminalChar] = FIRST[nonTerminalChar].find('@') != FIRST[nonTerminalChar].end();
    }
    getFOLLOW();    //计算 Follow 集合

    cout << "First: " << endl;
    for (char chr : nonTerminalSymbol) {
        cout << chr << ": ";
        set<char> first;
        first = FIRST[chr];
        for (char ch : first) {
            cout << ch;
        }
        cout << endl;
    }
    cout << "Follow: " << endl;
    for (char chr : nonTerminalSymbol) {
        cout << chr << ": ";
        set<char> follow;
        follow = FOLLOW[chr];
        for (char ch : follow) {
            cout << ch;
        }
        cout << endl;
    }

    cout << "terminalSymbol: " << endl;
    for (char terminalChar : terminalSymbol) {
        cout << terminalChar << " ";
    }
    cout << endl;
    if (!is_LL1()) {
        printf("No LL(1)\n");
    }
    else {
        printf("Is LL(1)\n");
    }
    return 0;
}