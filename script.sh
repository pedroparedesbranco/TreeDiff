#!/bin/bash

dir="trees/100000/" # the directory to search for files in

iterations=5

echo
echo
echo $dir
echo
echo "day:"
for dir1 in "$dir"*/; do
  # echo $dir1;
  # for file in "$dir1"; do
  files=("$dir1"/*)
  file1=${files[0]}
  file2=${files[1]}
  for ((i=1; i<=$iterations; i++)) do
    ./day "$file1" "$file2"
  done
done
echo
echo
echo
echo "treediff_post:"
for dir1 in "$dir"*/; do
  # echo $dir1;
  # for file in "$dir1"; do
  files=("$dir1"/*)
  file1=${files[0]}
  file2=${files[1]}
  for ((i=1; i<=$iterations; i++)) do
    ./treediff_postorder "$file1" "$file2"
  done
  # ./treediffsada "$file1" "$file2"
  # if [[ "$output1" != "[1]" ]]; then
  #   echo "errou" "$dir1"
  #   break
  # fi

  # tree1=$(cat $file1)
  # tree2=$(cat $file2)
  # Rscript.exe -e "library('TreeDist'); \
  # tree1 <- ape::read.tree(text = '$tree1'); \
  # tree2 <- ape::read.tree(text = '$tree2'); \
  # start_time <- Sys.time(); \
  # print(RobinsonFoulds(tree1, tree2)); \
  # end_time <- Sys.time(); \
  # print(end_time - start_time)"
done
echo
echo





# for file1 in "$dir"/*; do
#   for file2 in "$dir"/*; do
#     echo "$file1" "$file2"
#     if [[ "$file1" != "$file2" ]]; then
#         start=`date +%s.%N`
#         ./treediff "$file1" "$file2"
#         end=`date +%s.%N`
#         runtime=$( echo "$end - $start" | bc -l )
#         # echo $output1
#         # echo $runtime
#         tree1=$(cat $file1)
#         tree2=$(cat $file2)
#         start=`date +%s.%N`
#         Rscript.exe -e "library('TreeDist'); \
#         tree1 <- ape::read.tree(text = '$tree1'); \
#         tree2 <- ape::read.tree(text = '$tree2'); \
#         start_time <- Sys.time(); \
#         print(RobinsonFoulds(tree1, tree2)); \
#         end_time <- Sys.time(); \
#         print(end_time - start_time)"
#         end=`date +%s.%N`
#         runtime=$( echo "$end - $start" | bc -l )
#         # echo $output2
#         # echo $runtime
#         # if [[ "$output1" != "$output2" ]]; then
#         #     echo "Output doesn't match expected value:"
#         #     echo "  File 1: $file1"
#         #     echo "  File 2: $file2"
#         # else
#         #     echo "$file1 and $file2 It was successfull."
#         # fi
#     fi
#   done
# done


# output2="$(Rscript.exe -e "library('TreeDist'); \
# tree1 <- ape::read.tree(text = '$tree1'); \
# tree2 <- ape::read.tree(text = '$tree2'); \
# start_time <- Sys.time() \
# print(RobinsonFoulds(tree1, tree2));" 2>&1)"


# start=`date +%s.%N`
# stuff
# end=`date +%s.%N`

# runtime=$( echo "$end - $start" | bc -l )

# file1="trees/tree_1.txt"
# file2="trees/tree_2.txt"

# tree1=$(cat $file1)
# tree2=$(cat $file2)

# echo $tree1

# echo $tree2

# Rscript.exe -e "library('TreeDist'); \
# tree1 <- ape::read.tree(text = '$tree1'); \
# tree2 <- ape::read.tree(text = '$tree2'); \
# print(RobinsonFoulds(tree1, tree2));"


# Rscript.exe -e "library('TreeDist'); \
# tree1 <- ape::read.tree(text = '(A, ((B, (C, (D, E))), ((F, G), (H, I))));'); \
# tree2 <- ape::read.tree(text = '(A, ((B, (C, (D, (H, I)))), ((F, G), E)));'); \
# print(RobinsonFoulds(tree1, tree2));"

# Rscript.exe -e "library('TreeDist'); \
# tree1 <- ape::read.tree(text = '(A, ((B, (C, (D, E))), ((F, G), (H, I))));'); \
# tree2 <- ape::read.tree(text = '(A, ((B, (C, (D, (H, I)))), ((F, G), E)));');"

# Rscript.exe -e "library('TreeDist'); \
# print(RobinsonFoulds(tree1, tree2));"