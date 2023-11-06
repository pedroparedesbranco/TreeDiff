# TreeDiff

TreeDiff is a repository for comparing and visualizing changes between phylogenetic trees. It provides a suite of tools and functions to help users analyse the Robinson Foulds and Triplets metrics between two phylogenetic trees.

## Why use it?

The TreeDiff repository uses the Succinct Data Structure Library ([SDSL](https://github.com/simongog/sdsl-lite/tree/master)) to represent the trees in a balanced parenthesis representation. TreeDiff also extends the metrics to compare fully labelled trees. For the Robinson Foulds metric, it also allows to compute the weighted Robinson Foulds metric when it recieves weighted trees as input and there is an option to visualize where the trees differe from each other.

## Requirements

To use the TreeDiff repository there are the following requirements:

* `g++` compiler, version 9.4.0 or higher, to use clang++ change in Makefile `CC = clang++` and `CFLAGS = -O3 -Wall -std=c++11`
* ETE3 python library
* All the requirements present in the [SDSL](https://github.com/simongog/sdsl-lite/tree/master) repository

## Installation

To download and install TreeDiff use the following commands:

```
git clone https://github.com/pedroparedesbranco/TreeDiff.git
cd TreeDiff
git submodule update --init
cd sdsl-lite
./install.sh ../sdsl
cd ..
make
```

This installs [SDSL](https://github.com/simongog/sdsl-lite/tree/master) and creates five executables:

* `rf_postorder`, it computes the Robinson Foulds metric using mostly the `post_order_select` function present in `bp_support_sada_extended.hpp` to traverse the trees.
* `rf_nextsibling`, it computes the Robinson Foulds metric using mostly the `next_sibling` and `first_child` functions present in `bp_support_sada_extended.hpp` to traverse the trees.
* `rf_day`, it computes the Robinson Foulds metric using the Day's algorithm approach.
* `trip_treediff`, it computes the Triplets metric with low memory usage.
* `trip_sht`, it computes the Triplets metric efficiently.

These executables can be found in the main directory.

## Commands

To use this TreeDiff, the following commands can be used:

Arguments:

* `tree1.txt`, the first tree to be compared in a newick format.
* `tree2.txt`, the second tree to be compared in a newick format.
* `info`, optional argument that only works for the first two implementations, it indicates which clusters are exclusive for both trees by returning their pre-order indexes.

```
./rf_postorder tree1.txt tree2.txt [info]
./rf_nextsibling tree1.txt tree2.txt [info]
./rf_day tree1.txt tree2.txt
./trip_treediff tree1.txt tree2.txt
./trip_sht tree1.txt tree2.txt
```

## Examples

In the [tree_examples](https://github.com/pedroparedesbranco/TreeDiff/tree/main/tree_examples) folder it can be seen two examples of trees to give as an input.

TreeA.txt contains the newick format for the following tree:

![alt text](https://github.com/pedroparedesbranco/TreeDiff/blob/main/tree1.png)

TreeB.txt contains the newick format for the following tree:

![alt text](https://github.com/pedroparedesbranco/TreeDiff/blob/main/tree2.png)

By running the command

```
./rf_postorder tree_examples/treeA.txt tree_examples/treeB.txt info
```

The output will be:

```
Robinson Foulds distance is: 1
Indexes of clusters that are differents in tree_examples/treeA.txt:
3
Indexes of clusters that are differents in tree_examples/treeB.txt:
7
```

The indexes that are shown in the differences represent the pre-order indexes that contain the exclusive clusters.
This means that in treeA the cluster (A, B) is exclusive and in treeB the cluster (A, C) is exclusive.

## Generating trees

In this repository it is also possible to genarate random phylogenetic trees in a newick format using the following command:

Arguments:

* `number_of_trees`, number of trees to be generated.
* `number_of_leafs`, number of leafs that the trees will contain.
* `--fully_labelled`, optional argument to create fully labelled trees.
* `--weighted`, optional argument to create weighted trees.

```
python3 tree_generator.py number_of_trees number_of_leafs [--fully_labelled] [--weighted]
```

