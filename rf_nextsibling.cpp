#include <string>
#include <stack>

#include "bp_support_pedro2.hpp"

using namespace std;
using namespace sdsl;



/*
Saves the weight found inside a label.
*/
void get_weight(FILE* fp, vector<float> &w, char c, int count, bool &weighted_trees, float &w_rf_dist){
    string str = "";
    float weight;
    while(!(c == ')' || c == '(' || c == ',' || c == ';')){
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
        str.append(1, c);
        c = getc(fp);
    }
    if(str != "_"){
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
This function computes recieves a newick format and returns a bit vector that represents that tree in a balanced parenthesis representation.
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
        if(c == '('){
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
            add_bit(bv, count2, 1);
            add_bit(bv, count2, 0);
            get_string(fp, c, count, strings, code, num_tree, internal_nodes, weighted_trees, w_rf_dist, w1, w2);
            count++;
        }
    }
    fclose(fp);
    return bv;
}

/*
This function computes the Robinson Foulds distance between weighted phylogenetic trees.
It uses the NextSibling and the FirstChild operations to traverse the tree in a post-order traversal.
*/
int weighted_rf_onlyleaves(bp_support_pedro2<> &vec_1, bp_support_pedro2<> &vec_2, int_vector<32>& code, int current, float &w_rf_dist, vector<int> &clusters_1, vector<int> &clusters_2, vector<float>& w1, vector<float>& w2){
    int lcas, lcas_aux, x;
    
    float weight1, weight2;
    
    if(vec_1.isleaf(current)){
        lcas = vec_2.nodeselect(code[vec_1.nodemap(current) - 1] + 1);
        weight1 = w1[vec_1.nodemap(current) - 1];
        weight2 = w2[vec_2.nodemap(lcas) - 1];
        w_rf_dist = w_rf_dist - (weight1 + weight2 - abs(weight1 - weight2));
    }
    else{
        lcas = weighted_rf_onlyleaves(vec_1, vec_2, code, vec_1.first_child(current), w_rf_dist, clusters_1, clusters_2, w1, w2);
        //  cout << "Comparission between: " << vec_1.nodemap(current) << " and " << vec_2.nodemap(lcas) << endl;
        if(vec_1.num_leaves(current) == vec_2.num_leaves(lcas)){
            weight1 = w1[vec_1.nodemap(current) - 1];
            weight2 = w2[vec_2.nodemap(lcas) - 1];
            w_rf_dist = w_rf_dist - (weight1 + weight2 - abs(weight1 - weight2));
            clusters_1.push_back(vec_1.nodemap(current));
            clusters_2.push_back(vec_2.nodemap(lcas));
        }
    }
    

    while((x = vec_1.next_sibling(current)) != -1){
        current = x;
        if(vec_1.isleaf(current)){ 
            lcas_aux = vec_2.nodeselect(code[vec_1.nodemap(current) - 1] + 1);
            lcas = vec_2.lca(lcas, lcas_aux);
            weight1 = w1[vec_1.nodemap(current) - 1];
            weight2 = w2[vec_2.nodemap(lcas_aux) - 1];
            w_rf_dist = w_rf_dist - (weight1 + weight2 - abs(weight1 - weight2));
        }
        else{
            lcas_aux = weighted_rf_onlyleaves(vec_1, vec_2, code, vec_1.first_child(current), w_rf_dist, clusters_1, clusters_2, w1, w2);
            lcas = vec_2.lca(lcas, lcas_aux);
            //  cout << "Comparission between: " << vec_1.nodemap(current) << " and " << vec_2.nodemap(lcas_aux) << endl;
            if(vec_1.num_leaves(current) == vec_2.num_leaves(lcas_aux)){
                weight1 = w1[vec_1.nodemap(current) - 1];
                weight2 = w2[vec_2.nodemap(lcas_aux) - 1];
                w_rf_dist = w_rf_dist - (weight1 + weight2 - abs(weight1 - weight2));
                clusters_1.push_back(vec_1.nodemap(current));
                clusters_2.push_back(vec_2.nodemap(lcas_aux));
            }
        }
    }
    return lcas;
}

