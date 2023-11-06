#include <string>
#include <stack>

#include "bp_support_sada_extended.hpp"

using namespace std;
using namespace sdsl;

struct Struct {
    int lca;
    int size;
};

/*
Saves the weight found inside a label.
*/
void get_weight(FILE* fp, vector<float> &w, char c, int count, bool &weighted_trees, float &w_rf_dist){
    string str = "";
    float weight;
    while(!(c == ')' || c == '(' || c == ',' || c == ';')){
        if(c == EOF){
            cout << "Program did not found ';' in the newick format." << endl;
            exit(1);
        }
        str.append(1, c);
        c = getc(fp);
    }
    weight = stof(str);
    if(int(w.size()) < count + 1){
        w.resize(count + 1);
    }
    w[count] = weight;
    w_rf_dist = w_rf_dist + weight;
    weighted_trees = true;
    ungetc(c, fp);
}
/*
Looks for the label of a given node and does the correlation between trees using a hash table.
*/
void get_string(FILE* fp, char c, int count, unordered_map<string, int>& strings, int_vector<32>& code, int num_tree, bool &internal_nodes, bool &weighted_trees, float &w_rf_dist, vector<float>& w1, vector<float>& w2){
    string str = "";
    int aux;
    while(!(c == ')' || c == '(' || c == ',' || c == ';' || c == ':')){
        if(c == EOF){
            cout << "Program did not found ';' in the newick format." << endl;
            exit(1);
        }
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
            if(int(code.size()) < aux + 1){
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

/*
Adds a bit with value i to the bit vector received as input.
*/
void add_bit(bit_vector& bv, int &count, int i){
    bv.resize(bv.size() + 1);
    bv[bv.size() - 1] = i;
}

/*
This function recieves a newick format and returns a bit vector that represents that tree in a balanced parenthesis representation.
*/
bit_vector create_bit_vector(char* tree, unordered_map<string, int>& strings, int_vector<32>& code, int num_tree, int &num_nodes, bool &internal_nodes, bool &weighted_trees, float &w_rf_dist, vector<float>& w1, vector<float>& w2){
    bit_vector bv = bit_vector(0);
    stack<int> stack;
    string str = "";
    int count = 0;
    int count2 = 0;
    char c;
    FILE* fp;
    fp = fopen(tree, "r");
    while((c=getc(fp)) != ';'){
        if(c == EOF){
            cout << "Program did not found ';' in the newick format for tree: " << tree << endl;
            exit(1);
        }
        else if(c == '('){
            stack.push(count);
            count++;
            add_bit(bv, count2, 1);
        }
        else if(c == ','){
            continue;
        }
        else if(c == ')'){
            num_nodes++;
            c = getc(fp);
            if(c == EOF){
                cout << "Program did not found ';' in the newick format for tree: " << tree << endl;
                exit(1);
            }
            if(!(c == ')' || c == '(' || c == ',' || c == ';')){
                if(c == ':'){
                    c = getc(fp);
                    if(c == EOF){
                        cout << "Program did not found ';' in the newick format for tree: " << tree << endl;
                        exit(1);
                    }
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
            add_bit(bv, count2, 0);
        }
        else{
            // cout << "c = " << c << endl;
            add_bit(bv, count2, 1);
            add_bit(bv, count2, 0);
            get_string(fp, c, count, strings, code, num_tree, internal_nodes, weighted_trees, w_rf_dist, w1, w2);
            count++;
        }
    }
    fclose(fp);
    // cout << bv << endl;
    // cout << bv.size() << endl;
    return bv;
}

/*
This function computes the Robinson Foulds distance between weighted phylogenetic trees with taxa only on the leaves.
It uses the PostOrderSelect operation to traverse the tree in a post-order traversal.
*/
void weighted_rf_onlyleaves(bp_support_pedro2<>& vec_1, bp_support_pedro2<>& vec_2, int_vector<32>& code, int size, float &w_rf_dist, vector<int> &clusters_1, vector<int> &clusters_2, vector<float>& w1, vector<float>& w2){
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
                clusters_1.push_back(vec_1.nodemap(post_current));
                clusters_2.push_back(vec_2.nodemap(lcas));
            }
            lcas = -1;
        }
    }
}

/*
This function computes the Robinson Foulds distance between weighted fully labelled phylogenetic trees.
It uses the PostOrderSelect operation to traverse the tree in a post-order traversal.
*/
void weighted_rf_allnodes(bp_support_pedro2<>& vec_1, bp_support_pedro2<>& vec_2, int_vector<32>& code, int size, float &w_rf_dist, vector<int> &clusters_1, vector<int> &clusters_2, vector<float>& w1, vector<float>& w2){
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
            first.size = 1;
            weight1 = w1[vec_1.nodemap(post_current) - 1];
            weight2 = w2[vec_2.nodemap(first.lca) - 1];
            w_rf_dist = w_rf_dist - (weight1 + weight2 - abs(weight1 - weight2));
            // cout << "first.lca = " << vec_2.nodemap(first.lca) << endl;
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
            if(size_cluster == int(vec_2.cluster_size(lcas))){
                weight1 = w1[vec_1.nodemap(post_current) - 1];
                weight2 = w2[vec_2.nodemap(lcas) - 1];
                w_rf_dist = w_rf_dist - (weight1 + weight2 - abs(weight1 - weight2));
                clusters_1.push_back(vec_1.nodemap(post_current));
                clusters_2.push_back(vec_2.nodemap(lcas));
            }
            lcas = -1;
        }
    }
}

