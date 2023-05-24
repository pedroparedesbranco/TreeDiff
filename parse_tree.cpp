#include <array>
#include <string>
#include <queue>
#include <memory>
#include <cmath>
#include <stack>
#include <unordered_map>
#include <iostream>
#include <list>

//#include <sdsl/k2_tree.hpp>
//#include <sdsl/bp_support_g.hpp>

#include "bp_support_ggg.hpp"
#include "bp_support_pedro.hpp"
#include <sdsl/bp_support_g.hpp>

using namespace std;
using namespace sdsl;

list<int> clusters_1 = {};
list<int> clusters_2 = {};

unordered_map<string, int> hash_table1;
unordered_map<int, string> hash_table1_aux;
unordered_map<string, int> hash_table2;
unordered_map<int, string> hash_table2_aux;

string* array1;
string* array2;

int array1_count = 0;
int array2_count = 0;

float* weights1;
float* weights2;

int weights1_count = 0;
int weights2_count = 0;

bool internal_nodes = false;
bool weights_bool = false;

int rf_dist = 0;
int triplets_dist = 0;

int total = 0;

// void remove_spaces(char * str){
//     int i, j;
//     int len = strlen(str);
//     for(i=0; i<len; i++)
//     {
//         if(str[i] == ' ')
//         {
//             for(j=i; j < len; j++)
//             {
//                 str[j] = str[j+1];
//             }
//             len--;
//             i--;
//         }
//     }
// }

// void get_string_2(FILE* fp, string* array, float* weights, unordered_map<string, int> &hash_table, char c, int count){
//     string str = "";
//     while(!(c == ')' || c == '(' || c == ',' || c == ';')){
//         str.append(1, c);
//         c = getc(fp);
//         // internal_nodes = true;
//     }
//     // cout << str << endl;
//     hash_table[str] = count + 1;
//     array[count] = str;
//     ungetc(c, fp);
// }

void get_weight(FILE* fp, float* weights, char c, int count){
    string str = "";
    float weight;
    while(!(c == ')' || c == '(' || c == ',' || c == ';')){
        str.append(1, c);
        c = getc(fp);
    }
    weight = stof(str);
    weights[count] = weight;
    total = total + weight;
    weights_bool = true;
}

void get_string(FILE* fp, string* array, float* weights, unordered_map<string, int> &hash_table, char c, int count){
    string str = "";
    while(!(c == ')' || c == '(' || c == ',' || c == ';' || c == ':')){
        str.append(1, c);
        c = getc(fp);
    }
    // if(str != "_"){
    //     hash_table[str] = count + 1;
    //     array[count] = str;
    // }
    // else{
    //     internal_nodes = false;
    // }
    hash_table[str] = count + 1;
    // hash_table_aux[count + 1] = str;
    // cout << "inicio = " << count << endl;
    array[count] = str;
    // cout << "fim = " << count << endl;
    
    if(c == ':'){
        c = getc(fp);
        get_weight(fp, weights, c, count);
    }
    ungetc(c, fp);
}

// int get_string_2(char* tree, string* array, unordered_map<string, int> &hash_table, int current, int count){
//     string str = "";
//     while(!(tree[current] == ')' || tree[current] == '(' || tree[current] == ',' || tree[current] == '\0')){
//         str.append(1, tree[current]);
//         current++;
//     }
//     hash_table[str] = count + 1;
//     array[count] = str;
//     return current - 1;
// }

void add_bit(bit_vector& bv, int i, string* array, float* weights, int &array_count, int &weights_count){
    // if(i == 1){
    //     (array_count)++;
    //     (weights_count)++;
    //     cout << array_count << endl;
    //     if(array_count % 1000 == 0){
    //         array = (string*)realloc(array, (array_count + 1000) * sizeof(string));
    //         weights = (float*)realloc(weights, (weights_count + 1000) * sizeof(float));
    //     }
    // }
    bv.resize(bv.size() + 1);
    bv[bv.size() - 1] = i;
}

