default-target: build

PROTO_PATH = ProtoMsg
OUT_PATH = ProtoMsg/gen

CXX = g++

CXXFLAGS = --std=c++11 -g -Wall -Werror -Wextra

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(IXXFLAGS) -MD -MP $< -o $@


%.op: %.pb.cc
	$(CXX) -c $(CXXFLAGS) $(IXXFLAGS) -MD -MP $< -o $@

CXXFLAGS += -O0
IXXFLAGS += -I./include -I./ProtoMsg/gen
LDXXFLAGS +=  -lboost_system -lprotobuf 

NAME = ClientApplication

CLIENTAPPSRC = $(wildcard test/ClientApp/*.cpp)
CLIENTAPPOBJ = $(patsubst %.cpp, %.o, $(CLIENTAPPSRC))

DUMMYSERVERSRC = $(wildcard test/DummyServer/*.cpp)
DUMMYSERVEROBJ = $(patsubst %.cpp, %.o, $(DUMMYSERVERSRC))

ALLSRC = $(CLIENTAPPSRC) $(DUMMYSERVERSRC)
ALLOBJ = $(CLIENTAPPOBJ) $(DUMMYSERVEROBJ)

DEPS = $(patsubst %.cpp, %.d, $(ALLSRC))

proto: 
	mkdir -p $(OUT_PATH)
	protoc --proto_path=$(PROTO_PATH) --cpp_out=$(OUT_PATH) ProtoMsg/ProtoMsgRequest.proto 
	protoc --proto_path=$(PROTO_PATH) --cpp_out=$(OUT_PATH) ProtoMsg/ProtoMsgResponse.proto
	$(CXX) -c $(CXXFLAGS) $(IXXFLAGS) -MD -MP ProtoMsg/gen/ProtoMsgRequest.pb.cc -o ProtoMsg/gen/ProtoMsgRequest.o
	$(CXX) -c $(CXXFLAGS) $(IXXFLAGS) -MD -MP ProtoMsg/gen/ProtoMsgResponse.pb.cc -o ProtoMsg/gen/ProtoMsgResponse.o

build: ClientApplication DummyServer

all: proto build

ClientApplication: $(CLIENTAPPOBJ)
	$(CXX) $(CLIENTAPPOBJ) ProtoMsg/gen/ProtoMsgRequest.o ProtoMsg/gen/ProtoMsgResponse.o $(LDXXFLAGS) -o $@

DummyServer: $(DUMMYSERVEROBJ)
	$(CXX) $(DUMMYSERVEROBJ) ProtoMsg/gen/ProtoMsgRequest.o ProtoMsg/gen/ProtoMsgResponse.o $(LDXXFLAGS) -o $@

clean::
	rm -f $(DEPS)
	rm -f $(ALLOBJ)
	rm -f ClientApplication
	rm -f DummyServer 

clean_all: clean
	rm -rf ProtoMsg/gen

-include $(DEPS)

.PHONY: default-target build all clean
