pushd ../../../src
make clean
popd
tar --exclude='../../../src/results/*' -cvzf src.tar.gz ../../../src
sudo docker build -t cltsa:latest -f Dockerfile .
sudo docker run --publish 8000:8080 --detach --name cltsa cltsa:latest
rm -r src.tar.gz
