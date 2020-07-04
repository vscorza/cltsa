#!/bin/bash
echo "[[Generating LaTeX tables]]"
if [ "$1" == "docker" ]; then
	Rscript r_output/generate_table_docker.r
else
	Rscript r_output/generate_table.r
fi
