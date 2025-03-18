.PHONY: all clean help

CXX=g++
CC=gcc
CXXFLAGS=-std=c++20

BUILDDIR=build
SOURCEDIR=src
INCLUDEDIR=include
EXEC=gym_tracker
SOURCES:=$(wildcard $(SOURCEDIR)/*.cc)
OBJ:=$(patsubst $(SOURCEDIR)/%.cc,$(BUILDDIR)/%.o,$(SOURCES))

all: dir $(BUILDDIR)/$(EXEC)

dir:
	if not exist $(BUILDDIR) mkdir $(BUILDDIR)

$(BUILDDIR)/$(EXEC): $(OBJ) build/sqlite3.o
		$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJ): $(BUILDDIR)/%.o : $(SOURCEDIR)/%.cc
		$(CXX) $(CXXFLAGS) -I$(INCLUDEDIR) -Isqlite3 -c $< -o $@

build/sqlite3.o:
		$(CC) -c sqlite3/sqlite3.c -o $@

clean:
		rmdir /s /q $(BUILDDIR)

help:
		@echo "Usage: make {all|clean|help}" 1>&2 && false