/*
This function computes the Robinson Foulds distance between phylogenetic trees with taxa only on the leaves.
It uses the PostOrderSelect operation to traverse the tree in a post-order traversal.
*/
void rf_onlyleaves(bp_support_pedro2<>& vec_1, bp_support_pedro2<>& vec_2, int_vector<32>& code, int size, int &rf_dist, vector<int> &clusters_1, vector<int> &clusters_2){
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
                clusters_1.push_back(vec_1.nodemap(post_current));
                clusters_2.push_back(vec_2.nodemap(lcas));
            }
            lcas = -1;
        }
    }
}

/*
This function computes the Robinson Foulds distance between fully labelled phylogenetic trees.
It uses the PostOrderSelect operation to traverse the tree in a post-order traversal.
*/
void rf_allnodes(bp_support_pedro2<>& vec_1, bp_support_pedro2<>& vec_2, int_vector<32>& code, int size, int &rf_dist, vector<int> &clusters_1, vector<int> &clusters_2){
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
            if(size_cluster == int(vec_2.cluster_size(lcas))){
                rf_dist++;
                clusters_1.push_back(vec_1.nodemap(post_current));
                clusters_2.push_back(vec_2.nodemap(lcas));
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

    float w_rf_dist = 0;

    bool internal_nodes = false;
    bool weighted_trees = false;

    vector<int> clusters_1;
    vector<int> clusters_2;

    unordered_map<string, int> strings;

    int_vector<32> code;

    vector<float> w1;
    vector<float> w2;

    auto start = std::chrono::system_clock::now();
    
    bit_vector v_1 = create_bit_vector(argv[1], strings, code, 0, num_nodes1, internal_nodes, weighted_trees, w_rf_dist, w1, w2);
    bit_vector v_2 = create_bit_vector(argv[2], strings, code, 1, num_nodes2, internal_nodes, weighted_trees, w_rf_dist, w1, w2);

    strings.clear();    

    size = v_1.size() / 2;

    bp_support_pedro2<> vec_1(&v_1);
    bp_support_pedro2<> vec_2(&v_2);

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    // cout << "parse time: " << elapsed_seconds.count() << "s" << endl;
    // exit(0);

    // rf_onlyleaves(vec_1, vec_2, code, size, rf_dist, clusters_1, clusters_2);
    // cout << "Robinson Foulds distance is: " << float(num_nodes1 + num_nodes2 - rf_dist*2) / 2 << endl;

    // exit(0);

    auto start2 = std::chrono::system_clock::now();
    // Some computation here
    if(internal_nodes){
        if(weighted_trees){
            weighted_rf_allnodes(vec_1, vec_2, code, size, w_rf_dist, clusters_1, clusters_2, w1, w2);
            cout << "Weighted Robinson Foulds distance is: " << w_rf_dist / 2 << endl;
            if(argc > 3 && !strcmp(argv[3], "info")){
                cout << "Indexes of clusters that are differents in " << argv[1] << ":" << endl;
                for (int i = 1; i <= int(v_1.size()) / 2; i++){
                    if((!(find(clusters_1.begin(), clusters_1.end(), i) != clusters_1.end())) && (!vec_1.isleaf(vec_1.nodeselect(i)))){
                        cout << i << endl;
                    }
                }
                cout << "Indexes of clusters that are differents in " << argv[2] << ":" << endl;
                for (int i = 1; i <= int(v_2.size()) / 2; i++){
                    if((!(find(clusters_2.begin(), clusters_2.end(), i) != clusters_2.end())) && (!vec_2.isleaf(vec_2.nodeselect(i)))){
                        cout << i << endl;
                    }
                }
            }
        }
        else{
            rf_allnodes(vec_1, vec_2, code, size, rf_dist, clusters_1, clusters_2);
            cout << "Robinson Foulds distance is: " << float(num_nodes1 + num_nodes2 - rf_dist*2) / 2 << endl;
            if(argc > 3 && !strcmp(argv[3], "info")){
                cout << "Indexes of clusters that are differents in " << argv[1] << ":" << endl;
                for (int i = 1; i <= int(v_1.size()) / 2; i++){
                    if((!(find(clusters_1.begin(), clusters_1.end(), i) != clusters_1.end())) && (!vec_1.isleaf(vec_1.nodeselect(i)))){
                        cout << i << endl;
                    }
                }
                cout << "Indexes of clusters that are differents in " << argv[2] << ":" << endl;
                for (int i = 1; i <= int(v_2.size()) / 2; i++){
                    if((!(find(clusters_2.begin(), clusters_2.end(), i) != clusters_2.end())) && (!vec_2.isleaf(vec_2.nodeselect(i)))){
                        cout << i << endl;
                    }
                }
            }
        }
    }
    else{
        if(weighted_trees){
            weighted_rf_onlyleaves(vec_1, vec_2, code, size, w_rf_dist, clusters_1, clusters_2, w1, w2);
            cout << "Weighted Robinson Foulds distance is: " << w_rf_dist / 2 << endl;
            if(argc > 3 && !strcmp(argv[3], "info")){
                cout << "Indexes of clusters that are differents in " << argv[1] << ":" << endl;
                for (int i = 1; i <= int(v_1.size()) / 2; i++){
                    if((!(find(clusters_1.begin(), clusters_1.end(), i) != clusters_1.end())) && (!vec_1.isleaf(vec_1.nodeselect(i)))){
                        cout << i << endl;
                        cout << w1[i-1] << endl;
                    }
                }
                cout << "Indexes of clusters that are differents in " << argv[2] << ":" << endl;
                for (int i = 1; i <= int(v_2.size()) / 2; i++){
                    if((!(find(clusters_2.begin(), clusters_2.end(), i) != clusters_2.end())) && (!vec_2.isleaf(vec_2.nodeselect(i)))){
                        cout << i << endl;
                        cout << w2[i-1] << endl;
                    }
                }
            }

        }
        else{
            rf_onlyleaves(vec_1, vec_2, code, size, rf_dist, clusters_1, clusters_2);
            cout << "Robinson Foulds distance is: " << float(num_nodes1 + num_nodes2 - rf_dist*2) / 2 << endl;
            if(argc > 3 && !strcmp(argv[3], "info")){
                cout << "Indexes of clusters that are differents in " << argv[1] << ":" << endl;
                for (int i = 1; i <= int(v_1.size()) / 2; i++){
                    if((!(find(clusters_1.begin(), clusters_1.end(), i) != clusters_1.end())) && (!vec_1.isleaf(vec_1.nodeselect(i)))){
                        cout << i << endl;
                    }
                }
                cout << "Indexes of clusters that are differents in " << argv[2] << ":" << endl;
                for (int i = 1; i <= int(v_2.size()) / 2; i++){
                    if((!(find(clusters_2.begin(), clusters_2.end(), i) != clusters_2.end())) && (!vec_2.isleaf(vec_2.nodeselect(i)))){
                        cout << i << endl;
                    }
                }
            }
        }
    }
    auto end2 = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds2 = end2-start2;
    // cout << "algorithm time: " << elapsed_seconds2.count() << "s" << endl;
    // cout << elapsed_seconds2.count() << ", ";
    return 0;
}
