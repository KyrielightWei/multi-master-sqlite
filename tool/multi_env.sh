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
export GCC_DIR="$ENV_PATH/gcc-4.8.5-install"
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

#============GCC PRE
#unset LIBRARY_PATH CPATH C_INCLUDE_PATH PKG_CONFIG_PATH CPLUS_INCLUDE_PATH INCLUDE 取消编译器标志，编译gcc出错时可能有用
export GCC_PRE=$ENV_PATH/gcc-pre
gmp=gmp-6.1.2
mpfr=mpfr-4.0.2
mpc=mpc-1.1.0
isl=isl-0.18
export LD_LIBRARY_PATH="$GCC_PRE/$gmp/lib:$GCC_PRE/$mpfr/lib:$GCC_PRE/$mpc/lib:$GCC_PRE/$isl/lib:$LD_LIBRARY_PATH"
export C_INCLUDE_PATH="$GCC_PRE/$gmp/include/:$GCC_PRE/$mpfr/include:$GCC_PRE/$mpc/include:$GCC_PRE/$isl/include:$C_INCLUDE_PATH"
export CPLUS_INCLUDE_PATH="$GCC_PRE/$gmp/include/:$GCC_PRE/$mpfr/include:$GCC_PRE/$mpc/include:$GCC_PRE/$isl/include:$CPLUS_INCLUDE_PATH"
export LIBRARY_PATH="$GCC_PRE/$gmp/lib:$GCC_PRE/$mpfr/lib:$GCC_PRE/$mpc/lib:$GCC_PRE/$isl/lib:$LD_LIBRARY_PATH"


#=============Make
export PATH="$ENV_PATH/make-install/bin:$PATH"

#=============Ant
export ANT_HOME=$ENV_PATH/apache-ant-1.9.14

export PATH=$PATH:$ANT_HOME/bin