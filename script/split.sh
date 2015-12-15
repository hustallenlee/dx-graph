#!/bin/bash

filepath=$1
echo $filepath

filename=${filepath##*/}
echo $filename

number=$2
echo $number

edge_size=$3
echo $edge_size

file_size=`du -b $filepath | awk '{print $1}'`
echo $file_size

edge_num=$[$file_size / $edge_size]
echo $edge_num

each_edges_per_piece=$[$edge_num / $number]
echo $each_edges_per_piece


for (( i=0; i<$number; i++ ))
do

if [ $i -ne $[$number - 1] ]
then
	echo "generate piece "$i
	dd if=$filepath bs=$edge_size count=$each_edges_per_piece skip=$[$i * $each_edges_per_piece] of=${i}_$filename
else
	echo "generate piece"$i
	dd if=$filepath bs=$edge_size skip=$[$i * $each_edges_per_piece] of=${i}_$filename
fi
done

