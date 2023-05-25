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

// without internal nodes (post_order implementation)
void rf_2(bp_support_pedro2<> vec_1, bp_support_pedro2<> vec_2, vector<int>& code, int size, int &rf_dist, vector<int> &clusters_1){
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
void rf(bp_support_pedro2<> vec_1, bp_support_pedro2<> vec_2, vector<int>& code, int size, int &rf_dist, vector<int> &clusters_1){
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


int main(int argc, char* argv[]){

    int rf_dist = 0;

    int num_nodes1 = 0;
    int num_nodes2 = 0;

    int size;

    bool internal_nodes = false;

    vector<int> clusters_1;
    vector<int> clusters_2;

    unordered_map<string, int> strings;

    vector<int> code;

    auto start = std::chrono::system_clock::now();
    
    bit_vector v_1 = create_bit_vector(argv[1], strings, code, 0, num_nodes1, internal_nodes);
    bit_vector v_2 = create_bit_vector(argv[2], strings, code, 1, num_nodes2, internal_nodes);

    size = v_1.size() / 2;

    bp_support_pedro2<> vec_1(&v_1);
    bp_support_pedro2<> vec_2(&v_2);

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    // cout << "parse time: " << elapsed_seconds.count() << "s" << endl;

    auto start2 = std::chrono::system_clock::now();
    // Some computation here
    if(internal_nodes){
        rf(vec_1, vec_2, code, size, rf_dist, clusters_1);
        cout << "Robinson Foulds distance is: " << float(num_nodes1 + num_nodes2 - rf_dist*2) / 2 << endl;
        // cout << "clusters differents in " << argv[1] << ":" << endl;
        // for (int i = 0; i < clusters_1.size(); i++){
        //     cout << clusters_1[i] << endl;
        // }
    }
    else{
        rf_2(vec_1, vec_2, code, size, rf_dist, clusters_1);
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
    // cout << "algorithm time: " << elapsed_seconds2.count() << "s" << endl;
    // cout << elapsed_seconds2.count() << ", ";
    return 0;
}
