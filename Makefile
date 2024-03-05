SRCDIR := ./src
SRCFILES := $(wildcard $(SRCDIR)/*.cpp)

OBJDIR := ./bin
OBJFILES := $(SRCFILES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

HEADERDIR := ./include
HEADERFILES := $(wildcard $(HEADERDIR)/*.hpp)

BINDIR := ./bin
EXECFILE := $(BINDIR)/build.exe

include user_fltk_flags
CXX := $(CXX)
CXXFLAGS := -I./include $(FLTK_CXXFLAGS) --std=c++20 -Wall -pedantic -g3
LDFLAGS := $(FLTK_LDFLAGS)

.PHONY: all
all: $(EXECFILE)

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

$(EXECFILE): $(OBJFILES)
	$(CXX) $(OBJFILES) $(LDFLAGS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERFILES) Makefile | $(OBJDIR)
	$(CXX) -c $< $(CXXFLAGS) -o $@
	

$(OBJDIR):
	mkdir $@

.PHONY: clean
clean:
	rm -rf $(BINDIR)