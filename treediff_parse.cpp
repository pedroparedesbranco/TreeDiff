#include <string>
#include <stack>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "bp_support_pedro2.hpp"

using namespace std;
using namespace sdsl;

struct Struct {
    int lca;
    int size;
};

void get_weight(FILE* fp, vector<float> &w, char c, int count, bool &weighted_trees, float &w_rf_dist){
    string str = "";
    float weight;
    while(!(c == ')' || c == '(' || c == ',' || c == ';')){
        str.append(1, c);
        c = getc(fp);
    }
    weight = stof(str);
    if(w.size() < count + 1){
        w.resize(count + 1);
    }
    w[count] = weight;
    w_rf_dist = w_rf_dist + weight;
    weighted_trees = true;
    ungetc(c, fp);
}

void get_string(FILE* fp, char c, int count, unordered_map<string, int>& strings, vector<int>& code, int num_tree, bool &internal_nodes, bool &weighted_trees, float &w_rf_dist, vector<float>& w1, vector<float>& w2){
    string str = "";
    int aux;
    while(!(c == ')' || c == '(' || c == ',' || c == ';' || c == ':')){
        str.append(1, c);
        c = getc(fp);
    }
    if(str != "_"){
        // cout << "count = " << count << "  string = " << str << endl;
        if(num_tree == 0){
            strings[str] = count;
        }
        else{
            aux = strings[str];
            if(code.size() < aux + 1){
                code.resize(aux + 1);
            }
            code[strings[str]] = count;
        }
    }
    else{
        // cout << "nao entrou str = " << str << endl;
        internal_nodes = false;
    }
    if(c == ':'){
        c = getc(fp);
        if(num_tree == 0){
            get_weight(fp, w1, c, count, weighted_trees, w_rf_dist);
        }
        else{
            get_weight(fp, w2, c, count, weighted_trees, w_rf_dist);
        }
        return;
    }
    ungetc(c, fp);
}

void add_bit(bit_vector& bv, int i){
    bv.resize(bv.size() + 1);
    bv[bv.size() - 1] = i;
}

bit_vector create_bit_vector(char* tree, unordered_map<string, int>& strings, vector<int>& code, int num_tree, int &num_nodes, bool &internal_nodes, bool &weighted_trees, float &w_rf_dist, vector<float>& w1, vector<float>& w2){
    bit_vector bv = bit_vector(0);
    stack<int> stack;
    string str = "";
    int count = 0;
    int state;
    char c;
    FILE* fp;
    fp = fopen(tree, "r");
    while((c=getc(fp)) != ';'){
        if(c == '('){
            stack.push(count);
            count++;
            add_bit(bv, 1);
        }
        else if(c == ','){
            continue;
        }
        else if(c == ')'){
            num_nodes++;
            c = getc(fp);
            if(!(c == ')' || c == '(' || c == ',' || c == ';')){
                if(c == ':'){
                    c = getc(fp);
                    if(num_tree == 0){
                        get_weight(fp, w1, c, stack.top(), weighted_trees, w_rf_dist);
                    }
                    else{
                        get_weight(fp, w2, c, stack.top(), weighted_trees, w_rf_dist);
                    }
                }
                else{
                    // cout << "c = " << c << endl;
                    internal_nodes = true;
                    get_string(fp, c, stack.top(), strings, code, num_tree, internal_nodes, weighted_trees, w_rf_dist, w1, w2);
                }     
            }
            else{
                ungetc(c, fp);
            }
            stack.pop();
            add_bit(bv, 0);
        }
        else{
            // cout << "c = " << c << endl;
            add_bit(bv, 1);
            add_bit(bv, 0);
            get_string(fp, c, count, strings, code, num_tree, internal_nodes, weighted_trees, w_rf_dist, w1, w2);
            count++;
        }
    }
    fclose(fp);
    // cout << bv << endl;
    // cout << bv.size() << endl;
    return bv;
}


int main(int argc, char* argv[]){

    int rf_dist = 0;

    int num_nodes1 = 0;
    int num_nodes2 = 0;

    int size;

    float w_rf_dist = 0;

    bool internal_nodes = false;
    bool weighted_trees = false;

    vector<int> clusters_1;
    vector<int> clusters_2;

    unordered_map<string, int> strings;

    vector<int> code;

    vector<float> w1;
    vector<float> w2;

    auto start = std::chrono::system_clock::now();
    
    bit_vector v_1 = create_bit_vector(argv[1], strings, code, 0, num_nodes1, internal_nodes, weighted_trees, w_rf_dist, w1, w2);
    bit_vector v_2 = create_bit_vector(argv[2], strings, code, 1, num_nodes2, internal_nodes, weighted_trees, w_rf_dist, w1, w2);

    strings.clear();

    size = v_1.size() / 2;

    bp_support_pedro2<> vec_1(&v_1);
    bp_support_pedro2<> vec_2(&v_2);

    FILE* fp;
    stringstream s1, s2, ss1, ss2;

    v_1.serialize(s1);
    v_2.serialize(s2);
    vec_1.serialize(ss1);
    vec_2.serialize(ss2);

    // cout << "s1 = " << s1.str() << endl;
    // cout << "s2 = " << s2.str() << endl;
    // cout << "ss1 = " << ss1.str() << endl;
    // cout << "ss2 = " << ss2.str() << endl;

    ofstream files1("serialization/s1.txt");

    files1 << s1.str();
    files1.close();

    ofstream files2("serialization/s2.txt");

    files2 << s2.str();
    files2.close();

    ofstream filess1("serialization/ss1.txt");

    filess1 << ss1.str();
    filess1.close();

    ofstream filess2("serialization/ss2.txt");

    filess2 << ss2.str();
    filess2.close();


    // fp = fopen("serialization/s1.txt", "w");
    // fprintf(fp, "%s", s1.str());
    // fclose(fp);

    // fp = fopen("serialization/s2.txt", "w");
    // fprintf(fp, "%s", s2.str());
    // fclose(fp);

    // fp = fopen("serialization/ss1.txt", "w");
    // fprintf(fp, "%s", ss1.str());
    // fclose(fp);

    // fp = fopen("serialization/ss2.txt", "w");
    // fprintf(fp, "%s", ss2.str());
    // fclose(fp);





    auto end = std::chrono::system_clock::now();
    return 0;
}
