CPP=g++
TGT=mantis

all: $(TGT)

$(TGT): 
	$(CPP) -O2 -m64 src/$(TGT).cpp -lpthread -lhdf5 -lsig_px1500 -o $@