export ROOT := $(shell pwd)

export CXX := g++
export CXXFLAGS += -O2 -DANDROID -fPIC --pie \
	-std=c++11 -fexceptions -frtti \
	-I$(ROOT)/include -I$(ROOT)/libjson/include \
	-L$(ROOT)/libjson -ljson -lOpenSLES

export MAKE := make --no-print-directory

export SRC := $(shell cd src && ls *.cc)
export OBJ := $(SRC:.cc=.o)
export BIN := player



all:
	@cd libjson && $(MAKE)
	@cd src && $(MAKE)


.PHONY: clean
clean: 
	@-cd libjson && $(MAKE) clean
	@-cd src && $(MAKE) clean
	@echo -e "  CLEAN\t" $(BIN)
	@$(RM) $(BIN)
