#include <string>
#include <stack>

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

void get_string(FILE* fp, char c, int count, unordered_map<string, int>& strings, int_vector<32>& code, int num_tree, bool &internal_nodes, bool &weighted_trees, float &w_rf_dist, vector<float>& w1, vector<float>& w2){
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

void add_bit(bit_vector& bv, int &count, int i){
    bv.resize(bv.size() + 1);
    bv[bv.size() - 1] = i;
}

bit_vector create_bit_vector(char* tree, unordered_map<string, int>& strings, int_vector<32>& code, int num_tree, int &num_nodes, bool &internal_nodes, bool &weighted_trees, float &w_rf_dist, vector<float>& w1, vector<float>& w2){
    bit_vector bv = bit_vector(0);
    stack<int> stack;
    string str = "";
    int count = 0;
    int count2 = 0;
    int state;
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

long compute_shared(int l_blue, int l_red, int r_blue, int r_red){
    return l_red * (r_blue *(r_blue - 1))*0.5 + l_blue * (r_red *(r_red - 1))*0.5 + r_red * (l_blue *(l_blue - 1))*0.5 + r_blue * (l_red *(l_red - 1))*0.5;
}

int node_color(int init, int mid, int last, int node){
    if(node > init && node < mid){
        return 1;
    }
    else if(node >= mid && node < last){
        return 2;
    }
    else{
        return 0;
    }
}


vector<int> count_nodes(bp_support_pedro2<>& vec_1, bp_support_pedro2<>& vec_2, int_vector<32>& code, int root, int init, int mid, int last, int &dist, int branch){
    vector<int> results, results2, results_aux;
    if(vec_1.isleaf(root)){
        results.resize(5);
        results[0] = 1;
        for(int i = 1; i < 5; i++){
            results[i] = 0;
        }
        if(code[vec_1.nodemap(root) - 1] + 1 > init && code[vec_1.nodemap(root) - 1] + 1 < mid){
            results[branch*2 + 1] = 1;
            return results;
        }
        else if(code[vec_1.nodemap(root) - 1] + 1 >= mid && code[vec_1.nodemap(root) - 1] + 1 < last){
            results[branch*2 + 2] = 1;
            return results;
        }
        else{
            return results;
        }
        // node_color(init, mid, last, code[vec_1.nodemap(root) - 1] + 1);
        // return 1;
    }
    else{
        results_aux.resize(5);
        results = count_nodes(vec_1, vec_2, code, root+1, init, mid, last, dist, 0);
        results2 = count_nodes(vec_1, vec_2, code, vec_1.select(vec_1.nodemap(root) + results[0] + 1), init, mid, last, dist, 1);
        results_aux[0] = results[0] + results2[0] + 1;
        results_aux[1] = results[1] + results[3];
        results_aux[2] = results[2] + results[4];
        results_aux[3] = results2[1] + results2[3];
        results_aux[4] = results2[2] + results2[4];
        // for(int i = 1; i < 5; i++){
        //     results[i] = results[i] + results2[i];
        // }
        // cout << results_aux << endl;
        dist = dist + compute_shared(results_aux[1], results_aux[2], results_aux[3], results_aux[4]);
        return results_aux;
    }
}


//n^2
int triplets_onlyleaves(bp_support_pedro2<>& vec_1, bp_support_pedro2<>& vec_2, int_vector<32>& code, int root, int &dist, int size){ // TODO remove size
    int mid;
    int last;
    // cout << "node = " << vec_2.nodemap(root) << endl;
    if(vec_2.isleaf(root)){
        return 1;
    }
    else{
        mid = triplets_onlyleaves(vec_1, vec_2, code, root+1, dist, size) + 1;
        last = mid + triplets_onlyleaves(vec_1, vec_2, code, vec_2.select(vec_2.nodemap(root) + mid), dist, size);
        // cout << "node = " << vec_2.nodemap(root) << " mid = " << vec_2.nodemap(root) + mid << " last = " << vec_2.nodemap(root) + last << endl;
        count_nodes(vec_1, vec_2, code, 0, vec_2.nodemap(root), vec_2.nodemap(root) + mid, vec_2.nodemap(root) + last, dist, 0);
        // cout << "dist = " << dist << endl;
        return last;
    }
}


//nlog(n)
int triplets_onlyleaves_aux(bp_support_pedro2<>& vec_1, bp_support_pedro2<>& vec_2, int_vector<32>& code, int root, int &dist, vector<int> &values){ // TODO remove size
    int mid;
    int last;
    // cout << "node = " << vec_2.nodemap(root) << endl;
    if(vec_2.isleaf(root)){
        return 1;
    }
    else{
        mid = triplets_onlyleaves_aux(vec_1, vec_2, code, root+1, dist, values) + 1;
        last = mid + triplets_onlyleaves_aux(vec_1, vec_2, code, vec_2.select(vec_2.nodemap(root) + mid), dist, values);
        // cout << "node = " << vec_2.nodemap(root) << " mid = " << vec_2.nodemap(root) + mid << " last = " << vec_2.nodemap(root) + last << endl;
        count_nodes(vec_1, vec_2, code, 0, vec_2.nodemap(root), vec_2.nodemap(root) + mid, vec_2.nodemap(root) + last, dist, 0);
        // cout << "dist = " << dist << endl;
        return last;
    }
}

void update_HDT(bp_support_pedro2<>& vec_2, vector<vector<int>> &values, vector<long>& values2, int change, int old_color, int new_color){
    long l_red, l_blue, r_red, r_blue, size;
    // cout << "change = " << change << endl;
    int current = vec_2.nodeselect(change);
    // cout << "current = " << current << endl;
    values[vec_2.nodemap(current) - 1][old_color]--;
    values[vec_2.nodemap(current) - 1][new_color]++;
    while(current != 0){
        current = vec_2.parent2(current);
        values[vec_2.nodemap(current) - 1][old_color]--;
        values[vec_2.nodemap(current) - 1][new_color]++;

        l_red = values[vec_2.nodemap(current + 1) - 1][2];
        l_blue = values[vec_2.nodemap(current + 1) - 1][1];

        size = values[vec_2.nodemap(current + 1) - 1][0] + values[vec_2.nodemap(current + 1) - 1][1] + values[vec_2.nodemap(current + 1) - 1][2];
        size = size + size - 1;


        // cout << "vec_2 = " << vec_2 << endl;
        // cout << vec_2.nodemap(current) << " size = " << size << endl;

        // cout << vec_2.nodemap(current) + size + 1 << endl;

        r_red = values[vec_2.nodemap(current) + size][2];
        r_blue = values[vec_2.nodemap(current) + size][1];

        // values[vec_2.nodemap(current + 1) - 1][0] + values[vec_2.nodemap(current + size + 1) - 1][0]

        // cout << "first = " << vec_2.nodemap(current + 1) << " second = " << vec_2.nodemap(current) + size + 1 << endl;
        values2[vec_2.nodemap(current) - 1] = l_red * (r_blue *(r_blue - 1))*0.5 + l_blue * (r_red *(r_red - 1))*0.5 + r_red * (l_blue *(l_blue - 1))*0.5 + r_blue * (l_red *(l_red - 1))*0.5 + values2[vec_2.nodemap(current + 1) - 1] + values2[vec_2.nodemap(current)  + size];
        // values2[vec_2.nodemap(current) - 1] = l_red * (r_blue *(r_blue - 1))/2 + l_blue * (r_red *(r_red - 1))/2 + r_red * (l_blue *(l_blue - 1))/2 + r_blue * (l_red *(l_red - 1))/2 + values2[vec_2.nodemap(current + 1) - 1] + values2[vec_2.nodemap(current)  + size];
        // cout << "l_red = " << l_red << " l_blue = " << l_blue << " r_red = " << r_red << " r_blue = " << r_blue << " values = " << values2[vec_2.nodemap(current) - 1] << endl;
        // cout << "values" << endl;
        // current = vec_2.parent2(current);
    }
    
    // cout << "\n\nwhite = " << values[0][1] << endl;
    // cout << "blue = " << values[0][2] << endl;
    // cout << "red = " << values[0][3] << "\n\n" << endl;
}


long triplets_coloring(bp_support_pedro2<>& vec_1, bp_support_pedro2<>& vec_2, int_vector<32>& code, vector<vector<int>> &values, vector<long>& values2, int root){
    int size1, size2, second, current;
    long dist = 0;
    if(vec_1.isleaf(root)){
        // cout << "last_leaf = " << code[vec_1.nodemap(root) - 1] + 1 << endl;
        // values[code[vec_1.nodemap(root) - 1]][3]--;
        // values[code[vec_1.nodemap(root) - 1]][1]++;
        update_HDT(vec_2, values, values2, code[vec_1.nodemap(root) - 1] + 1, 2, 0);
        // cout << "leaf = " << vec_1.nodemap(root) << endl;
        return 0;
    }
    second = vec_1.next_sibling(root+1);
    size1 = vec_1.num_leaves(root+1);
    size2 = vec_1.num_leaves(second);
    current = vec_1.nodemap(root);
    if(size1 < size2){
        for(int i = current + 1; i < current + size1 + size1; i++){
            // cout << "i = " << i << endl;
            if(vec_1.isleaf(vec_1.nodeselect(i))){
                //TODO
                // cout << "code = " << code[i-1] << endl;
                update_HDT(vec_2, values, values2, code[i-1] + 1, 2, 1);
            }
        }
        dist = values2[0];
        // cout << "dist_aux = " << dist << endl;
        for(int i = current + 1; i < current + size1 + size1; i++){
            // cout << "i = " << i << endl;
            if(vec_1.isleaf(vec_1.nodeselect(i))){
                //TODO
                update_HDT(vec_2, values, values2, code[i-1] + 1, 1, 0);
            }
        }
        dist = dist + triplets_coloring(vec_1, vec_2, code, values, values2, vec_1.nodeselect(current + size1 + size1));
        for(int i = current + 1; i < current + size1 + size1; i++){
            if(vec_1.isleaf(vec_1.nodeselect(i))){
                //TODO
                update_HDT(vec_2, values, values2, code[i-1] + 1, 0, 2);
            }
        }
        dist = dist + triplets_coloring(vec_1, vec_2, code, values, values2, vec_1.nodeselect(current + 1));
        return dist;
    }
    else{
        for(int i = current + size1 + size1; i < current + size1 + size1 - 1 + size2 + size2; i++){
            // cout << "i = " << i << endl;
            // cout << "current = " << current << endl;
            // cout << "size1 = " << size1 << endl;
            // cout << "size2 = " << size2 << endl;
            if(vec_1.isleaf(vec_1.nodeselect(i))){
                //TODO
                update_HDT(vec_2, values, values2, code[i-1] + 1, 2, 1);
            }
        }
        dist = values2[0];
        // cout << "dist_aux = " << dist << endl;
        for(int i = current + size1 + size1; i < current + size1 + size1 - 1 + size2 + size2; i++){
            // cout << "i = " << i << endl;
            if(vec_1.isleaf(vec_1.nodeselect(i))){
                //TODO
                update_HDT(vec_2, values, values2, code[i-1] + 1, 1, 0);
            }
        }
        dist = dist + triplets_coloring(vec_1, vec_2, code, values, values2, vec_1.nodeselect(current + 1));
        for(int i = current + size1 + size1; i < current + size1 + size1 - 1 + size2 + size2; i++){
            // cout << "i = " << i << endl;
            if(vec_1.isleaf(vec_1.nodeselect(i))){
                //TODO
                update_HDT(vec_2, values, values2, code[i-1] + 1, 0, 2);
            }
        }
        dist = dist + triplets_coloring(vec_1, vec_2, code, values, values2, vec_1.nodeselect(current + size1 + size1));
        return dist;
    }
}

void triplets_onlyleaves_2(bp_support_pedro2<>& vec_1, bp_support_pedro2<>& vec_2, int_vector<32>& code, int root, long &dist, int size){ // TODO remove size
    vector<vector<int>> values;
    vector<long> values2;
    values.resize(size);
    for(int i = 0; i < size; i++){
        values2.push_back(0);
        for(int j = 0; j < 2; j++){
            values[i].push_back(0);
        }
        values[i].push_back(vec_2.num_leaves(vec_2.nodeselect(i+1)));
        // cout << values[i] << endl;
    }
    dist = triplets_coloring(vec_1, vec_2, code, values, values2, 0);
    // cout << "dist = " << dist << endl;
}


int main(int argc, char* argv[]){

    int rf_dist = 0;

    int num_nodes1 = 0;
    int num_nodes2 = 0;

    long triplets_equal = 0;

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

    // cout << v_1.bit_size() << "   " << v_1.capacity() << endl;
    // cout << v_2.bit_size() << "   " << v_2.capacity() << endl;
    // cout << code.size() << "   " << code.capacity() << endl;

    size = v_1.size() / 2;

    bp_support_pedro2<> vec_1(&v_1);
    bp_support_pedro2<> vec_2(&v_2);

    // cout << sizeof(vec_1) << endl;
    // cout << sizeof(vec_2) << endl;

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    // cout << "parse time: " << elapsed_seconds.count() << "s" << endl;

    auto start2 = std::chrono::system_clock::now();

    // Some computation here
    triplets_onlyleaves_2(vec_1, vec_2, code, 0, triplets_equal, size);

    long aux = long(size-num_nodes1);
    // long num_nodes12 = long(num_nodes1);

    // cout << "aquiii" << ((aux) *((aux) - 1)*((aux)-2))/6 << endl;


    cout << "aquiii   " << triplets_equal << endl;

    cout << ((aux) *((aux) - 1)*((aux)-2))/6 - triplets_equal << endl;
    
    auto end2 = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds2 = end2-start2;
    // cout << "algorithm time: " << elapsed_seconds2.count() << "s" << endl;
    cout << elapsed_seconds2.count() << ", ";
    return 0;
}
