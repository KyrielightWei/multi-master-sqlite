CXXFLAGS += -std=c++11
LDFLAGS += -L/usr/local/lib `pkg-config --libs protobuf grpc++ grpc` -Wl,--as-needed -lpthread -ldl

all : os_remote.grpc.pb.cc  os_remote.pb.cc
.PHONY : all

PROTOC = protoc
GRPC_CPP_PLUGIN_PATH ?= `which grpc_cpp_plugin`

PROTOS_PATH = ./
vpath %.proto $(PROTOS_PATH)

.PRECIOUS: %.grpc.pb.cc
%.grpc.pb.cc: %.proto
	$(PROTOC) -I $(PROTOS_PATH) --grpc_out=$(PROTOS_PATH) --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

.PRECIOUS: %.pb.cc
%.pb.cc: %.proto
	$(PROTOC) -I $(PROTOS_PATH) --cpp_out=$(PROTOS_PATH) $<

clean:
	rm -f client *.o  ../common/*.pb.cc ../common/*.pb.h ../server/server ../server/*.o ../common/*.o
