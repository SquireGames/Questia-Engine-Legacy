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
	-cmd /c copy lib\AngelScript-2.31.2-win\libangelscript.a tests\libangelscript.a
	-cmd /c copy lib\Cryptopp-5.6.5-win\libcryptopp.a tests\libcryptopp.a
	-cmd /c copy lib\SFML-2.4.2-win\bin\openal32.dll tests\openal32.dll
	-cmd /c copy lib\SFML-2.4.2-win\bin\sfml-audio-2.dll tests\sfml-audio-2.dll
	-cmd /c copy lib\SFML-2.4.2-win\bin\sfml-graphics-2.dll tests\sfml-graphics-2.dll
	-cmd /c copy lib\SFML-2.4.2-win\bin\sfml-network-2.dll tests\sfml-network-2.dll
	-cmd /c copy lib\SFML-2.4.2-win\bin\sfml-system-2.dll tests\sfml-system-2.dll
	-cmd /c copy lib\SFML-2.4.2-win\bin\sfml-window-2.dll tests\sfml-window-2.dll
	-cmd /c copy lib\Windows\libgcc_s_dw2-1.dll tests\libgcc_s_dw2-1.dll
	-cmd /c copy "lib\Windows\libstdc++-6.dll" "tests\libstdc++-6.dll"
	-cmd /c copy lib\Windows\libwinpthread-1.dll tests\libwinpthread-1.dll
	-cmd /c tests\test.exe
else
	@echo TODO implement
endif

cleanTest:
	@echo Cleaning Test...
ifeq ($(OS),Windows_NT)
	-cmd /c del /f tests\test.d
	-cmd /c del /f tests\test.exe
	-cmd /c del /f tests\libquestia-eng.$(MAJOR).$(MINOR).$(PATCH).dll
	-cmd /c del /f tests\libangelscript.a
	-cmd /c del /f tests\libcryptopp.a
	-cmd /c del /f tests\openal32.dll
	-cmd /c del /f tests\sfml-audio-2.dll
	-cmd /c del /f tests\sfml-graphics-2.dll
	-cmd /c del /f tests\sfml-network-2.dll
	-cmd /c del /f tests\sfml-system-2.dll
	-cmd /c del /f tests\sfml-window-2.dll
	-cmd /c del /f tests\libgcc_s_dw2-1.dll
	-cmd /c del /f tests\"libstdc++-6.dll"
	-cmd /c del /f tests\libwinpthread-1.dll
else
	$(RM) -r $(BUILDDIR_R) $(TARGET_R)
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

clean: cleanRelease cleanDebug cleanTest

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
