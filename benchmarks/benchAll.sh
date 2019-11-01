#!/bin/bash
for filename in *.js; do
	printf "%s: " "${filename%.*}"
    node "$filename"
done
