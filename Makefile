default-target: build

PROTO_PATH = ClientProtobuf
OUT_PATH = ClientProtobuf/gen

CXX = g++

CXXFLAGS = --std=c++11 -g -Wall -Werror -Wextra

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(IXXFLAGS) -MD -MP $< -o $@


%.op: %.pb.cc
	$(CXX) -c $(CXXFLAGS) $(IXXFLAGS) -MD -MP $< -o $@

CXXFLAGS += -O0
IXXFLAGS += -I./Client -I./ClientProtobuf/gen
LDXXFLAGS += -lboost_system -lprotobuf 

NAME = ClientApplication

CLIENTAPPSRC = $(wildcard ClientApp/*.cpp)
CLIENTAPPOBJ = $(patsubst %.cpp, %.o, $(CLIENTAPPSRC))

ALLSRC = $(CLIENTAPPSRC)
ALLOBJ = $(CLIENTAPPOBJ)

DEPS = $(patsubst %.cpp, %.d, $(CLIENTAPPSRC))

proto: 
	mkdir -p $(OUT_PATH)
	protoc --proto_path=$(PROTO_PATH) --cpp_out=$(OUT_PATH) ClientProtobuf/ClientAppRequest.proto 
	protoc --proto_path=$(PROTO_PATH) --cpp_out=$(OUT_PATH) ClientProtobuf/ClientAppResponse.proto
	$(CXX) -c $(CXXFLAGS) $(IXXFLAGS) -MD -MP ClientProtobuf/gen/ClientAppRequest.pb.cc -o ClientProtobuf/gen/ClientAppRequest.o
	$(CXX) -c $(CXXFLAGS) $(IXXFLAGS) -MD -MP ClientProtobuf/gen/ClientAppResponse.pb.cc -o ClientProtobuf/gen/ClientAppResponse.o

build: ClientApplication 

all: proto build

ClientApplication: $(ALLOBJ)
	$(CXX) $(LDXXFLAGS) $(ALLOBJ) ClientProtobuf/gen/ClientAppRequest.o ClientProtobuf/gen/ClientAppResponse.o -o $@

clean::
	rm -f $(DEPS)
	rm -f $(ALLOBJ)
	rm -f ClientApplication
	rm -rf ClientProtobuf/gen	

-include $(DEPS)

.PHONY: default-target build all clean
