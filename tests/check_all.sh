#!/bin/sh

make clean

alias grep='grep --color=always'

grep '//'          *.*
grep -i 'todo'     *.*
grep 'alloc'       *.*
grep 'free'        *.*
grep 'str.\?cpy'   *.*
grep 'memcpy'      *.*
grep 's.\?printf'  *.*
grep 'str.\?dup'   *.*
