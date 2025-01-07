#!/bin/bash
for filename in ./json-test/*.json; do
	echo "$filename"
	./cmake-build-debug/validator "$filename"
done

