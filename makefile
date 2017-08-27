#-------------------
# Variables
#------------------- 
CC           := g++ 
CFLAGS       := -std=c++14 -Wall -MP -MD -fPIC
DEBUGFLAGS   := -g -DDEBUGMODE=1
LDFLAGS	     := -shared
LIB_win      := -L lib/SFML-2.4.2-win/lib -lsfml-audio -lsfml-network -lsfml-graphics -lsfml-window -lsfml-system -L lib/Cryptopp-5.6.5-win -lcryptopp -L lib/AngelScript-2.31.2-win -langelscript -lstdc++fs -lktmw32
LIB_lin      := -lsfml-audio -lsfml-network -lsfml-graphics -lsfml-window -lsfml-system -lstdc++fs
LIB_TEST_win := -L lib/UnitTest++-2.0-win -lUnitTest++
LIB_TEST_lin := 
SRCEXT       := cpp

SRCDIR       := src
INC          := -I include
TARGET_R     := bin/libquestia-eng
TARGET_D     := bin/libquestia-eng-d
BUILDDIR_R   := build
BUILDDIR_D   := build_debug

MAJOR 		 := 1
MINOR		 := 0
PATCH		 := 0

#-------------------
# Mode
#-------------------

ifneq "$(findstring ebug, $(MAKECMDGOALS))" ""
	TARGET   := $(TARGET_D)
	BUILDDIR := $(BUILDDIR_D)
	CFLAGS   += $(DEBUGFLAGS)
else
	TARGET   := $(TARGET_R)
	BUILDDIR := $(BUILDDIR_R)
endif

#-------------------
# Dependant Variables
#-------------------

ifeq ($(OS),Windows_NT)
	SOURCES := $(wildcard src/*.$(SRCEXT)) $(wildcard src/**/*.$(SRCEXT)) $(wildcard src/**/**/*.$(SRCEXT))\
	$(wildcard src/**/**/**/*.$(SRCEXT)) $(wildcard src/**/**/**/**/*.$(SRCEXT))\
	$(wildcard src/**/**/**/**/**/*.$(SRCEXT)) $(wildcard src/**/**/**/**/**/**/*.$(SRCEXT))
	
	EXT := .dll
	LIB := $(LIB_win)
else
	SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))

	EXT = .so
	LIB := $(LIB_lin)
endif

OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
DEPS := $(OBJECTS:.o=.d))

#------------------- 
# Build
#-------------------

all: default
default: release
Release: release
Debug: release
debug: release

test: release
ifeq ($(OS),Windows_NT)
	$(CC) tests/Main.cpp -o tests/test.exe  -std=c++14 $(INC) $(LIB) $(LIB_TEST_win) -L bin -lquestia-eng.$(MAJOR).$(MINOR).$(PATCH)
	-cmd /c del /f tests\libquestia-eng.$(MAJOR).$(MINOR).$(PATCH).dll
	-cmd /c copy bin\libquestia-eng.$(MAJOR).$(MINOR).$(PATCH).dll tests\libquestia-eng.$(MAJOR).$(MINOR).$(PATCH).dll
	-cmd /c $(subst /,\\, $(wildcard tests/*.exe))
else
	@echo TODO implement
endif

release: $(OBJECTS) $(TARGET)$(EXT)

$(TARGET)$(EXT): $(OBJECTS)
ifeq ($(OS),Windows_NT)
	cmd /c if not exist $(subst /,\\, $(dir $@)) md $(subst /,\\, $(dir $@))
else
	mkdir -p $(dir $@)
endif
	@echo Linking...
	$(CC) $^ -o $(TARGET).$(MAJOR).$(MINOR).$(PATCH)$(EXT) $(LIB) $(LDFLAGS)
	
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
ifeq ($(OS),Windows_NT)
	cmd /c if not exist $(subst /,\\, $(dir $@)) md $(subst /,\\, $(dir $@))
else
	mkdir -p $(dir $@)
endif
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean: cleanRelease cleanDebug

cleanRelease:
	@echo Cleaning Release...
ifeq ($(OS),Windows_NT)
	-cmd /c rmdir /s /q $(BUILDDIR_R)
	-cmd /c del /f $(subst /,\\,$(TARGET_R)).$(MAJOR).$(MINOR).$(PATCH)$(EXT)
else
	$(RM) -r $(BUILDDIR_R) $(TARGET_R)
endif

cleanDebug:
	@echo " Cleaning Debug...";
ifeq ($(OS),Windows_NT)
	-cmd /c rmdir /s /q $(BUILDDIR_D)
	-cmd /c del /f $(subst /,\\,$(TARGET_D)).$(MAJOR).$(MINOR).$(PATCH)$(EXT)
else
	$(RM) -r $(BUILDDIR_D) $(TARGET_D)
endif

.PHONY: clean
#------------ 
-include $(DEPS)