bit_vector create_bit_vector(char* tree, string* array, float* weights, unordered_map<string, int> &hash_table, int &array_count, int &weights_count){
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
            add_bit(bv, 1, array, weights, array_count, weights_count);
        }
        else if(c == ','){
            continue;
        }
        else if(c == ')'){
            c = getc(fp);
            if(!(c == ')' || c == '(' || c == ',' || c == ';')){
                if(c == ':'){
                    get_weight(fp, weights, c, stack.top());
                }
                else{
                    get_string(fp, array, weights, hash_table, c, stack.top());
                    internal_nodes = true;
                }
            }
            else{
                ungetc(c, fp);
            }
            stack.pop();
            add_bit(bv, 0, array, weights, array_count, weights_count);
        }
        else{
            add_bit(bv, 1, array, weights, array_count, weights_count);
            add_bit(bv, 0, array, weights, array_count, weights_count);
            get_string(fp, array, weights, hash_table, c, count);
            count++;
        }
    }
    fclose(fp);
    // cout << bv << endl;
    // cout << bv.size() << endl;
    return bv;
}


// without internal nodes
int rf_2(bp_support_pedro<> vec_1, bp_support_pedro<> vec_2, string* array1, unordered_map<string, int> &hash_table, int current){
    int lcas, lcas_aux, x;
    
    if(vec_1.isleaf(current)){
        // cout << vec_1.nodemap(current) << endl;
        // cout << "hash1 = " << hash_table[array1[vec_1.nodemap(current) - 1]] << endl;
        lcas = vec_2.nodeselect(hash_table[array1[vec_1.nodemap(current) - 1]]);
    }
    else{
        // cout << vec_1.nodemap(current) << endl;
        lcas = rf_2(vec_1, vec_2, array1, hash_table, vec_1.first_child(current));
        // cout << "Comparission between: " << vec_1.nodemap(current) << " and " << vec_2.nodemap(lcas) << endl;
        if(vec_1.cluster_size(current) != vec_2.cluster_size(lcas)){
            clusters_1.push_back(vec_1.nodemap(current));
            clusters_2.push_back(vec_2.nodemap(lcas));
            rf_dist++;
        }
    }
    

    while((x = vec_1.next_sibling(current)) != -1){
        current = x;
        if(vec_1.isleaf(current)){ 
            // cout << vec_1.nodemap(current) << endl;
            // cout << "hash2 = " << hash_table[array1[vec_1.nodemap(current) - 1]] << endl;
            // cout << vec_2.nodeselect(hash_table[array1[vec_1.nodemap(current) - 1]]) << endl;
            lcas = vec_2.lca(lcas, vec_2.nodeselect(hash_table[array1[vec_1.nodemap(current) - 1]]));
        }
        else{
            // cout << vec_1.nodemap(current) << endl;
            lcas_aux = rf_2(vec_1, vec_2, array1, hash_table, vec_1.first_child(current));
            lcas = vec_2.lca(lcas, lcas_aux);
            // cout << "Comparission between: " << vec_1.nodemap(current) << " and " << vec_2.nodemap(lcas_aux) << endl;
            if(vec_1.cluster_size(current) != vec_2.cluster_size(lcas_aux)){
                clusters_1.push_back(vec_1.nodemap(current));
                clusters_2.push_back(vec_2.nodemap(lcas_aux));
                rf_dist++;
            }
        }
    }
    return lcas;
}

