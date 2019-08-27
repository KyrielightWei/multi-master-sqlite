echo "multi-master env has installed"

export PROTOBUF="$ENV_PATH/protobuf-install"

export GRPC="$ENV_PATH/grpc-install"

export PATH="$PROTOBUF/bin:$GRPC/bin:$PATH"

export LD_LIBRARY_PATH="$PROTOBUF/lib:$GRPC/lib:$LD_LIBRARY_PATH"

export PKG_CONFIG_PATH="$GRPC/lib/pkgconfig:$PROTOBUF/lib/pkgconfig:$PKG_CONFIG_PATH" 

export C_INCLUDE_PATH="$GRPC/include/:$PROTOBUF/include:$C_INCLUDE_PATH"

export CPLUS_INCLUDE_PATH="$GRPC/include/:$PROTOBUF/include:$CPLUS_INCLUDE_PATH"

export LIBRARY_PATH="$PROTOBUF/lib:$GRPC/lib:$LIBRARY_PATH"