/*
This function computes the Robinson Foulds distance between phylogenetic trees with taxa only on the leaves.
It uses the NextSibling and the FirstChild operations to traverse the tree in a post-order traversal.
*/
int rf_onlyleaves(bp_support_pedro2<> &vec_1, bp_support_pedro2<> &vec_2, int_vector<32>& code, int current, int &rf_dist, vector<int> &clusters_1, vector<int> &clusters_2){
    int lcas, lcas_aux, x;
    
    if(vec_1.isleaf(current)){
        lcas = vec_2.nodeselect(code[vec_1.nodemap(current) - 1] + 1);
    }
    else{
        lcas = rf_onlyleaves(vec_1, vec_2, code, vec_1.first_child(current), rf_dist, clusters_1, clusters_2);
        //  cout << "Comparission between: " << vec_1.nodemap(current) << " and " << vec_2.nodemap(lcas) << endl;
        if(vec_1.num_leaves(current) == vec_2.num_leaves(lcas)){
            rf_dist++;
            clusters_1.push_back(vec_1.nodemap(current));
            clusters_2.push_back(vec_2.nodemap(lcas));
        }
    }
    

    while((x = vec_1.next_sibling(current)) != -1){
        current = x;
        if(vec_1.isleaf(current)){ 
            lcas = vec_2.lca(lcas, vec_2.nodeselect(code[vec_1.nodemap(current) - 1] + 1));
        }
        else{
            lcas_aux = rf_onlyleaves(vec_1, vec_2, code, vec_1.first_child(current), rf_dist, clusters_1, clusters_2);
            lcas = vec_2.lca(lcas, lcas_aux);
            //  cout << "Comparission between: " << vec_1.nodemap(current) << " and " << vec_2.nodemap(lcas_aux) << endl;
            if(vec_1.num_leaves(current) == vec_2.num_leaves(lcas_aux)){
                rf_dist++;
                clusters_1.push_back(vec_1.nodemap(current));
                clusters_2.push_back(vec_2.nodemap(lcas_aux));
            }
        }
    }
    return lcas;
}

/*
This function computes the Robinson Foulds distance between fully labelled weighted phylogenetic trees.
It uses the NextSibling and the FirstChild operations to traverse the tree in a post-order traversal.
*/
int weighted_rf_allnodes(bp_support_pedro2<> &vec_1, bp_support_pedro2<> &vec_2, int_vector<32>& code, int current, float &w_rf_dist, vector<int> &clusters_1, vector<int> &clusters_2, vector<float>& w1, vector<float>& w2){
    int lcas, lcas_aux, x;
    float weight1, weight2;

    if(vec_1.isleaf(current)){
        lcas = vec_2.nodeselect(code[vec_1.nodemap(current) - 1] + 1);
        weight1 = w1[vec_1.nodemap(current) - 1];
        weight2 = w2[vec_2.nodemap(lcas) - 1];
        w_rf_dist = w_rf_dist - (weight1 + weight2 - abs(weight1 - weight2));
    }
    else{
        lcas = vec_2.lca(vec_2.nodeselect(code[vec_1.nodemap(current) - 1] + 1), weighted_rf_allnodes(vec_1, vec_2, code, vec_1.first_child(current), w_rf_dist, clusters_1, clusters_2, w1, w2));
        // cout << "Comparission between: " << vec_1.nodemap(current) << " and " << vec_2.nodemap(lcas) << endl;
        if(vec_1.cluster_size(current) == vec_2.cluster_size(lcas)){
            weight1 = w1[vec_1.nodemap(current) - 1];
            weight2 = w2[vec_2.nodemap(lcas) - 1];
            w_rf_dist = w_rf_dist - (weight1 + weight2 - abs(weight1 - weight2));
            clusters_1.push_back(vec_1.nodemap(current));
            clusters_2.push_back(vec_2.nodemap(lcas));
        }
    }
    

    while((x = vec_1.next_sibling(current)) != -1){
        current = x;
        if(vec_1.isleaf(current)){ 
            lcas_aux = vec_2.nodeselect(code[vec_1.nodemap(current) - 1] + 1);
            lcas = vec_2.lca(lcas, lcas_aux);
            weight1 = w1[vec_1.nodemap(current) - 1];
            weight2 = w2[vec_2.nodemap(lcas_aux) - 1];
            w_rf_dist = w_rf_dist - (weight1 + weight2 - abs(weight1 - weight2));
        }
        else{
            lcas_aux = vec_2.lca(vec_2.nodeselect(code[vec_1.nodemap(current) - 1] + 1), weighted_rf_allnodes(vec_1, vec_2, code, vec_1.first_child(current), w_rf_dist, clusters_1, clusters_2, w1, w2));
            lcas = vec_2.lca(lcas, lcas_aux);
            // cout << "Comparission between: " << vec_1.nodemap(current) << " and " << vec_2.nodemap(lcas_aux) << endl;
            if(vec_1.cluster_size(current) == vec_2.cluster_size(lcas_aux)){
                weight1 = w1[vec_1.nodemap(current) - 1];
                weight2 = w2[vec_2.nodemap(lcas_aux) - 1];
                w_rf_dist = w_rf_dist - (weight1 + weight2 - abs(weight1 - weight2));
                clusters_1.push_back(vec_1.nodemap(current));
                clusters_2.push_back(vec_2.nodemap(lcas_aux));
            }
        }
    }
    return lcas;
}


