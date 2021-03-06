#!/bin/bash
sed -n '/clang/p' icl_showcmds_build.log | tee a  # 筛选出clang编译的项
awk -F '\-o ' '{print $2}' a > b  # 筛选-o之后的内容
awk '{print $1}' b > c  #筛选出out目录下的输出文件路径
sed -i 's/"//g' c  # 删除所有"号
sed -i '/^\s*$/d' c  #删除所有空行
cat c | while read line; do echo ./checksec --output xml -ff $line; done
rm a b c

# 将文本行倒序排列
sed '1!G;h;$!d' ./sed_test.txt
echo "      123" | sed 's/ //g'
# 将多行文件转换为一行
sed ':a;N;s/\n/ /;ta;'
