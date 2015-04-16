#!/bin/sh
files=$*
grep -h -v "^#" $files |  cut -f 1 -d "," | sort -u | wc -l