int rf_weighted_2(bp_support_pedro<> vec_1, bp_support_pedro<> vec_2, string* array1, unordered_map<string, int> &hash_table, int current){
    int lcas, lcas_aux, x;
    
    if(vec_1.isleaf(current)){
        // cout << vec_1.nodemap(current) << endl;
        lcas = vec_2.nodeselect(hash_table[array1[vec_1.nodemap(current) - 1]]);
    }
    else{
        // cout << vec_1.nodemap(current) << endl;
        lcas = rf_2(vec_1, vec_2, array1, hash_table, vec_1.first_child(current));
        // cout << "Comparission between: " << vec_1.nodemap(current) << " and " << vec_2.nodemap(lcas) << endl;
        if(vec_1.cluster_size(current) != vec_2.cluster_size(lcas)){
            clusters_1.push_back(vec_1.nodemap(current));
            clusters_2.push_back(vec_2.nodemap(lcas));
            rf_dist++;
        }
    }
    

    while((x = vec_1.next_sibling(current)) != -1){
        current = x;
        if(vec_1.isleaf(current)){ 
            // cout << vec_1.nodemap(current) << endl;
            lcas = vec_2.lca(lcas, vec_2.nodeselect(hash_table[array1[vec_1.nodemap(current) - 1]]));
        }
        else{
            // cout << vec_1.nodemap(current) << endl;
            lcas_aux = rf_2(vec_1, vec_2, array1, hash_table, vec_1.first_child(current));
            lcas = vec_2.lca(lcas, lcas_aux);
            // cout << "Comparission between: " << vec_1.nodemap(current) << " and " << vec_2.nodemap(lcas_aux) << endl;
            if(vec_1.cluster_size(current) != vec_2.cluster_size(lcas_aux)){
                clusters_1.push_back(vec_1.nodemap(current));
                clusters_2.push_back(vec_2.nodemap(lcas_aux));
                rf_dist++;
            }
        }
    }
    return lcas;
}


// with internal nodes
int rf(bp_support_pedro<> vec_1, bp_support_pedro<> vec_2, string* array1, unordered_map<string, int> &hash_table, int current){
    int lcas, lcas_aux, x;

    if(vec_1.isleaf(current)){
        // cout << vec_1.nodemap(current) << endl;
        lcas = vec_2.nodeselect(hash_table[array1[vec_1.nodemap(current) - 1]]);
    }
    else{
        // cout << vec_1.nodemap(current) << endl;
        lcas = vec_2.lca(vec_2.nodeselect(hash_table[array1[vec_1.nodemap(current) - 1]]), rf(vec_1, vec_2, array1, hash_table, vec_1.first_child(current)));
        // cout << "Comparission between: " << vec_1.nodemap(current) << " and " << vec_2.nodemap(lcas) << endl;
        if(vec_1.cluster_size(current) != vec_2.cluster_size(lcas)){
            clusters_1.push_back(vec_1.nodemap(current));
            clusters_2.push_back(vec_2.nodemap(lcas));
            rf_dist++;
        }
    }
    

    while((x = vec_1.next_sibling(current)) != -1){
        current = x;
        if(vec_1.isleaf(current)){ 
            // cout << vec_1.nodemap(current) << endl;
            lcas = vec_2.lca(lcas, vec_2.nodeselect(hash_table[array1[vec_1.nodemap(current) - 1]]));
        }
        else{
            // cout << vec_1.nodemap(current) << endl;
            lcas_aux = rf(vec_1, vec_2, array1, hash_table, vec_1.first_child(current));
            lcas = vec_2.lca(lcas, lcas_aux);
            // cout << "Comparission between: " << vec_1.nodemap(current) << " and " << vec_2.nodemap(lcas_aux) << endl;
            if(vec_1.cluster_size(current) != vec_2.cluster_size(lcas_aux)){
                clusters_1.push_back(vec_1.nodemap(current));
                clusters_2.push_back(vec_2.nodemap(lcas_aux));
                rf_dist++;
            }
        }
    }
    return lcas;
}

