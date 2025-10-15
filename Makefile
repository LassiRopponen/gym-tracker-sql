.PHONY: all clean help

CXX=g++
CC=gcc
CXXFLAGS=-std=c++20

BUILDDIR=build
SOURCEDIR=src
INCLUDEDIR=include
EXEC=gym-tracker
SOURCES:=$(wildcard $(SOURCEDIR)/*.cc)
OBJ:=$(patsubst $(SOURCEDIR)/%.cc,$(BUILDDIR)/%.o,$(SOURCES))

all: $(BUILDDIR)/$(EXEC)

$(BUILDDIR)/$(EXEC): $(OBJ) build/sqlite3.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJ): $(BUILDDIR)/%.o : $(SOURCEDIR)/%.cc
	$(CXX) $(CXXFLAGS) -I$(INCLUDEDIR) -Isqlite3 -c $< -o $@

build/sqlite3.o:
	$(CC) -c sqlite3/sqlite3.c -o $@

clean:
ifeq ($(OS),Windows_NT)
	del /q /f "$(BUILDDIR)\*"
	for /d %%d in ("$(BUILDDIR)\*") do rmdir /q /s "%%d"
	cd . > build/.gitkeep
else
	rm -rf $(BUILDDIR)/*
endif
