### PROJECT

TARGET := $(shell basename $(shell pwd))
SOURCES := $(wildcard *.cpp)

### CONFIG

BUILD_MODE ?= DEBUG

ifeq ($(BUILD_MODE), RELEASE) 
CXX = g++
CXXFLAGS = -pipe -pedantic -Wall -Wextra -O2 -fopenmp -g
else
CXX = g++
CXXFLAGS = -pipe -pedantic -Wall -Wextra -g2
endif

LDFLAGS = -lm -Wl,--as-needed

VERBOSE_MAKE ?= 0

ifeq ($(VERBOSE_MAKE),1)
	A := 
else
	A := @
endif

OBJDIR := .obj
DEPDIR := .dep

CFLAGS += $(addprefix -I,$(INCLUDEPATH))
CFLAGS += $(addprefix -D,$(DEFINES))
OBJECTS := $(patsubst %.cpp,$(OBJDIR)/%.o,$(SOURCES))
DEPENDS := $(patsubst %.cpp,$(DEPDIR)/%.d,$(SOURCES))

GENERATED := $(TARGET) 

### RULES

.PHONY: all clean

all: $(TARGET)

clean:
	@echo -e "\tRM\t$(GENERATED)"
	$(A)rm -f $(GENERATED) 
	@echo -e "\tRM\t$(OBJDIR) $(DEPDIR)"
	$(A)rm -rf $(OBJDIR) $(DEPDIR)

$(OBJDIR)/%.o: %.cpp
	@echo -e "\tCXX\t$<"
	$(A)$(CXX) $(CXXFLAGS) -o $@ -c $<

$(DEPDIR)/%.d: %.cpp
	@echo -e "\tDEP\t$<"
	$(A)$(CXX) $(CXXFLAGS) -o $@ -MT "$(OBJDIR)/$*.o" -MM $< 

$(TARGET): $(OBJECTS)
	@echo -e "\tLD\t$^"
	$(A)$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

$(OBJECTS): Makefile | $(OBJDIR)

$(DEPENDS): Makefile | $(DEPDIR)

$(OBJDIR):
	@mkdir -p $@

$(DEPDIR):
	@mkdir -p $@

-include $(DEPENDS)
