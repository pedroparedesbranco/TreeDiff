from ete3 import Tree
import random
import dendropy
import sys
import os

"""
Creates random fully labelled weighted phylogenetic trees with taxa only on the leaves
number_trees: number of trees that are genarated
number_leafs: number of leaves that the trees contain
"""
def create_fully_labelled_weighted_trees(number_trees, number_leafs):
    for i in range(number_trees):
        label = number_leafs
        name_list = list()
        name_list = [str(x) for x in range(number_leafs)]
        t1 = Tree()
        t1.populate(number_leafs, names_library=name_list)
        for node in t1.traverse():
            node.dist = random.uniform(0, 10)
            if not node.is_leaf():
                node.name = str(label - 1)
                label += 1
        tree_list = dendropy.TreeList()
        data=t1.write(format = 3)
        data = data[:-1]
        print(data + str(label - 1) + ";")

"""
Creates random fully labelled phylogenetic trees
number_trees: number of trees that are genarated
number_leafs: number of leaves that the trees contain
"""
def create_fully_labelled_trees(number_trees, number_leafs):
    for i in range(number_trees):
        label = number_leafs
        name_list = list()
        name_list = [str(x) for x in range(number_leafs)]
        t1 = Tree()
        t1.populate(number_leafs, names_library=name_list)
        for node in t1.traverse():
            if not node.is_leaf():
                node.name = str(label - 1)
                label += 1
        tree_list = dendropy.TreeList()
        data=str(t1.write(format = 8))
        data = data[:-1]
        print(data + str(label - 1) + ";")

"""
Creates random phylogenetic trees with taxa only on the leaves
number_trees: number of trees that are genarated
number_leafs: number of leaves that the trees contain
"""
def create_trees(number_trees, number_leafs):
    for i in range(number_trees):
        label = number_leafs
        name_list = list()
        name_list = [str(x) for x in range(number_leafs)]
        t1 = Tree()
        t1.populate(number_leafs, names_library=name_list)
        tree_list = dendropy.TreeList()
        data=t1.write(format = 9)
        print(data)

"""
Creates random weighted phylogenetic trees with taxa only on the leaves
number_trees: number of trees that are genarated
number_leafs: number of leaves that the trees contain
"""
def create_weighted_trees(number_trees, number_leafs):
    for i in range(number_trees):
        label = number_leafs
        name_list = list()
        name_list = [str(x) for x in range(number_leafs)]
        t1 = Tree()
        t1.populate(number_leafs, names_library=name_list)
        for node in t1.traverse():
            node.dist = random.uniform(0, 10)
            if not node.is_leaf():
                node.name = "_"
        tree_list = dendropy.TreeList()
        data=t1.write(format = 3)
        data = data[:-1]
        print(data + "_;")


if __name__ == "__main__":
    args = sys.argv[1:]
    number_trees = int(args[0])
    number_leafs = int(args[1])
    if("--fully_labelled" in args):
        fully_labelled = True
    else:
        fully_labelled = False
    if("--weighted" in args):
        weighted = True
    else:
        weighted = False
    if(fully_labelled and weighted):
        create_fully_labelled_weighted_trees(number_trees, number_leafs)
    elif(fully_labelled):
        create_fully_labelled_trees(number_trees, number_leafs)
    elif(weighted):
        create_weighted_trees(number_trees, number_leafs)
    else:
        create_trees(number_trees, number_leafs)