int rf_weighted(bp_support_pedro<> vec_1, bp_support_pedro<> vec_2, string* array1, unordered_map<string, int> &hash_table, int current){
    int lcas, lcas_aux, x;
    float w1, w2;

    if(vec_1.isleaf(current)){
        // cout << vec_1.nodemap(current) << endl;
        lcas = vec_2.nodeselect(hash_table[array1[vec_1.nodemap(current) - 1]]);
        w1 = weights1[vec_1.nodemap(current) - 1];
        w2 = weights2[vec_2.nodemap(lcas) - 1];
        rf_dist = rf_dist - (w1 + w2 - abs(w1 - w2));
    }
    else{
        // cout << vec_1.nodemap(current) << endl;
        lcas = vec_2.lca(vec_2.nodeselect(hash_table[array1[vec_1.nodemap(current) - 1]]), rf(vec_1, vec_2, array1, hash_table, vec_1.first_child(current)));
        // cout << "Comparission between: " << vec_1.nodemap(current) << " and " << vec_2.nodemap(lcas) << endl;
        if(vec_1.cluster_size(current) == vec_2.cluster_size(lcas)){
            w1 = weights1[vec_1.nodemap(current) - 1];
            w2 = weights2[vec_2.nodemap(lcas) - 1];
            rf_dist = rf_dist - (w1 + w2 - abs(w1 - w2));
        }
    }
    

    while((x = vec_1.next_sibling(current)) != -1){
        current = x;
        if(vec_1.isleaf(current)){ 
            // cout << vec_1.nodemap(current) << endl;
            lcas = vec_2.lca(lcas, vec_2.nodeselect(hash_table[array1[vec_1.nodemap(current) - 1]]));
            w1 = weights1[vec_1.nodemap(current) - 1];
            w2 = weights2[vec_2.nodemap(lcas) - 1];
            rf_dist = rf_dist - (w1 + w2 - abs(w1 - w2));
        }
        else{
            // cout << vec_1.nodemap(current) << endl;
            lcas_aux = rf(vec_1, vec_2, array1, hash_table, vec_1.first_child(current));
            lcas = vec_2.lca(lcas, lcas_aux);
            // cout << "Comparission between: " << vec_1.nodemap(current) << " and " << vec_2.nodemap(lcas_aux) << endl;
            if(vec_1.cluster_size(current) == vec_2.cluster_size(lcas_aux)){
                w1 = weights1[vec_1.nodemap(current) - 1];
                w2 = weights2[vec_2.nodemap(lcas) - 1];
                rf_dist = rf_dist - (w1 + w2 - abs(w1 - w2));
            }
        }
    }
    return lcas;
}


int main(int argc, char* argv[]){

    array1 = (string*)malloc(20000 * sizeof(string));
    array2 = (string*)malloc(20000 * sizeof(string));

    weights1 = (float*)malloc(20000 * sizeof(float));
    weights2 = (float*)malloc(20000 * sizeof(float));


    // bit_vector v_1 = create_bit_vector_2(argv[1], array1, weights1, hash_table1, hash_table1_aux);
    // bit_vector v_2 = create_bit_vector_2(argv[2], array2, weights2, hash_table2, hash_table2_aux);
    
    bit_vector v_1 = create_bit_vector(argv[1], array1, weights1, hash_table1, array1_count, weights1_count);
    bit_vector v_2 = create_bit_vector(argv[2], array2, weights2, hash_table2, array2_count, weights2_count);

    cout << "chegou: " << array1_count << endl;

    bp_support_pedro<> vec_1(&v_1);
    bp_support_pedro<> vec_2(&v_2);


    auto start = std::chrono::system_clock::now();
    // Some computation here
    if(internal_nodes){
        if(weights_bool){
            rf_weighted(vec_1, vec_2, array1, hash_table2, 0);
        }
        else{
            rf(vec_1, vec_2, array1, hash_table2, 0);
            cout << "Robinson Foulds distance is: " << rf_dist << endl;
            // cout << "clusters differents in " << argv[1] << ":" << endl;
            // for (int x : clusters_1) {
            //     cout << x << endl;
            // }
        }
    }
    else{
        if(weights_bool){
            rf_weighted_2(vec_1, vec_2, array1, hash_table2, 0);
        }
        else{
            rf_2(vec_1, vec_2, array1, hash_table2, 0);
            cout << "distance = " << rf_dist << endl;
            // cout << "[1]" << endl;
            // cout << "Robinson Foulds distance is: " << rf_dist << endl;
            // cout << "clusters differents in " << argv[1] << ":" << endl;
            // for (int x : clusters_1) {
            //     cout << x << endl;
            // }
        }
    }
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    cout << "elapsed time: " << elapsed_seconds.count() << "s" << endl;
    return 0;
}
