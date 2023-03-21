EXEC_NAME := cn

C_FLAGS := -static-libgcc -static-libstdc++ -std=c++11 -pedantic-errors -Wall -Wextra -Werror -Wno-unused-parameter

SRCDIR := src
INCDIR := inc
OBJDIR := obj
BINDIR := bin

WIN_PDCURSES_DIR := C:\C\PDCurses-3.8

INCS := -I$(INCDIR) $(if $(findstring Windows_NT,$(OS)),-I$(WIN_PDCURSES_DIR))
LIBS := $(if $(findstring Windows_NT,$(OS)),-L$(WIN_PDCURSES_DIR)\wincon -lpdcurses,-lncurses)

SOURCES := $(wildcard $(SRCDIR)/*.cpp)
OBJECTS := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

all: $(BINDIR)/$(EXEC_NAME)

.PHONY: clean

$(BINDIR)/$(EXEC_NAME): $(OBJECTS)
	g++ $(C_FLAGS) $^ -o $@ $(LIBS)

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	g++ -c $(C_FLAGS) $< -o $@ $(INCS)

clean:
ifeq ($(OS),Windows_NT)
	del $(subst /,\,$(OBJECTS))
	del $(BINDIR)\$(EXEC_NAME).exe
else
	rm $(OBJECTS)
	rm $(BINDIR)/$(EXEC_NAME)
endif