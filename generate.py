from ete3 import Tree
import random
import dendropy
import pylabeledrf
from pylabeledrf.computeLRF import *
import os


def randomLabels(intree, p_speciation=0.7):
    """
    Function to assign random speciation and duplication nodes to a tree.
    Returns a new tree (leaves the input tree unchanged)

    :param intree: a tree as Dendropy object
    :param p_speciation: probability of a speciation node.
    """
    t = intree.clone(depth=1)
    for n in t.internal_nodes():
        n.label = None
    # for n in t.internal_nodes():
    #     if random.random() < p_speciation:
    #         n.label = 'speciation'
    #     else:
    #         n.label = 'duplication'
    t.seed_node.label = None
    return(t)


def create_random(i,name_list):
    t1 = Tree()
    t1.populate(i,names_library=name_list)
    t2 = Tree()
    t2.populate(i,names_library=name_list)
    tree_list1 = dendropy.TreeList()
    tree_list1.read(data=t1.write(format = 9),schema = 'newick')
    tree_list1.read(data=t2.write(format = 9),schema = 'newick')
    dtl1 = randomLabels(tree_list1[0])
    dtl2 = randomLabels(tree_list1[1])

    return dtl1, dtl2




# for i in range(200,10200,200):
#     t1 = Tree()
#     name_list = list()
#     d = 0
#     name_list = [str(x) for x in range(i)]
#     print(i)

# def generate_tree(n):
#     """Generate a random tree with n leaves"""
#     leaves = [str(i) for i in range(n)]
#     tree = Tree()
#     tree.populate(n, names_library=leaves)
#     return tree

def generate_newick(n, dim):
    """Generate n random trees and write each tree to a separate text file"""
    for i in range(n):
        name_list = list()
        name_list = [str(x) for x in range(dim)]
        dtl1, dtl2= create_random(dim, name_list)
        # Write the Newick string to a text file
        aux = list()
        aux.append(str(dtl1))
        aux.append(str(dtl2))
        aux[0] += ";"
        aux[1] += ";"
        for j in range(2):
            filename = f"trees/{dim}/test{i+1}/tree{j+1}_{dim}.in"    
            if not os.path.exists(f"trees/{dim}/test{i+1}"):
                os.makedirs(f"trees/{dim}/test{i+1}")
            with open(filename, "w") as f:
                # print(aux[j])
                f.write(aux[j])

# Example usage: generate 5 random trees and write each tree to a separate text file
generate_newick(5, 60000)
generate_newick(5, 70000)
generate_newick(5, 80000)
generate_newick(5, 90000)