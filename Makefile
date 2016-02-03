export ROOT := $(shell pwd)

export CXX := g++
export CXXFLAGS += -O2 -fPIC --pie \
	-std=c++11 -fexceptions -frtti \
	-I$(ROOT)/include -I$(ROOT)/libjson/include \
	-L$(ROOT)/libjson -ljson -lpthread

export MAKE := make --no-print-directory
export BIN := player

export SRC := CloudMusicApi.cc Lyric.cc MusicEntry.cc \
	Http.cc LyricDownloader.cc  MusicScanner.cc \
	Socket.cc Keyboard.cc main.cc Text.cc LocalCache.cc \
	MainUI.cc Screen.cc




export TARGET := 

ifeq ($(TARGET), android)
	SRC += OpenSLESMusicPlayer.cc
	CXXFLAGS += -DANDROID -lOpenSLES
else
	SRC += SDL2MusicPlayer.cc
	CXXFLAGS += -lSDL2 -lSDL2_mixer -lsmpeg2
endif



export OBJ := $(SRC:.cc=.o)

all:
	@cd libjson && $(MAKE)
	@cd src && $(MAKE)


.PHONY: clean
clean: 
	@-cd libjson && $(MAKE) clean
	@-cd src && $(MAKE) clean
	@echo -e "  CLEAN\t" $(BIN)
	@$(RM) $(BIN)
