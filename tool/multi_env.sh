echo "multi-master env has installed"

export PROTOBUF="$ENV_PATH/protobuf-install"

export GRPC="$ENV_PATH/grpc-install"

export PATH="$PROTOBUF/bin:$GRPC/bin:$PATH"

export LD_LIBRARY_PATH="$PROTOBUF/lib:$GRPC/lib:$LD_LIBRARY_PATH"

export PKG_CONFIG_PATH="$GRPC/lib/pkgconfig:$PROTOBUF/lib/pkgconfig:$PKG_CONFIG_PATH" 

export C_INCLUDE_PATH="$GRPC/include/:$PROTOBUF/include:$C_INCLUDE_PATH"

export CPLUS_INCLUDE_PATH="$GRPC/include/:$PROTOBUF/include:$CPLUS_INCLUDE_PATH"

export LIBRARY_PATH="$PROTOBUF/lib:$GRPC/lib:$LIBRARY_PATH"


#===========GCC Change
export GCC_DIR="$ENV_PATH/gcc-all/gcc-4.8.5-install"
PATH="$GCC_DIR/bin:$PATH"
export PATH
LD_LIBRARY_PATH="$GCC_DIR/lib64:$LD_LIBRARY_PATH"
export LD_LIBRARY_PATH
MANPATH="$GCC_DIR/share/man:$MANPATH"
export MANPATH

#=============JAVA
export JAVA_HOME=$ENV_PATH/jdk1.8.0
export JRE_HOME=${JAVA_HOME}/jre
export CLASSPATH=.:${JAVA_HOME}/lib:${JRE_HOME}/lib
export PATH=${JAVA_HOME}/bin:$PATH 

#=============Maven
export M2_HOME=$ENV_PATH/apache-maven
export PATH=${M2_HOME}/bin:${PATH}