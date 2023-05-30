#include <string>
#include <stack>
#include<iostream>
#include<fstream>
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

// without internal nodes (post_order implementation)
void weighted_rf_onlyleaves(bp_support_pedro2<> vec_1, bp_support_pedro2<> vec_2, vector<int>& code, int size, float &w_rf_dist, vector<int> &clusters_1, vector<float>& w1, vector<float>& w2){
    int lcas = -1;
    int current, post_current;
    int counter;
    int size_cluster;
    int close;
    float weight1, weight2;
    stack<Struct> lcas_stack;
    Struct first;
    // cout << "size = " << size << endl;
    for(current = 1; current <= size; current++){
        post_current = vec_1.post_order_select(current);
        if(vec_1.isleaf(post_current)){
            // cout << "current = " << current << "  post_current = " << vec_1.nodemap(post_current) << endl;
            first.lca = vec_2.nodeselect(code[vec_1.nodemap(post_current) - 1] + 1);
            // cout << "first.lca = " << vec_2.nodemap(first.lca) << endl;
            first.size = 1;
            weight1 = w1[vec_1.nodemap(post_current) - 1];
            weight2 = w2[vec_2.nodemap(first.lca) - 1];
            w_rf_dist = w_rf_dist - (weight1 + weight2 - abs(weight1 - weight2));
            lcas_stack.push(first);
        }
        else{
            close = vec_1.find_close(post_current);
            size_cluster = vec_1.cluster_size2(post_current, close) - 1; // TODO: -1??
            // cout << "current = " << current << "  post_current = " << vec_1.nodemap(post_current) << "  size = " << size_cluster << endl;
            counter = size_cluster;
            while(counter != 0){
                if(lcas == -1){
                    lcas = lcas_stack.top().lca;
                    counter = counter - lcas_stack.top().size;
                    lcas_stack.pop();
                }
                else{
                    lcas = vec_2.lca(lcas, lcas_stack.top().lca);
                    counter = counter - lcas_stack.top().size;
                    lcas_stack.pop();
                }
                // cout << "lcas = " << vec_2.nodemap(lcas) << "  counter = " << counter << endl;
            }
            first.lca = lcas;
            first.size = size_cluster + 1;
            lcas_stack.push(first);
            // cout << "Comparission between: " << vec_1.nodemap(post_current) << " and " << vec_2.nodemap(lcas) << endl;
            if(vec_1.num_leaves2(post_current, close) == vec_2.num_leaves(lcas)){
                weight1 = w1[vec_1.nodemap(post_current) - 1];
                weight2 = w2[vec_2.nodemap(lcas) - 1];
                w_rf_dist = w_rf_dist - (weight1 + weight2 - abs(weight1 - weight2));
            }
            else{
                clusters_1.push_back(vec_1.nodemap(post_current));
            }
            lcas = -1;
        }
    }
}

// with internal nodes (post_order implementation)
void weighted_rf_allnodes(bp_support_pedro2<> vec_1, bp_support_pedro2<> vec_2, vector<int>& code, int size, float &w_rf_dist, vector<int> &clusters_1, vector<float>& w1, vector<float>& w2){
    int lcas = -1;
    int current, post_current;
    int counter;
    int size_cluster;
    float weight1, weight2;
    stack<Struct> lcas_stack;
    Struct first;
    // cout << "size = " << size << endl;
    for(current = 1; current <= size; current++){
        post_current = vec_1.post_order_select(current);
        if(vec_1.isleaf(post_current)){
            // cout << "current = " << current << "  post_current = " << vec_1.nodemap(post_current) << endl;
            first.lca = vec_2.nodeselect(code[vec_1.nodemap(post_current) - 1] + 1);
            // cout << "first.lca = " << vec_2.nodemap(first.lca) << endl;
            first.size = 1;
            lcas_stack.push(first);
        }
        else{
            first.lca = vec_2.nodeselect(code[vec_1.nodemap(post_current) - 1] + 1);
            // cout << "first.lca = " << vec_2.nodemap(first.lca) << endl;
            first.size = 1;
            lcas_stack.push(first);

            size_cluster = vec_1.cluster_size(post_current); // TODO: -1??
            // cout << "current = " << current << "  post_current = " << vec_1.nodemap(post_current) << "  size = " << size_cluster << endl;
            counter = size_cluster;
            while(counter != 0){
                if(lcas == -1){
                    lcas = lcas_stack.top().lca;
                    counter = counter - lcas_stack.top().size;
                    lcas_stack.pop();
                }
                else{
                    lcas = vec_2.lca(lcas, lcas_stack.top().lca);
                    counter = counter - lcas_stack.top().size;
                    lcas_stack.pop();
                }
                // cout << "lcas = " << vec_2.nodemap(lcas) << "  counter = " << counter << endl;
            }
            first.lca = lcas;
            first.size = size_cluster;
            lcas_stack.push(first);
            // cout << "Comparission between: " << vec_1.nodemap(post_current) << " and " << vec_2.nodemap(lcas) << endl;
            if(size_cluster == vec_2.cluster_size(lcas)){
                weight1 = w1[vec_1.nodemap(post_current) - 1];
                weight2 = w2[vec_2.nodemap(lcas) - 1];
                w_rf_dist = w_rf_dist - (weight1 + weight2 - abs(weight1 - weight2));
            }
            else{
                clusters_1.push_back(vec_1.nodemap(post_current));
            }
            lcas = -1;
        }
    }
}

