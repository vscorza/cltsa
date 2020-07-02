#!/bin/bash
pushd ../../../src
make clean
popd
mkdir r_sh
cp ../*.sh r_sh
pushd r_sh
mkdir -p r_output
cp ../../r_output/generate_table.r r_output/generate_table.r
popd
tar -cvzf r_sh.tar.gz r_sh
tar --exclude='../../../src/results/*' -cvzf src.tar.gz ../../../src
docker build -t cltsa:latest -f Dockerfile .
docker stop cltsa || true && docker rm cltsa || true
docker run -it --publish 8000:8080 --detach --name cltsa cltsa:latest
rm -r src.tar.gz r_sh r_sh.tar.gz