/*
This function computes the Robinson Foulds distance between fully labelled phylogenetic trees.
It uses the NextSibling and the FirstChild operations to traverse the tree in a post-order traversal.
*/
// with internal nodes (next_sibling implementation)
int rf_allnodes(bp_support_pedro2<> &vec_1, bp_support_pedro2<> &vec_2, int_vector<32>& code, int current, int &rf_dist, vector<int> &clusters_1, vector<int> &clusters_2){
    int lcas, lcas_aux, x;

    if(vec_1.isleaf(current)){
        lcas = vec_2.nodeselect(code[vec_1.nodemap(current) - 1] + 1);
    }
    else{
        lcas = vec_2.lca(vec_2.nodeselect(code[vec_1.nodemap(current) - 1] + 1), rf_allnodes(vec_1, vec_2, code, vec_1.first_child(current), rf_dist, clusters_1, clusters_2));
        // cout << "Comparission between: " << vec_1.nodemap(current) << " and " << vec_2.nodemap(lcas) << endl;
        if(vec_1.cluster_size(current) == vec_2.cluster_size(lcas)){
            rf_dist++;
            clusters_1.push_back(vec_1.nodemap(current));
            clusters_2.push_back(vec_2.nodemap(lcas));
        }
    }
    

    while((x = vec_1.next_sibling(current)) != -1){
        current = x;
        if(vec_1.isleaf(current)){ 
            lcas = vec_2.lca(lcas, vec_2.nodeselect(code[vec_1.nodemap(current) - 1] + 1));
        }
        else{
            lcas_aux = vec_2.lca(vec_2.nodeselect(code[vec_1.nodemap(current) - 1] + 1), rf_allnodes(vec_1, vec_2, code, vec_1.first_child(current), rf_dist, clusters_1, clusters_2));
            lcas = vec_2.lca(lcas, lcas_aux);
            // cout << "Comparission between: " << vec_1.nodemap(current) << " and " << vec_2.nodemap(lcas_aux) << endl;
            if(vec_1.cluster_size(current) == vec_2.cluster_size(lcas_aux)){
                rf_dist++;
                clusters_1.push_back(vec_1.nodemap(current));
                clusters_2.push_back(vec_2.nodemap(lcas_aux));
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
    bool weighted_trees = false;

    float w_rf_dist = 0;

    vector<int> clusters_1;
    vector<int> clusters_2;

    unordered_map<string, int> strings;

    int_vector<32> code;

    vector<float> w1;
    vector<float> w2;

    auto start = std::chrono::system_clock::now();
    
    bit_vector v_1 = create_bit_vector(argv[1], strings, code, 0, num_nodes1, internal_nodes, weighted_trees, w_rf_dist, w1, w2);
    bit_vector v_2 = create_bit_vector(argv[2], strings, code, 1, num_nodes2, internal_nodes, weighted_trees, w_rf_dist, w1, w2);

    bp_support_pedro2<> vec_1(&v_1);
    bp_support_pedro2<> vec_2(&v_2);

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    // cout << "parse time: " << elapsed_seconds.count() << "s" << endl;

    auto start2 = std::chrono::system_clock::now();

    if(internal_nodes){
        if(weighted_trees){
            weighted_rf_allnodes(vec_1, vec_2, code, 0, w_rf_dist, clusters_1, clusters_2, w1, w2);
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
            rf_allnodes(vec_1, vec_2, code, 0, rf_dist, clusters_1, clusters_2);
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
            weighted_rf_onlyleaves(vec_1, vec_2, code, 0, w_rf_dist, clusters_1, clusters_2, w1, w2);
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
            rf_onlyleaves(vec_1, vec_2, code, 0, rf_dist, clusters_1, clusters_2);
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
