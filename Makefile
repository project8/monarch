CC=g++
CFLAGS=-Wall -gstabs+ -Wextra
CSRC=$(wildcard ./src/*.cpp) src/MonarchHeader.pb.cpp
PTHLIB=pthread
LIBDIRS=$(addprefix -L,$(PXLIBDIR) $(PBLIBDIR) $(shell pwd))
INCDIRS=$(addprefix -I,$(shell pwd)/include $(shell pwd)/src)
LDFLAGS=$(addprefix -l,$(PXLIB) protobuf)
BUILDDIR=build
OBJ=$(sort $(CSRC:%.cpp=%.o))
TGT=monarch_test
LIBNAME=monarch
LIB=$(addsuffix .so,$(addprefix lib,$(LIBNAME)))
PBHDR=src/MonarchHeader.pb.h

all: $(PBHDR) $(LIB) $(TGT) 

$(TGT): $(LIB)
	@echo LD $@
	$(CC) -o $@ $(LIBDIRS) $(LDFLAGS) -l$(LIBNAME)

$(LIB): $(OBJ)
	@echo SO $@
	@$(CC) -shared -o $@ $^ $(LDFLAGS)

%.pb.o: %.pb.cpp $(BUILDIR)
	@echo CXX $(basename $(notdir $@))
	@$(CC) $(CFLAGS) $(INCDIRS) -c $< -o $@

%.o: %.cpp $(BUILDIR)
	@echo CXX $(basename $(notdir $@))
	@$(CC) $(CFLAGS) $(INCDIRS) -c $< -o $@

clean: 
	@find . -maxdepth 2 -name "*.o" | xargs -I{} rm {}
	@find . -maxdepth 2 -name "*~" | xargs -I{} rm {}
	@find . -maxdepth 1 -name $(TGT) | xargs -I{} rm {}
	@echo cleaned.

$(PBHDR): src/MonarchHeader.proto
	$(PROTOC) -Isrc --cpp_out=src src/MonarchHeader.proto
	mv src/MonarchHeader.pb.cc src/MonarchHeader.pb.cpp

$(BUILDIR): 
	@[ -d $@ ] || mkdir -p $@

.PHONY: check-syntax
check-syntax:
	$(CC) $(CFLAGS) -Wall -Wextra -pedantic -fsyntax-only $(CSRC)
