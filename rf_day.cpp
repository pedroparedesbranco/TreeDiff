#include <array>
#include <string>
#include <queue>
#include <memory>
#include <cmath>
#include <stack>
#include <unordered_map>
#include <iostream>
#include <list>
#include <chrono>

using namespace std;

struct Struct {
    int left;
    int right;
    int n;
    int weight;
};

/*
Looks for the label of a given node and does the correlation between trees using a hash table.
*/
void get_string(FILE* fp, char c, int size, int num_tree, int &leaf_count, unordered_map<string, int> &strings, vector<int> *sizes, vector<int> *labels){
    string str = "";
    while(!(c == ')' || c == '(' || c == ',' || c == ';')){
        if(c == EOF){
            cout << "Program did not found ';' in the newick format." << endl;
            exit(1);
        }
        str.append(1, c);
        c = getc(fp);
    }
    if(num_tree == 0){
        strings[str] = leaf_count;
        labels[0].push_back(leaf_count);
    }
    else{
        labels[1].push_back(strings[str]);
    }


    sizes[num_tree].push_back(size);
    leaf_count++;
    ungetc(c, fp);
}

/*
This function recieves a newick format and creates a table that represents the tree by saving the label i.
*/
int create_table(FILE* fp, int size, int num_tree, int &leaf_count, int &internal_count, unordered_map<string, int> &strings, vector<int> *sizes, vector<int> *labels){
    char c;
    string str;
    while((c=getc(fp)) != ';'){
        if(c == EOF){
            cout << "Program did not found ';' in the newick format." << endl;
            exit(1);
        }
        else if(c == '('){
            size = size + create_table(fp, 0, num_tree, leaf_count, internal_count, strings, sizes, labels);
        }
        else if(c == ')'){
            sizes[num_tree].push_back(size);
            labels[num_tree].push_back(-1);
            internal_count++;
            return size + 1;
        }
        else if(c == ','){
            continue;
        }
        else{
            get_string(fp, c, 0, num_tree, leaf_count, strings, sizes, labels);
            size++;
        }
    }
    return -1;
}

/* returns the most left leaf that is below i.*/
int left_leaf(int i, int w){
    return i - w;
}

/* Creates a table that contains all clusters by saving the first and last leaf it contains.*/
void create_clusters(int num_nodes1, int num_leafs, vector<int> *sizes, vector<int> *labels, vector<int> *clusters){
    int i, right, left, loc;
    int leaf_code = -1;
    right = -1;
    for(i = 0; i < num_leafs; i++){
        clusters[0][i] = -1;
        clusters[1][i] = -1;
    }
    i = 0;
    while(i < num_nodes1){
        if(sizes[0][i] == 0){
            leaf_code++;
            right = leaf_code;
            i++;
        }
        else{
            left = labels[0][left_leaf(i, sizes[0][i])];
            i++;
            if(i == num_nodes1){
                loc = right;
            }
            else{
                if(sizes[0][i] == 0){
                    loc = right;
                }
                else{
                    loc = left;
                }
            }
            clusters[0][loc] = left;
            clusters[1][loc] = right;
        }
    }
}

/* Searches in the cluster table if a given cluster is present in the given tree.*/
bool verify_cluster(int l,int r, vector<int> *clusters){
    if((l == clusters[0][l] && r == clusters[1][l]) || (l == clusters[0][r] && r == clusters[1][r])){
        return true;
    }
    return false;
}

/* Computes the Robinson Foulds distance by travering the second tree
 and whenever finds a cluster verifies if it is present in the first tree.*/
void compute_distance(int num_nodes2, int num_leafs, vector<int> *sizes, vector<int> *labels, vector<int> *clusters, int &rf_dist){
    stack<Struct> stack;
    Struct first, second;
    int i;
    int enc, weight;
    for(i = 0; i < num_nodes2; i++){
        if(sizes[1][i] == 0){
            enc = labels[1][i];
            first.left = enc;
            first.right = enc;
            first.n = 1;
            first.weight = 1;
            stack.push(first);
        }
        else{
            first.left = num_leafs + 1;
            first.right = 0;
            first.n = 0;
            first.weight = 1;
            weight = sizes[1][i];
            while(weight != 0){
                second.left = stack.top().left;
                second.right = stack.top().right;
                second.n = stack.top().n;
                second.weight = stack.top().weight;
                stack.pop();
                first.left = min(second.left, first.left);
                first.right = max(second.right, first.right);
                first.n = second.n + first.n;
                first.weight = second.weight + first.weight;
                weight = weight - second.weight;
            }
            stack.push(first);
            if(first.n == first.right - first.left + 1){
                if(verify_cluster(first.left, first.right, clusters)){
                    rf_dist++;
                }
            }
        }
    }
}

int main(int argc, char* argv[]){
    int num_nodes1 = 0;
    int num_nodes2 = 0;
    int leaf_count1 = 0;
    int leaf_count2 = 0;
    int internal_count1 =  0;
    int internal_count2 =  0;

    int rf_dist = 0;

    unordered_map<string, int> strings;

    vector<int> sizes[2];

    vector<int> clusters[2];

    vector<int> labels[2];

    auto start = std::chrono::system_clock::now();

    FILE* fp;
    fp = fopen(argv[1], "r");

    create_table(fp, 0, 0, leaf_count1, internal_count1, strings, sizes, labels);


    num_nodes1 = sizes[0].size();

    fclose(fp);

    fp = fopen(argv[2], "r");


    create_table(fp, 0, 1, leaf_count2, internal_count2, strings, sizes, labels);

    strings.clear();
    num_nodes2 = sizes[1].size();


    fclose(fp);

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    // cout << elapsed_seconds.count() << ", ";
    // cout << "parse time: " << elapsed_seconds.count() << "s" << endl;
    auto start2 = std::chrono::system_clock::now();


    clusters[0].resize(leaf_count1);
    clusters[1].resize(leaf_count1);

    create_clusters(num_nodes1, leaf_count1, sizes, labels, clusters);

    compute_distance(num_nodes2, leaf_count1, sizes, labels, clusters, rf_dist);

    cout << "Robinson Foulds distance is: " << float(num_nodes1 - (leaf_count1 + rf_dist) + num_nodes2 - (leaf_count1 + rf_dist)) / 2 << endl;

    auto end2 = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds2 = end2-start2;
    // cout << "algorithm time: " << elapsed_seconds2.count() << "s" << endl;
    // cout << elapsed_seconds2.count() << ", ";

    return 0;
}