// without internal nodes (post_order implementation)
void rf_onlyleaves(bp_support_pedro2<> vec_1, bp_support_pedro2<> vec_2, vector<int>& code, int size, int &rf_dist, vector<int> &clusters_1){
    int lcas = -1;
    int current, post_current;
    int counter;
    int size_cluster;
    int close;
    stack<Struct> lcas_stack;
    Struct first;
    // cout << "size = " << size << endl;
    for(current = 1; current <= size; current++){
        post_current = vec_1.post_order_select(current);
        if(vec_1.isleaf(post_current)){
            // cout << "current = " << current << "  post_current = " << vec_1.nodemap(post_current) << endl;
            first.lca = vec_2.nodeselect(code[vec_1.nodemap(post_current) - 1] + 1);
            // cout << "first.lca = " << vec_2.nodemap(first.lca) << endl;
            first.size = 1;
            lcas_stack.push(first);
        }
        else{
            close = vec_1.find_close(post_current);
            size_cluster = vec_1.cluster_size2(post_current, close) - 1; // TODO: -1??
            // cout << "current = " << current << "  post_current = " << vec_1.nodemap(post_current) << "  size = " << size_cluster << endl;
            counter = size_cluster;
            while(counter != 0){
                if(lcas == -1){
                    lcas = lcas_stack.top().lca;
                    counter = counter - lcas_stack.top().size;
                    lcas_stack.pop();
                }
                else{
                    lcas = vec_2.lca(lcas, lcas_stack.top().lca);
                    counter = counter - lcas_stack.top().size;
                    lcas_stack.pop();
                }
                // cout << "lcas = " << vec_2.nodemap(lcas) << "  counter = " << counter << endl;
            }
            first.lca = lcas;
            first.size = size_cluster + 1;
            lcas_stack.push(first);
            // cout << "Comparission between: " << vec_1.nodemap(post_current) << " and " << vec_2.nodemap(lcas) << endl;
            if(vec_1.num_leaves2(post_current, close) == vec_2.num_leaves(lcas)){
                rf_dist++;
            }
            else{
                clusters_1.push_back(vec_1.nodemap(post_current));
            }
            lcas = -1;
        }
    }
}

// with internal nodes (post_order implementation)
void rf_allnodes(bp_support_pedro2<> vec_1, bp_support_pedro2<> vec_2, vector<int>& code, int size, int &rf_dist, vector<int> &clusters_1){
    int lcas = -1;
    int current, post_current;
    int counter;
    int size_cluster;
    stack<Struct> lcas_stack;
    Struct first;
    // cout << "size = " << size << endl;
    for(current = 1; current <= size; current++){
        post_current = vec_1.post_order_select(current);
        if(vec_1.isleaf(post_current)){
            // cout << "current = " << current << "  post_current = " << vec_1.nodemap(post_current) << endl;
            first.lca = vec_2.nodeselect(code[vec_1.nodemap(post_current) - 1] + 1);
            // cout << "first.lca = " << vec_2.nodemap(first.lca) << endl;
            first.size = 1;
            lcas_stack.push(first);
        }
        else{
            first.lca = vec_2.nodeselect(code[vec_1.nodemap(post_current) - 1] + 1);
            // cout << "first.lca = " << vec_2.nodemap(first.lca) << endl;
            first.size = 1;
            lcas_stack.push(first);

            size_cluster = vec_1.cluster_size(post_current); // TODO: -1??
            // cout << "current = " << current << "  post_current = " << vec_1.nodemap(post_current) << "  size = " << size_cluster << endl;
            counter = size_cluster;
            while(counter != 0){
                if(lcas == -1){
                    lcas = lcas_stack.top().lca;
                    counter = counter - lcas_stack.top().size;
                    lcas_stack.pop();
                }
                else{
                    lcas = vec_2.lca(lcas, lcas_stack.top().lca);
                    counter = counter - lcas_stack.top().size;
                    lcas_stack.pop();
                }
                // cout << "lcas = " << vec_2.nodemap(lcas) << "  counter = " << counter << endl;
            }
            first.lca = lcas;
            first.size = size_cluster;
            lcas_stack.push(first);
            // cout << "Comparission between: " << vec_1.nodemap(post_current) << " and " << vec_2.nodemap(lcas) << endl;
            if(size_cluster == vec_2.cluster_size(lcas)){
                rf_dist++;
            }
            else{
                clusters_1.push_back(vec_1.nodemap(post_current));
            }
            lcas = -1;
        }
    }
}

