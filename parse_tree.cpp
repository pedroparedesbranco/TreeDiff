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

using namespace std;
using namespace sdsl;

list<int> clusters_1 = {};
list<int> clusters_2 = {};

bool internal_nodes = false;

int rf_dist = 0;
int triplets_dist = 0;

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

void get_string(FILE* fp, string* array, unordered_map<string, int> &hash_table, char c, int count){
    string str = "";
    while(!(c == ')' || c == '(' || c == ',' || c == EOF)){
        str.append(1, c);
        c = getc(fp);
    }
    hash_table[str] = count + 1;
    array[count] = str;
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

void add_bit(bit_vector& bv, int i){
    bv.resize(bv.size() + 1);
    bv[bv.size() - 1] = i;
}

bit_vector create_bit_vector_2(char* tree, string* array, unordered_map<string, int> &hash_table){
    bit_vector bv = bit_vector(0);
    stack<int> stack;
    string str = "";
    int count = 0;
    int state;
    char c;
    FILE* fp;
    fp = fopen(tree, "r");
    while((c=getc(fp)) != EOF){
        if(c == '('){
            stack.push(count);
            count++;
            add_bit(bv, 1);
        }
        else if(c == ','){
            continue;
        }
        else if(c == ')'){
            c = getc(fp);
            if(!(c == ')' || c == '(' || c == ',' || c == EOF)){
                get_string(fp, array, hash_table, c, stack.top());
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
            get_string(fp, array, hash_table, c, count);
            count++;
        }
    }
    fclose(fp);
    cout << bv << endl;
    cout << bv.size() << endl;
    return bv;
}

// bit_vector create_bit_vector(char* tree, string* array, unordered_map<string, int> &hash_table){
//     bit_vector bv = bit_vector(0);
//     stack<int> stack;
//     int count = 0;
//     // int pos = 0;
//     for(int i = 0; tree[i] != '\0'; i++){
//         // cout << "i = " << i << endl;
//         if(tree[i] == '('){
//             stack.push(count);
//             count++;
//             add_bit(bv, 1);
//             // bv[pos] = 1;
//             // pos++;
//         }
//         else if(tree[i] == ','){
//             continue;
//         }
//         else if(tree[i] == ')'){
//             if(!(tree[i+1] == ')' || tree[i+1] == '(' || tree[i+1] == ',' || tree[i+1] == '\0')){
//                 i = get_string_2(tree, array, hash_table, i + 1, stack.top());
//                 internal_nodes = true;
//             }
//             // hash_table[tree[i+1]] = stack.top() + 1;
//             // array[stack.top()] = tree[i+1];
//             stack.pop();
//             add_bit(bv, 0);
//             // bv[pos] = 0;
//             // pos++;
//         }
//         else{
//             add_bit(bv, 1);
//             add_bit(bv, 0);
//             i = get_string_2(tree, array, hash_table, i, count);
//             // hash_table[tree[i]] = count + 1;
//             // array[count] = tree[i];
//             count++;
//             // bv[pos] = 1;
//             // bv[pos+1] = 0;
//             // pos = pos+2;
//         }
//     }
//     cout << bv << endl;
//     cout << bv.size() << endl;
//     return bv;
// }



// int robinson_foulds(bp_support_ggg<> vec_1, bp_support_ggg<> vec_2, bit_vector v_1, bit_vector v_2, char* array1, unordered_map<char, int> &hash_table){
//     int distance = 0;
//     int prev = 1;
//     int current = 0;
//     int lcas[18];
//     int lchild;
//     int lca1;

//     cout << vec_1.first_child(0) << endl;
//     cout << vec_1.first_child(1) << endl;
//     cout << vec_1.first_child(2) << endl;
//     cout << isleaf(vec_1, v_1, 3) << endl;
//     cout << lca(vec_2, 4, 6) << endl;
//     // cout << next_sibling(vec_1, v_1, 3) << endl;
//     // cout << next_sibling(vec_1, v_1, 5) << endl;
//     exit(1);

//     cout << endl;

//     cout << vec_2.enclose(vec_2.enclose(7)) << endl;

//     cout << endl;


//     cout << endl;

//     cout << lca(vec_2, 5, 7) << endl;

//     exit(1);
//     while(!isleaf(vec_1, v_1, current)){
//         current = first_child(vec_1, v_1, current);
//     }
//     while(prev){
//         prev = parent(vec_1, current);
//         lcas[current] = hash_table[array1[current]];
//         while(next_sibling(vec_1, v_1, current)){
//             current = next_sibling(vec_1, v_1, current);
//             lcas[current] = hash_table[array1[current]];
//         }
//     }

//     current = prev;
//     prev = parent(vec_1, current);
//     lchild = last_child(vec_1, v_1, current);
//     // while(next_sibling(vec_1, v_1, current)){

//     // }
//     return distance;
// }

// without internal nodes
int rf_2(bp_support_ggg<> vec_1, bp_support_ggg<> vec_2, string* array1, unordered_map<string, int> &hash_table, int current){
    int lcas, lcas_aux, x;
    
    if(vec_1.isleaf(current)){
        // cout << vec_1.nodemap(current) << endl;
        lcas = vec_2.nodeselect(hash_table[array1[vec_1.nodemap(current) - 1]]);
    }
    else{
        // cout << vec_1.nodemap(current) << endl;
        lcas = rf_2(vec_1, vec_2, array1, hash_table, vec_1.first_child(current));
        cout << "Comparission between: " << vec_1.nodemap(current) << " and " << vec_2.nodemap(lcas) << endl;
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
            cout << "Comparission between: " << vec_1.nodemap(current) << " and " << vec_2.nodemap(lcas_aux) << endl;
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
int rf(bp_support_ggg<> vec_1, bp_support_ggg<> vec_2, string* array1, unordered_map<string, int> &hash_table, int current){
    int lcas, lcas_aux, x;

    if(vec_1.isleaf(current)){
        // cout << vec_1.nodemap(current) << endl;
        lcas = vec_2.nodeselect(hash_table[array1[vec_1.nodemap(current) - 1]]);
    }
    else{
        // cout << vec_1.nodemap(current) << endl;
        lcas = vec_2.lca(vec_2.nodeselect(hash_table[array1[vec_1.nodemap(current) - 1]]), rf(vec_1, vec_2, array1, hash_table, vec_1.first_child(current)));
        cout << "Comparission between: " << vec_1.nodemap(current) << " and " << vec_2.nodemap(lcas) << endl;
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
            cout << "Comparission between: " << vec_1.nodemap(current) << " and " << vec_2.nodemap(lcas_aux) << endl;
            if(vec_1.cluster_size(current) != vec_2.cluster_size(lcas_aux)){
                clusters_1.push_back(vec_1.nodemap(current));
                clusters_2.push_back(vec_2.nodemap(lcas_aux));
                rf_dist++;
            }
        }
    }
    return lcas;
}


// int f(bp_support_ggg<> vec_1, bp_support_ggg<> vec_2, bit_vector v_1, bit_vector v_2, char* array1, unordered_map<char, int> &hash_table, int count, int lcas){
//     int current = vec_1.post_order_select(count);
//     int sib;

//     cout << cluster_size(vec_1, nodeselect(vec_1, 1)) << endl;
//     cout << cluster_size(vec_1, nodeselect(vec_1, 2)) << endl;
//     cout << cluster_size(vec_1, nodeselect(vec_1, 3)) << endl;
//     cout << cluster_size(vec_1, nodeselect(vec_1, 4)) << endl;
//     cout << cluster_size(vec_1, nodeselect(vec_1, 5)) << endl;
//     cout << cluster_size(vec_1, nodeselect(vec_1, 6)) << endl;
//     cout << cluster_size(vec_1, nodeselect(vec_1, 7)) << endl;
//     cout << cluster_size(vec_1, nodeselect(vec_1, 8)) << endl;
//     cout << cluster_size(vec_1, nodeselect(vec_1, 9)) << endl;

//     exit(0);

//     if(lcas == -1){
//         lcas = hash_table[array1[current]];
//     }
//     else{
//         //compare()
//         printf(" ");
//     }
//     while(!is_last_sib(vec_1, v_1, current)){
//         current = vec_1.post_order_select(++count);
//         // if(bv[current+1] == 1){
//         //     lcas = lca()
//         // }
//         lcas = lca(vec_2, lcas, current);
//     }

//     f(vec_1, vec_2, v_1, v_2, array1, hash_table, count+1, lcas);

//     if(!isleaf(vec_1, v_1, count)){
//         lcas = f(vec_1, vec_2, v_1, v_2, array1, hash_table, first_child(vec_1, v_1, current), -1);
//     }
//     // if(lca == -1){
//     //     lcas = hash_table[array1[current]];
//     // }
//     while((sib = next_sibling(vec_1, v_1, current))){
//         current = sib;
//         // lcas = lca(vec_2, v_2, lcas, current);
//     }
//     cout << nodemap(vec_1, current) << endl;
//     return lcas;

// }

// int triplets(bp_support_ggg<> vec_1, bp_support_ggg<> vec_2, bit_vector v_1, bit_vector v_2){
//     int triplets = 0;
//     for(int i = 0; i < 18; i++){
//         for(int j = i + 1; j < 18; j++){
//             for(int k = j+1; k < 18; k++){
//                 cout << i << j << k << endl;
//                 cout << nodeselect(vec_1, i+1) << nodeselect(vec_1, j+1) << nodeselect(vec_1, k+1) << endl;
//                 cout << nodeselect(vec_2, i+1) << nodeselect(vec_2, j+1) << nodeselect(vec_2, k+1) << endl;
//                 cout << lca(vec_1, nodeselect(vec_1, i+1), nodeselect(vec_1, j+1)) << endl;
//                 // depth(lca(a,b)) > depth(lca(b,c)) && depth(lca(a,b) > depth(lca(b,c)))
//                 if(depth(vec_1, lca(vec_1, nodeselect(vec_1, i+1), nodeselect(vec_1, j+1))) > 
//                     depth(vec_1, lca(vec_1, nodeselect(vec_1, j+1), nodeselect(vec_1, k+1))) 
//                     && depth(vec_2, lca(vec_2, nodeselect(vec_2, i+1), nodeselect(vec_2, j+1))) >
//                     depth(vec_2, lca(vec_2, nodeselect(vec_2, j+1), nodeselect(vec_2, k+1)))){
//                         triplets++;
//                     }
//             }
//         }
//     }
//     return triplets;
// }




int main(int argc, char* argv[]){
    char newick_1[200];
    char newick_2[200];
    
    string array1[200];
    string array2[200];

    bit_vector v_1;
    bit_vector v_2; 

    int distance;

    unordered_map<string, int> hash_table1;
    unordered_map<string, int> hash_table2;


    //bp_support_ggg vec_1, vec_2;

    uint v_1_size, v_2_size;

    // FILE * fp1, * fp2;

    // if(!strcmp(argv[1], "help")){
    //     printf("Usage:\n\n\t./treediff help\n\t./treediff distance\n");
    //     exit(0);
    // }

    // if(argc != 6){
    //     printf("Wrong Format\n\nUsage : ./treediff <task> <metric> <tree1_file> <tree2_file> <consider internal nodes?>.\n\nFor more information run: ./treediff help.\n");
    //     exit(0);
    // }


    // fp1 = fopen (argv[1], "r");
    // fp2 = fopen (argv[2], "r");

    // char c = getc(fp1);
    // cout << "c = " << c << endl;

    // fscanf(fp1, "%[^\n]s", newick_1);
    // fscanf(fp2, "%[^\n]s", newick_2);

    // remove_spaces(newick_1);
    // remove_spaces(newick_2);

    //printf("%s", newick_1);

    v_1 = create_bit_vector_2(argv[1], array1, hash_table1);
    v_2 = create_bit_vector_2(argv[2], array2, hash_table2);

    // fclose(fp1);
    // fclose(fp2);

    bp_support_ggg<> vec_1(&v_1);
    bp_support_ggg<> vec_2(&v_2);


    if(internal_nodes){
        rf(vec_1, vec_2, array1, hash_table2, 0);
        cout << "Robinson Foulds distance is: " << rf_dist << endl;
        cout << "clusters differents in " << argv[1] << ":" << endl;
        for (int x : clusters_1) {
            cout << x << endl;
        }
    }
    else{
        rf_2(vec_1, vec_2, array1, hash_table2, 0);
        cout << "Robinson Foulds distance is: " << rf_dist << endl;
        cout << "clusters differents in " << argv[1] << ":" << endl;
        for (int x : clusters_1) {
            cout << x << endl;
        }
    }
    return 0;
}
