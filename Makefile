CC=g++
CFLAGS=-Wall -Werror -O2 -gstabs+
CSRC=$(wildcard ./src/*.cpp)
PXINCDIR=/usr/local/include
PXLIBDIR=/usr/local/lib
PXLIB=sig_px1500
PTHLIB=pthread
LIBDIRS=$(addprefix -L,$(PXLIBDIR))
INCDIRS=$(addprefix -I,$(shell pwd)/include)
LDFLAGS=$(addprefix -l,$(PXLIB))
BUILDDIR=build
OBJ=$(CSRC:%.cpp=%.o)
TGT=Mantis

$(TGT): $(OBJ)
	@echo LD $@
	@$(CC) -o $@ $(OBJ) $(LDFLAGS)

%.o: %.cpp $(BUILDIR)
	@echo CXX $(basename $(notdir $@))
	@$(CC) $(CFLAGS) $(INCDIRS) -c $< -o $@

install:
	cp $(TGT) /usr/local/bin/Mantis

clean: 
	@find . -maxdepth 2 -name "*.o" | xargs -I{} rm {}
	@find . -maxdepth 2 -name "*~" | xargs -I{} rm {}
	@find . -maxdepth 1 -name $(TGT) | xargs -I{} rm {}
	@echo cleaned.

$(BUILDIR): 
	@[ -d $@ ] || mkdir -p $@

MantisBlock.o: MantisBlock.cc MantisBlock.hh
	$(CC) $(CFLAGS) $(INCDIRS) -c $< -o $@
