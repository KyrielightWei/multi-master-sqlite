CXXFLAGS += -std=c++11
LDFLAGS += -L/usr/local/lib `pkg-config --libs protobuf grpc++ grpc` -Wl,--as-needed -lpthread -ldl


client : os_remote.pb.o os_remote.grpc.pb.o os_remote_client.o os_client_wrap.o
	gcc $^ $(LDFLAGS) -lstdc++ -o $@

os_remote_client.o : os_remote_client.c
	gcc -c $^




PROTOC = protoc
GRPC_CPP_PLUGIN_PATH ?= `which grpc_cpp_plugin`

vpath %.proto ./

.PRECIOUS: %.grpc.pb.cc
%.grpc.pb.cc: %.proto
	$(PROTOC) -I ./ --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

.PRECIOUS: %.pb.cc
%.pb.cc: %.proto
	$(PROTOC) -I ./ --cpp_out=. $<

clean:
	rm -f *.o client *.pb.cc *.pb.h
