export ROOT := $(shell pwd)

export CXX := g++
export CXXFLAGS += -O2 -fPIC --pie \
	-std=c++11 -fexceptions -frtti -g \
	-I$(ROOT)/include -I$(ROOT)/libjson/include \
	-L$(ROOT)/libjson -ljson -lpthread

export MAKE := make --no-print-directory -j$(shell cat /proc/cpuinfo | grep processor | wc -l)
export BIN := player

export SRC := CloudMusicApi.cc Lyric.cc MusicEntry.cc \
	Http.cc LyricDownloader.cc  MusicScanner.cc \
	Socket.cc Keyboard.cc main.cc Text.cc LocalCache.cc \
	MainUI.cc Screen.cc IniFile.cc Config.cc


ifeq ($(wildcard /system/build.prop), )
	export TARGET := linux
else
	export TARGET := android
endif


ifeq ($(TARGET), android)
	SRC += OpenSLESMusicPlayer.cc
	CXXFLAGS += -DANDROID -lOpenSLES
else
	SRC += SDL2MusicPlayer.cc
	CXXFLAGS += -lm -ldl -lz -lavcodec -lavformat -lavutil -lSDL2 -lSDL2_mixer -lsmpeg2
endif



export OBJ := $(SRC:.cc=.o)

all:
	@echo "Building for $(TARGET)"
	@cd libjson && $(MAKE)
	@cd src && $(MAKE)


.PHONY: clean
clean: 
	@-cd libjson && $(MAKE) clean
	@-cd src && $(MAKE) clean
	@echo -e "  CLEAN\t" $(BIN)
	@$(RM) $(BIN)
