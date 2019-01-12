#include <bits/stdc++.h>

using namespace std;

map<char, vector<string>> GRAMMAR;
multimap<string, char> reversedGrammar;
map<char, set<char>> FIRST;
map<char, set<char>> FOLLOW;
set<char> nonTerminalSymbol;
set<char> terminalSymbol;
map<char, bool> toEpsilon;

//void first(char ch, set<char> &collection) {
//    auto iter = GRAMMAR.find(ch);
//    if (iter == GRAMMAR.end()) {     //未找到以 ch 开头的产生式，则直接退出
//        return;
//    }
//    for (int i = 0; i < GRAMMAR.count(ch); i++, iter++) {
//        for (char chr : iter->second) {
//            if (!isupper(chr)) {     //终结符
//                collection.insert(chr);
//                break;
//            } else {
//                if (chr == ch) {     //存在左递归，跳出循环
//                    break;
//                }
//                first(chr, collection);     //递归查找
//                if (!toEpsilon[chr]) {       //直到找到第一个First集合里面不包含Epsilon的非终结符
//                    break;
//                }
//            }
//        }
//    }
//}
//
//void getFIRST() {
//    for (char nonTerminalChar : nonTerminalSymbol) {
//        set<char> collection;
//        first(nonTerminalChar, collection);
//        FIRST.insert(make_pair(nonTerminalChar, collection));
//    }
//}

void getFIRST() {
    bool update = true;
    while (update) {
        update = false;
        for (char nonTerminalChar : nonTerminalSymbol) { //遍历非终结符
            int firstSize = FIRST[nonTerminalChar].size();
            for (string rightSide : GRAMMAR[nonTerminalChar]) {  //遍历产生式右部
                for (char ch : rightSide) {
                    if (!isupper(ch)) {
                        FIRST[nonTerminalChar].insert(ch);
                        break;
                    } else {
                        bool flag = false;
                        for (char temp : FIRST[ch]) {
                            if (temp == '@') {
                                flag = true;
                            }
                            FIRST[nonTerminalChar].insert(temp);
                        }
                        if (!flag) {
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

bool isLast(string str, char ch) {  //ch 是否是 s 的直接或间接的最后一个非终结符
    if (!isupper(ch)) {
        return false;
    }
    for (int i = str.length() - 1; i >= 0; i--) {
        if (ch == str[i]) {
            return true;
        }
        if (!isupper(str[i]) || toEpsilon[str[i]] == false) {
            return false;
        }
    }
    return false;
}

//void follow(char ch, set<char> &collection) {
//    if (!isupper(ch)) {
//        //如果不是非终结符就退出
//        return;
//    }
//    for (auto iter = GRAMMAR.begin(); iter != GRAMMAR.end(); iter++) {
//        for (string rightSide : iter->second) {
//            for (int i = 0; i < rightSide.length(); i++) {
//                if (ch == rightSide[i]) {
//                    if (i != rightSide.length() - 1) {
//                        if (!isupper(rightSide[i + 1])) {
//                            //在所有产生式中直接出现在该非终结符后面的终结符直接加入到该非终结符的 Follow 集合
//                            collection.insert(rightSide[i + 1]);
//                        } else {
//                            //需要将该非终结符后的非终结符的 First 集合加入到该终结符的 Follow 集合中
////                            first(rightSide[i + 1], collection);
//                            for (char temp : FIRST[rightSide[i + 1]]) {
//                                collection.insert(temp);
//                            }
//                        }
//                    }
//                    if (i == rightSide.length() - 1) {
//                        collection.insert('$');
//                    } else if (i < rightSide.length() - 1) {
//                        bool flag = true;
//                        for (int j = i + 1; j < rightSide.length(); j++) {
//                            if (!isupper(rightSide[j]) || !toEpsilon[rightSide[j]]) {
//                                flag = false;
//                                if (!isupper(rightSide[j])) {
//                                    collection.insert(rightSide[j]);
//                                }
//                                break;
//                            }
//                        }
//                        if (flag) {
//                            collection.insert('$');
//                        }
//                    }
//                }
//                if (isLast(rightSide, ch)) {
//                    auto iter2 = reversedGrammar.find(rightSide);
//                    for (int i = 0; i < reversedGrammar.count(rightSide); i++, iter2++) {
//                        if (iter2->second != ch) {
//                            follow(iter2->second, collection);
//                        }
//                    }
//                }
//            }
//        }
//    }
//}

void getFOLLOW() {
    bool update = true;
    while (update) {
        update = false;
        for (char nonTerminalChar : nonTerminalSymbol) { //遍历非终结符
            int followSize = FOLLOW[nonTerminalChar].size();
            for (auto iter = GRAMMAR.begin(); iter != GRAMMAR.end(); iter++) {
                for (string rightSide : iter->second) {      //遍历所有产生式右部
//                    cout << rightSide << endl;
                    int i = 0;
                    while (i < rightSide.length()) {
                        for (; i < rightSide.length(); i++) {
                            if (nonTerminalChar == rightSide[i]) {
                                if(i == rightSide.length() - 1 && isupper(rightSide[i])) {
                                    for(char ch : FOLLOW[iter->first]) {
                                        FOLLOW[nonTerminalChar].insert(ch);
                                    }
                                }
                                i++;
                                break;
                            }
                        }
                        for (; i < rightSide.length(); i++) {
                            if (!isupper(rightSide[i])) {
                                FOLLOW[nonTerminalChar].insert(rightSide[i]);
                                break;
                            } else {
                                for (char ch : FIRST[rightSide[i]]) {
                                    if (ch != '@') {
                                        FOLLOW[nonTerminalChar].insert(ch);
                                    }
                                }
                                if (!toEpsilon[rightSide[i]]) {
                                    break;
                                } else {
                                    for(char ch : FOLLOW[iter->first]) {
                                        FOLLOW[nonTerminalChar].insert(ch);
                                    }
                                }
                            }
                            if(i == rightSide.length() - 1 && isupper(rightSide[i])) {
                                for(char ch : FOLLOW[iter->first]) {
                                    FOLLOW[nonTerminalChar].insert(ch);
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
//    for (char nonTerminalChar : nonTerminalSymbol) {
//        set<char> collection;
//        follow(nonTerminalChar, collection);
//        FOLLOW.insert(make_pair(nonTerminalChar, collection));
//        for (char temp : collection) {
//            if (temp != '@') {
//                FOLLOW[nonTerminalChar].insert(temp);
//            }
//        }
//        if (nonTerminalChar == GRAMMAR.begin()->first) {
//            FOLLOW[nonTerminalChar].insert('$');
//        }
//    }
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
        reversedGrammar.insert(make_pair(temp.substr(3), temp[0]));     //读入文法并反转添加到 reversedGrammar 中
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

    return 0;
}