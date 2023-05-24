#include <string>
#include <stack>

#include "bp_support_pedro2.hpp"

using namespace std;
using namespace sdsl;


struct Struct {
    int lca;
    int size;
};

void get_string(FILE* fp, char c, int count, unordered_map<string, int>& strings, vector<int>& code, int num_tree, bool &internal_nodes){
    string str = "";
    int aux;
    while(!(c == ')' || c == '(' || c == ',' || c == ';')){
        str.append(1, c);
        c = getc(fp);
    }
    if(str != "_"){
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
        internal_nodes = false;
    }
    ungetc(c, fp);
}

void add_bit(bit_vector& bv, int i){
    bv.resize(bv.size() + 1);
    bv[bv.size() - 1] = i;
}

bit_vector create_bit_vector(char* tree, unordered_map<string, int>& strings, vector<int>& code, int num_tree, int &num_nodes, bool &internal_nodes){
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
                get_string(fp, c, stack.top(), strings, code, num_tree, internal_nodes);
                internal_nodes = true;
            }
            else{
                ungetc(c, fp);
            }
            stack.pop();
            add_bit(bv, 0);
        }
        else{
            add_bit(bv, 1);
            add_bit(bv, 0);
            get_string(fp, c, count, strings, code, num_tree, internal_nodes);
            count++;
        }
    }
    fclose(fp);
    // cout << bv << endl;
    // cout << bv.size() << endl;
    return bv;
}


// without internal nodes (next_sibling implementation)
int rf_2(bp_support_pedro2<> vec_1, bp_support_pedro2<> vec_2, vector<int>& code, int current, int &rf_dist, vector<int> &clusters_1){
    int lcas, lcas_aux, x;
    
    if(vec_1.isleaf(current)){
        lcas = vec_2.nodeselect(code[vec_1.nodemap(current) - 1] + 1);
    }
    else{
        lcas = rf_2(vec_1, vec_2, code, vec_1.first_child(current), rf_dist, clusters_1);
        //  cout << "Comparission between: " << vec_1.nodemap(current) << " and " << vec_2.nodemap(lcas) << endl;
        if(vec_1.num_leaves(current) == vec_2.num_leaves(lcas)){
            rf_dist++;
        }
        else{
            // cout << vec_1.nodemap(current) << endl;
            // cout << vec_1.num_leaves(current) << "    " << vec_2.num_leaves(lcas) << endl;
            clusters_1.push_back(vec_1.nodemap(current));
        }
    }
    

    while((x = vec_1.next_sibling(current)) != -1){
        current = x;
        if(vec_1.isleaf(current)){ 
            lcas = vec_2.lca(lcas, vec_2.nodeselect(code[vec_1.nodemap(current) - 1] + 1));
        }
        else{
            lcas_aux = rf_2(vec_1, vec_2, code, vec_1.first_child(current), rf_dist, clusters_1);
            lcas = vec_2.lca(lcas, lcas_aux);
            //  cout << "Comparission between: " << vec_1.nodemap(current) << " and " << vec_2.nodemap(lcas_aux) << endl;
            if(vec_1.num_leaves(current) == vec_2.num_leaves(lcas_aux)){
                rf_dist++;
            }
            else{
                clusters_1.push_back(vec_1.nodemap(current));
            }
        }
    }
    return lcas;
}


// with internal nodes (next_sibling implementation)
int rf(bp_support_pedro2<> vec_1, bp_support_pedro2<> vec_2, vector<int>& code, int current, int &rf_dist, vector<int> &clusters_1){
    int lcas, lcas_aux, x;

    if(vec_1.isleaf(current)){
        lcas = vec_2.nodeselect(code[vec_1.nodemap(current) - 1] + 1);
    }
    else{
        lcas = vec_2.lca(vec_2.nodeselect(code[vec_1.nodemap(current) - 1] + 1), rf(vec_1, vec_2, code, vec_1.first_child(current), rf_dist, clusters_1));
        // cout << "Comparission between: " << vec_1.nodemap(current) << " and " << vec_2.nodemap(lcas) << endl;
        if(vec_1.cluster_size(current) == vec_2.cluster_size(lcas)){
            rf_dist++;
        }
        else{
            clusters_1.push_back(vec_1.nodemap(current));
        }
    }
    

    while((x = vec_1.next_sibling(current)) != -1){
        current = x;
        if(vec_1.isleaf(current)){ 
            lcas = vec_2.lca(lcas, vec_2.nodeselect(code[vec_1.nodemap(current) - 1] + 1));
        }
        else{
            lcas_aux = rf(vec_1, vec_2, code, vec_1.first_child(current), rf_dist, clusters_1);
            lcas = vec_2.lca(lcas, lcas_aux);
            // cout << "Comparission between: " << vec_1.nodemap(current) << " and " << vec_2.nodemap(lcas_aux) << endl;
            if(vec_1.cluster_size(current) == vec_2.cluster_size(lcas_aux)){
                rf_dist++;
            }
            else{
                clusters_1.push_back(vec_1.nodemap(current));
            }
        }
    }
    return lcas;
}


int main(int argc, char* argv[]){

    int rf_dist = 0;

    int num_nodes1 = 0;
    int num_nodes2 = 0;

    bool internal_nodes = false;

    vector<int> clusters_1;
    vector<int> clusters_2;

    unordered_map<string, int> strings;

    vector<int> code;

    auto start = std::chrono::system_clock::now();
    
    bit_vector v_1 = create_bit_vector(argv[1], strings, code, 0, num_nodes1, internal_nodes);
    bit_vector v_2 = create_bit_vector(argv[2], strings, code, 1, num_nodes2, internal_nodes);

    bp_support_pedro2<> vec_1(&v_1);
    bp_support_pedro2<> vec_2(&v_2);

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    cout << "parse time: " << elapsed_seconds.count() << "s" << endl;

    auto start2 = std::chrono::system_clock::now();
    // Some computation here
    if(internal_nodes){
        rf(vec_1, vec_2, code, 0, rf_dist, clusters_1);
        cout << "Robinson Foulds distance is: " << float(num_nodes1 + num_nodes2 - rf_dist*2) / 2 << endl;
        // cout << "clusters differents in " << argv[1] << ":" << endl;
        // for (int i = 0; i < clusters_1.size(); i++){
        //     cout << clusters_1[i] << endl;
        // }
    }
    else{
        rf_2(vec_1, vec_2, code, 0, rf_dist, clusters_1);
        // cout << rf_dist << endl;
        cout << "distance = " << float(num_nodes1 + num_nodes2 - rf_dist*2) / 2 << endl;
        // cout << "[1]" << endl;
        // cout << "Robinson Foulds distance is: " << rf_dist << endl;
        // cout << "clusters differents in " << argv[1] << ":" << endl;
        // for (int i = 0; i < clusters_1.size(); i++){
        //     cout << clusters_1[i] << endl;
        // }
    }
    auto end2 = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds2 = end2-start2;
    cout << "algorithm time: " << elapsed_seconds2.count() << "s" << endl;
    return 0;
}
