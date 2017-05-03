#-------------------
# Variables
#------------------- 
CC          := g++ 
CFLAGS      := -std=c++11 -Wall -MP -MD -fPIC
DEBUGFLAGS  := -g -DDEBUGMODE=1
LDFLAGS	    := -shared 
LIB_win     := -L lib/SFML-2.4.2-win/lib -lsfml-audio -lsfml-network -lsfml-graphics -lsfml-window -lsfml-system -lstdc++fs -lktmw32
LIB_lin     := -L lib/SFML-2.4.2-lin/lib -lsfml-audio -lsfml-network -lsfml-graphics -lsfml-window -lsfml-system -lstdc++fs
SRCEXT      := cpp

SRCDIR      := src
INC         := -I include
TARGET_R    := bin/libquestia-eng
TARGET_D    := bin/libquestia_eng-d
BUILDDIR_R  := build
BUILDDIR_D  := build_debug

MAJOR 		:= 1
MINOR		:= 0
PATCH		:= 0

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

release: $(OBJECTS) $(TARGET)$(EXT)

$(TARGET)$(EXT): $(OBJECTS)
ifeq ($(OS),Windows_NT)
	cmd /c if not exist $(subst /,\\, $(dir $@)) md $(subst /,\\, $(dir $@))
else
	mkdir -p $(dir $@)
endif
	echo " Linking..."
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
	@echo " Cleaning Release...";
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
