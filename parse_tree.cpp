#include <array>
#include <string>
#include <queue>
#include <memory>
#include <cmath>
#include <stack>

#include <sdsl/k2_tree.hpp>

using namespace std;
using namespace sdsl;

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

void create_bit_vector(char* tree, char* array){
    //int stack[200];
    stack<int> stack;
    int count = 0;
    for(int i = 0; tree[i] != '\0'; i++){
        if(tree[i] == '('){
            stack.push(count);
            count++;
        }
        else if(tree[i] >= 'A' && tree[i] <= 'Z'){
            array[count] = tree[i];
            count++;
        }
        else if(tree[i] == ')'){
            printf("%d\n", stack.top());
            array[stack.top()] = tree[i+1];
            stack.pop();
            i++;
        }
        else{
            continue;
        }
    }
}

int rnk(bit_vector vec, int v, int state){
    int rank = 0;
    for(int i = 0; i < v; i++){
        if(vec[i] == state){
            rank++;
        }
    }
    return rank;
}


int select(bit_vector vec, int v, int state){
    int select = 0;
    for(int i = 0; i < n; i++){ //size of bit vector
        if(vec[i] == state){
            select++;
        }
        if(select == v){
            return i+1;
        }
    }
    return 0;
}


// min{j > i, excess(B, j) = excess(B, i) + d}∪{n+1},

int fwdsearch(bit_vector vec, int i, int d){

}

// max{j < i, excess(B, j) = excess(B, i) + d}∪{0}.

int bwdsearch(bit_vector vec, int i, int d){

}


// rmq(B, i, j) = leftmost position of a minimum in excess(B, i ... j),
int rmq(bit_vector vec, int i, int j){

}

// rMq(B, i, j) = leftmost position of a maximum in excess(B, i ... j),
int rMq(bit_vector vec, int i, int j){

}

// mincount(B, i, j) = times the minimum occurs in excess(B, i ... j),
int mincount(bit_vector vec, int i, int j){

}

// minselect(B, i, j,t) = position of the tth minimum in excess(B, i ... j).
int minselect(bit_vector vec, int i, int j, int t){

}


//excess(B, i) = rank1(B, i) − rank0(B, i) = 2 · rank1(B, i) − i

// close(B, i) = fwdsearch(B, i, −1),
// open(B, i) = bwdsearch(B, i, 0) + 1,
// enclose(B, i) = bwdsearch(B, i, −2) + 1.


int excess(bit_vector vec, int i){
    return 2 * rnk(vec, i, 1) - i;
}

int excess(bit_vector vec, int i, int v){
    return excess(vec, i) - excess(vec, v)
}

int open(bit_vector vec, int v){
    // returns the corresponding open parenthesis.
    return bwdsearch(vec, v, 0) + 1;
}

int close(bit_vector vec, int v){
    // returns the corresponding close parenthesis.
    return fwdsearch(vec, v, -1);
}

int enclose(bit_vector vec, int v){
    return bwdsearch(vec, v, -2) + 1;
}


//basic operations


int first_child(bit_vector vec, int v){
    if(vec[v+1] == 1){
        return v+1;
    }
    else{
        return 0;
    }
}

int last_child(bit_vector vec, int v){
    if(vec[v+1] == 1){
        return open(vec, close(vec, v) - 1);
    }
    else{
        return 0;
    }
}

int next_sibling(bit_vector vec, int v){
    if(vec[close(vec, v) + 1] == 1){
        return close(vec, v) + 1;
    }
    else{
        return 0;
    }
}

int prev_sibling(bit_vector vec, int v){
    if(v > 1 && vec[v-1] == 0){
        return open(vec, v - 1);
    }
    else{
        return 0;
    }
}

int parent(bit_vector vec, int v){
    if(v != 1){
        return enclose(vec, v);
    }
    else{
        return 0;
    }
}

int isleaf(bit_vector vec, int v){
    return vec[v+1] == 0;
}

int nodemap(bit_vector vec, int v){
    return rnk(vec, v, 1);
}

int nodeselect(bit_vector vec, int i){
    return select(vec, i, 1);
}

int postorder(bit_vector vec, int v){
    return rnk(vec, close(vec, v), 0);
}

int postorderselect(bit_vector vec, int i){
    return open(vec, select(vec, i, 0));
}

int depth(bit_vector vec, int v){
    return excess(vec, v);
}

int subtree(bit_vector vec, int v){
    return int((close(vec, v) - v + 1)/2);
}

int is_ancestor(bit_vector vec, int u, int v){
    return u <= v <= close(vec, u);
}

int level_ancestor(bit_vector vec, int v, int d){
    return bwdsearch(vec, v, -d - 1) + 1;
}


// complex operations






int main(int argc, char* argv[]){
    char newick_1[200];
    char newick_2[200];
    
    char array1[200];
    char array2[200];

    bit_vector v_1, v_2;

    uint v_1_size, v_2_size;

    FILE * fp1, * fp2;

    // if(!strcmp(argv[1], "help")){
    //     printf("Usage:\n\n\t./treediff help\n\t./treediff distance\n");
    //     exit(0);
    // }

    // if(argc != 6){
    //     printf("Wrong Format\n\nUsage : ./treediff <task> <metric> <tree1_file> <tree2_file> <consider internal nodes?>.\n\nFor more information run: ./treediff help.\n");
    //     exit(0);
    // }

    fp1 = fopen ("tree1.txt", "r");
    fp2 = fopen ("tree2.txt", "r");

    fscanf(fp1, "%[^\n]s", newick_1);
    fscanf(fp2, "%[^\n]s", newick_2);

    // remove_spaces(newick_1);
    // remove_spaces(newick_2);

    //printf("%s", newick_1);

    fclose(fp1);
    fclose(fp2);

    create_bit_vector(newick_1, array1);
    create_bit_vector(newick_2, array2);

    for(int i = 0; i < 9; i++){
        printf("%c  ",array1[i]);
    }

    // strcpy(newick_1, "(((B,C)F,D)G,(A,E)I)H"); 
    // strcpy(newick_2, "((((A,B)H,C)G,D)F,E)I");


    int distance_rf, distance_triplets;


    // print_tree(tree_1);


    // for(int i = 0; i < NUM_LEAFS; i++){
    //     printf("%c\n", leafs[i]->name);
    // }

    //tree_2 = transform_tree(newick_2);

    //distance_triplets = triplets(tree_1, tree_2, 0);

    //printf("Distance = %d.\n", distance_triplets);
    
    return 0;
}
