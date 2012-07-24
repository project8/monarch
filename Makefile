CC=g++
CFLAGS=-Wall -gstabs+
CSRC=$(wildcard ./src/*.cpp)
PTHLIB=pthread
LIBDIRS=$(addprefix -L,$(PXLIBDIR))
INCDIRS=$(addprefix -I,$(shell pwd)/include)
LDFLAGS=$(addprefix -l,$(PXLIB))
BUILDDIR=build
OBJ=$(CSRC:%.cpp=%.o)
TGT=monarch_test

$(TGT): $(OBJ)
	@echo LD $@
	@$(CC) -o $@ $(OBJ) $(LDFLAGS)

%.o: %.cpp $(BUILDIR)
	@echo CXX $(basename $(notdir $@))
	@$(CC) $(CFLAGS) $(INCDIRS) -c $< -o $@

clean: 
	@find . -maxdepth 2 -name "*.o" | xargs -I{} rm {}
	@find . -maxdepth 2 -name "*~" | xargs -I{} rm {}
	@find . -maxdepth 1 -name $(TGT) | xargs -I{} rm {}
	@echo cleaned.

$(BUILDIR): 
	@[ -d $@ ] || mkdir -p $@
