# TreeDiff

TreeDiff is a repository for comparing and visualizing changes between phylogenetic trees. It provides a suite of tools and functions to help users analyse the Robinson Foulds and Triplets metrics between two phylogenetic trees.

## Why use it?

The TreeDiff repository uses the Succinct Data Structure Library ([SDSL](https://github.com/simongog/sdsl-lite/tree/master)) to represent the trees in a balanced parenthesis representation. TreeDiff also extends the metrics to compare fully labelled trees. For the Robinson Foulds metric, it also allows to compute the weighted Robinson Foulds metric when it recieves weighted trees as input and there is an option to visualize where the trees differe from each other.

## Requirements

To use the TreeDiff repository there are the following requirements:

* recent `g++` compiler
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

## Commands

To use this TreeDiff, the following commands can be used where ´tree1´ is the first tree to be compared and ´tree2´ the second the second tree. The ´info´ argument is optional and it can be used to visualize the clusters that are exclusive to both trees.

```
./rf_postorder treeA.txt treeB.txt [info]
./rf_nextsibling treeA.txt treeB.txt [info]
./rf_day treeA.txt treeB.txt
./trip_treediff treeA.txt treeB.txt
./trip_sht treeA.txt treeB.txt
```

## Examples

In the [trees](https://github.com/pedroparedesbranco/TreeDiff/tree/main/trees) folder it can be seen an example of trees to give as an input.

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

