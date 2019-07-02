# ! /bin/bash

#sudo yum install gflags-devel gtest-devel
#sudo yum install libcxx-devel

git clone -b $(curl -L https://grpc.io/release) https://github.com/grpc/grpc
cd grpc
git submodule update --init

cd ./third_party/protobuf/
 ./configure --prefix=$HOME/software/protobuf-install
make clean 
make
make install

cd ../../

#grpc
sed "s|^prefix ?=.*|prefix ?= $HOME/software/grpc-install|" -i ./Makefile
make clean
make 
make install


echo 'export PROTOBUF="$HOME/software/protobuf-install"' >> ~/.bashrc

echo 'export GRPC="$HOME/software/grpc-install"' >> ~/.bashrc

echo 'export PATH="$PROTOBUF/bin:$GRPC/bin:$PATH"' >> ~/.bashrc

echo 'export LD_LIBRARY_PATH="$PROTOBUF/lib:$GRPC/lib:$LD_LIBRARY_PATH"' >> ~/.bashrc

echo 'export PKG_CONFIG_PATH="$GRPC/lib/pkgconfig:$PROTOBUF/lib/pkgconfig:$PKG_CONFIG_PATH"' >> ~/.bashrc

echo 'export C_INCLUDE_PATH="$GRPC/include/:$C_INCLUDE_PATH"' >> ~/.bashrc

echo 'export CPLUS_INCLUDE_PATH="$GRPC/include/:$CPLUS_INCLUDE_PATH"' >> ~/.bashrc

echo 'export LIBRARY_PATH="$PROTOBUF/lib:$GRPC/lib:$LIBRARY_PATH"' >> ~/.bashrc


exec "$SHELL"



