#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_LEAFS 9


typedef struct node {
    double weight;
    char name;
    struct node *left, *right, *hook;
    int color;
    int index;
} *node_t;

node_t* leafs;

node_t create_node(int state, node_t current){
    node_t new = (node_t)malloc(sizeof(struct node));

    if(state == 0){
        current->left = new;
        //current->right = NULL;
    }
    else{
        current->right = new;
        //current->left = NULL;
    }

    new->hook = current;

    return new;
}

node_t create_leaf(char name, int state, node_t current, int index){
    node_t new = (node_t)malloc(sizeof(struct node));

    leafs[index] = new;

    leafs[index]->name = name;
    leafs[index]->index = index;

    leafs[index]->left = NULL;
    leafs[index]->right = NULL;

    if(state == 0){
        current->left = leafs[index];
    }
    else{
        current->right = leafs[index];
    }

    leafs[index]->hook = current;

    return current;
}

void update_internal_node(node_t node, char name, int index){
    leafs[index] = node;
    leafs[index]->name = name;
    leafs[index]->index = index;
}


node_t transform_tree(char* tree){
    int state;
    int index = 0;

    // int counter = 0;

    node_t root = (node_t)malloc(sizeof(struct node));

    node_t current;

    current = root;

    current->hook = NULL;

    current->left = NULL;

    current->right = NULL;

    state = 0; // left = 0 and right = 1

    for(int i = 1; tree[i] != '\0'; i++){
        printf("%d\n", i);
        if(tree[i] == '('){
            current = create_node(state, current);
            state = 0;
        }
        else if(tree[i] == ')'){
            // counter++;
            if(tree[i+1] >= 'A' && tree[i+1] <= 'Z'){
                update_internal_node(current, tree[i+1], index);
                i++;
                index++;
            }
            if(current->hook != NULL){
                current = current->hook;
            }
        }
        else if(tree[i] >= 'A' && tree[i] <= 'Z'){ //TODO: big names
            current = create_leaf(tree[i], state, current, index);
            index++;
        }
        else if(tree[i] == ','){
            state = 1;
            // counter = 0;
        }
        else{
            continue;
        }
    }

    return root;
}


void print2DUtil(node_t root, int space)
{
    // Base case
    if (root == NULL)
        return;
 
    // Increase distance between levels
    space += 10;
 
    // Process right child first
    print2DUtil(root->right, space);
 
    // Print current node after space
    // count
    printf("\n");
    for (int i = 10; i < space; i++)
        printf(" ");
    printf("%c\n", root->name);
 
    // Process left child
    print2DUtil(root->left, space);
}

void print_tree(node_t root){
    print2DUtil(root, 0);
}



void coloring(node_t tree, int color){
    int first, last;

    node_t node = tree;

    last = node->index;

    while (node->left != NULL){
        node = node->left;
    }
    first = node->index;

    for(int i = first; i<=last; i++){
        leafs[i]->color = color;
    }
}


int triplets(node_t tree_1, node_t tree_2, int distance){

    if(tree_2 == NULL){
        return distance;
    }
    node_t subtree_1 = tree_1;
    node_t subtree_2 = tree_2;

    coloring(tree_1->left, 0);
    coloring(tree_1->right, 1);




    return distance;
}


int main(){
    char newick_1[200];
    char newick_2[200];

    // strcpy(newick_1, "(((B, C), D),(A, E))"); 
    // strcpy(newick_2, "((((A, B), C), D), E)");

    strcpy(newick_1, "(((B,C)F,D)G,(A,E)I)H"); 
    strcpy(newick_2, "((((A,B)H,C)G,D)F,E)I");

    node_t tree_1, tree_2;

    int distance_rf, distance_triplets;

    leafs = (node_t*)malloc(sizeof(node_t)*NUM_LEAFS);

    tree_1 = transform_tree(newick_1);
    // print_tree(tree_1);


    // for(int i = 0; i < NUM_LEAFS; i++){
    //     printf("%c\n", leafs[i]->name);
    // }

    tree_2 = transform_tree(newick_2);

    distance_triplets = triplets(tree_1, tree_2, 0);

    printf("Distance = %d.\n", distance_triplets);
    
    return 0;
}