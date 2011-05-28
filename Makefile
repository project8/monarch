CC=g++
CFLAGS=-Wall -Werror -O2
CSRC=$(wildcard ./src/*.cpp)
PXINCDIR=/usr/local/include
PXLIBDIR=/usr/local/lib
H5INCDIR=/usr/local/include
H5LIBDIR=/usr/local/lib
H5LIB=hdf5 hdf5_cpp
PXLIB=sig_px1500
PTHLIB=pthread
LIBDIRS=$(addprefix -L,$(H5LIBDIR) $(PXLIBDIR))
INCDIRS=$(addprefix -I,$(shell pwd)/include)
LDFLAGS=$(addprefix -L,$(H5LIBDIR)) $(addprefix -l,$(H5LIB) $(PXLIB))
BUILDDIR=build
OBJ=$(CSRC:%.cpp=%.o)
TGT=Mantis

$(TGT): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

%.o: %.cpp $(BUILDIR)
	$(CC) $(CFLAGS) $(INCDIRS) -c $< -o $@

clean: 
	@find . -maxdepth 2 -name "*.o" | xargs -I{} rm {}
	@find . -maxdepth 2 -name "*~" | xargs -I{} rm {}
	@find . -maxdepth 1 -name $(TGT) | xargs -I{} rm {}
	@echo cleaned.

$(BUILDIR): 
	@[ -d $@ ] || mkdir -p $@
	
MantisBlock.o: MantisBlock.cc MantisBlock.hh
	$(CC) $(CFLAGS) $(INCDIRS) -c $< -o $@
    
