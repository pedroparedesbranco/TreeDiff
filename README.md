# TreeDiff

# What is it?

TreeDiff is a repository for comparing and visualizing changes in tree structures. It provides a suite of tools and functions to help users compare two different tree structures and identify the differences between them.

# Installation

```
git submodule update --init
cd sdsl-lite
./install.sh ../sdsl
cd ..
git clone https://github.com/pedroparedesbranco/TreeDiff.git
```

# Generating trees

```
python3 tree_generator.py number_of_trees number_of_leafs [--fully_labelled] [--weighted]
```

# Commands

```
./rf_postorder treeA.txt treeB.txt [info]
./rf_nextsibling treeA.txt treeB.txt [info]
./rf_day treeA.txt treeB.txt
./trip_treediff treeA.txt treeB.txt
./trip_sht treeA.txt treeB.txt
```
