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
build: display_build_header $(EXECFILE)

.PHONY: display_build_header
display_build_header:
	@echo
	@echo [Building]...
	@echo CXX: $(CXX)
	@echo CXXFLAGS: [$(CXXFLAGS)]
	@echo $(SRCDIR)/.cpp to $(OBJDIR)/.o
	@echo

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

$(EXECFILE): $(OBJFILES)
	@echo
	@echo [Linking]...
	$(CXX) $(OBJFILES) $(LDFLAGS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERFILES) | $(OBJDIR)
	@echo Compiling $<...
	@$(CXX) -c $< $(CXXFLAGS) -o $@
	

$(OBJDIR):
	mkdir $@


.PHONY: build_test
build_test: display_building_test $(TEST_EXECFILE)
	@echo
	$(TEST_EXECFILE)

.PHONY: display_building_test
display_building_test:
	@echo
	@echo [Building tests]...

$(TEST_EXECFILE): $(TEST_DIR)/test_main.o $(TEST_OBJFILES)
	$(CXX) $^ $(LDFLAGS) -o $@

$(TEST_DIR)/test_main.o: $(TEST_DIR)/test_main.cpp $(TEST_HEADERFILES) | $(TEST_DIR)
	$(CXX) -c $< $(CXXFLAGS) -o $@

$(TEST_DIR):
	mkdir $@

.PHONY: clean_test
clean_test:
	rm -rf $(TEST_DIR)/test_main.o
	rm -rf $(TEST_EXECFILE)

.PHONY: clean
clean: clean_test
	rm -rf $(BINDIR)
	
	