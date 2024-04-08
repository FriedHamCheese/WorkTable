SRCDIR := ./src
SRCFILES := $(wildcard $(SRCDIR)/*.cpp)

OBJDIR := ./bin
OBJFILES := $(SRCFILES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

HEADERDIR := ./include
HEADERFILES := $(wildcard $(HEADERDIR)/*.hpp)

BINDIR := ./bin
EXECFILE := $(BINDIR)/build.exe

TEST_DIR := ./tests
TEST_EXECFILE := $(TEST_DIR)/test.exe
TEST_HEADERFILES := $(wildcard $(TEST_DIR)/*.hpp)
TEST_OBJFILES := $(filter-out $(OBJDIR)/main.o,$(OBJFILES))

include user_fltk_flags
CXX := $(CXX)
CXXFLAGS := -I./include $(FLTK_CXXFLAGS) --std=c++20 -Wall -pedantic -g3
LDFLAGS := $(FLTK_LDFLAGS)

.PHONY: all
all: build build_test

.PHONY: build
build: $(EXECFILE)
	$(EXECFILE)

.PHONY: path_debug
path_debug:
	@echo
	@echo SRCDIR: $(SRCDIR)
	@echo SRCFILES: 
	@echo $(SRCFILES)
	
	@echo
	@echo OBJDIR: $(OBJDIR)
	@echo OBJFILES: 
	@echo $(OBJFILES)
	
	@echo
	@echo HEADERDIR: $(HEADERDIR)
	@echo HEADERFILES: 
	@echo $(HEADERFILES)	
	
	@echo
	@echo BINDIR: $(BINDIR)
	@echo EXECFILE: $(EXECFILE)

	@echo
	@echo TEST_DIR: $(TEST_DIR)
	@echo TEST_EXECFILE: $(TEST_EXECFILE)
	@echo TEST_HEADERFILES: $(TEST_HEADERFILES)
	@echo TEST_OBJFILES: $(TEST_OBJFILES)

.PHONY: build_test
build_test: $(TEST_EXECFILE)
	$(TEST_EXECFILE)

$(EXECFILE): $(OBJFILES)
	$(CXX) $(OBJFILES) $(LDFLAGS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERFILES) Makefile | $(OBJDIR)
	$(CXX) -c $< $(CXXFLAGS) -o $@
	

$(OBJDIR):
	mkdir $@


$(TEST_EXECFILE): $(TEST_DIR)/test_main.o $(TEST_OBJFILES)
	$(CXX) $^ $(LDFLAGS) -o $@

$(TEST_DIR)/test_main.o: $(TEST_DIR)/test_main.cpp $(TEST_HEADERFILES) Makefile | $(TEST_DIR)
	$(CXX) -c $< $(CXXFLAGS) -o $@

$(TEST_DIR):
	mkdir $@

.PHONY: clean
clean:
	rm -rf $(BINDIR)