CC=g++
CFLAGS=-Wall -gstabs+ -Wextra
CSRC=$(wildcard ./src/*.cpp) src/MonarchHeader.pb.cpp
PTHLIB=pthread
LIBDIRS=$(addprefix -L,$(PXLIBDIR) $(PBDIR)/lib)
INCDIRS=$(addprefix -I,$(shell pwd)/include $(shell pwd)/src)
LDFLAGS=$(addprefix -l,$(PXLIB) protobuf)
BUILDDIR=build
OBJ=$(sort $(CSRC:%.cpp=%.o))
TGT=monarch_test
PBHDR=src/MonarchHeader.pb.h

all: $(PBHDR) $(TGT)

$(TGT): $(OBJ)
	@echo LD $@
	@$(CC) -o $@ $^ $(LDFLAGS)

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
	$(PBDIR)/bin/protoc -Isrc --cpp_out=src src/MonarchHeader.proto
	mv src/MonarchHeader.pb.cc src/MonarchHeader.pb.cpp

$(BUILDIR): 
	@[ -d $@ ] || mkdir -p $@

.PHONY: check-syntax
check-syntax:
	$(CC) $(CFLAGS) -Wall -Wextra -pedantic -fsyntax-only $(CSRC)