void triplets_onlyleaves(bp_support_pedro2<> vec_1, bp_support_pedro2<> vec_2){

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

    bit_vector v_3;
    bit_vector v_4;

    bp_support_pedro2<> vec_3(nullptr);
    bp_support_pedro2<> vec_4(nullptr);

    FILE* fp;

    // stringstream s1, s2, ss1, ss2;

    ifstream files1("serialization/s1.txt");

    stringstream s1;
    s1 << files1.rdbuf();
    files1.close();

    ifstream files2("serialization/s2.txt");

    stringstream s2;
    s2 << files2.rdbuf();
    files2.close();

    ifstream filess1("serialization/ss1.txt");

    stringstream ss1;
    ss1 << filess1.rdbuf();
    filess1.close();

    ifstream filess2("serialization/ss2.txt");

    stringstream ss2;
    ss2 << filess2.rdbuf();
    filess2.close();

    v_3.load(s1);
    v_4.load(s2);

    cout << v_1 << endl;
    cout << v_3 << endl;
    cout << v_2 << endl;
    cout << v_4 << endl;

    vec_4.load(ss1, &v_4);
    vec_3.load(ss2, &v_3);



    // ifstream in_file("s1.txt", ios::binary);
    // in_file.seekg(0, ios::end);
    // int file_size = in_file.tellg();
    // cout<<"Size of the file is"<<" "<< file_size<<" "<<"bytes";
    // fp = fopen("serialization/s1.txt", "r");

    // const std::string& tmp = s1.str();
    // const char* cstr = tmp.c_str();
    // cout << fgets(cstr, file_size, fp);

    // v_3.load()



    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    // cout << "parse time: " << elapsed_seconds.count() << "s" << endl;

    auto start2 = std::chrono::system_clock::now();
    // Some computation here
    if(internal_nodes){
        rf_allnodes(vec_1, vec_2, code, size, rf_dist, clusters_1);
        cout << "Robinson Foulds distance is: " << float(num_nodes1 + num_nodes2 - rf_dist*2) / 2 << endl;
        // cout << "clusters differents in " << argv[1] << ":" << endl;
        // for (int i = 0; i < clusters_1.size(); i++){
        //     cout << clusters_1[i] << endl;
        // }
    }
    else{
        if(weighted_trees){
            weighted_rf_onlyleaves(vec_1, vec_2, code, size, w_rf_dist, clusters_1, w1, w2);
            cout << "distance = " << w_rf_dist / 2 << endl;
            // cout << "clusters differents in " << argv[1] << ":" << endl;
            // for (int i = 0; i < clusters_1.size(); i++){
            //     cout << clusters_1[i] << "  weight1 = " << w1[clusters_1[i] - 1] << "  weight2 = " << w2[code[clusters_1[i] - 1]] << endl;
            // }

        }
        else{
            rf_onlyleaves(vec_3, vec_4, code, size, rf_dist, clusters_1);
            // cout << rf_dist << endl;
            cout << "distance = " << float(num_nodes1 + num_nodes2 - rf_dist*2) / 2 << endl;
            // cout << "[1]" << endl;
            // cout << "Robinson Foulds distance is: " << rf_dist << endl;
            // cout << "clusters differents in " << argv[1] << ":" << endl;
            // for (int i = 0; i < clusters_1.size(); i++){
            //     cout << clusters_1[i] << endl;
            // }
        }
    }
    auto end2 = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds2 = end2-start2;
    // cout << "algorithm time: " << elapsed_seconds2.count() << "s" << endl;
    // cout << elapsed_seconds2.count() << ", ";
    return 0;
}
