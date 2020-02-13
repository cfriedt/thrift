.PHONY: all clean check

TAG = hello
TOOLCHAIN=gcc
QUIET=@
DEBUG=1

GENCDIR = gen-c
GENCXXDIR = gen-cpp
GENPYDIR = gen-py
GENDIR = $(GENCXXDIR) $(GENPYDIR)

ifeq (gcc,$(TOOLCHAIN))
CROSS_COMPILE ?=
CC  := $(CROSS_COMPILE)gcc
CXX := $(CROSS_COMPILE)g++
AR  := $(CROSS_COMPILE)ar
endif
ifeq (clang,$(TOOLCHAIN))
CC  := clang
CXX := clang++
AR  := ar
endif

ARFLAGS :=
CFLAGS :=
CPPFLAGS :=
CXXFLAGS :=
LDFLAGS :=

ARFLAGS += crs
LDFLAGS += $(shell pkg-config --libs thrift)
LDFLAGS += -Wl,-rpath,/usr/local/lib
CFLAGS += -Wall -Wextra -Werror
#CFLAGS += -Wpedantic 
##pedantic, but not *that* pedantic
#CFLAGS += -Wno-variadic-macros -Wno-gnu-zero-variadic-macro-arguments
##pedantic, but not *that* pedantic
#CFLAGS += -Wno-gnu-statement-expression
CFLAGS += -mtune=native
ifeq ($(DEBUG),)
CFLAGS += -O3 -fomit-frame-pointer -DNDEBUG
else
CFLAGS += -O0 -g -DDEBUG
endif
CXXFLAGS += $(CFLAGS)
CFLAGS += -std=c11
CXXFLAGS += -std=c++14

ifeq (gcc,$(TOOLCHAIN))
CXXFLAGS += -D_GLIBCXX_USE_CXX11_ABI=1
endif

ifeq (clang,$(TOOLCHAIN))
CXXFLAGS += -stdlib=libc++
endif

THRIFT_CXX_LIBS = $(shell pkg-config --libs thrift)

GTEST_CFLAGS = $(shell pkg-config --cflags gtest gtest_main)
GTEST_LIBS = $(shell pkg-config --libs gtest gtest_main)

CLIB := lib$(TAG).a
CXXLIB := lib$(TAG)xx.a
LIB = $(CLIB) $(CXXLIB)
EXE :=

CPPFLAGS += -I.
CPPFLAGS += -Isrc
CPPFLAGS += $(shell pkg-config --cflags-only-I thrift)

TSRC := $(shell find * -name '*.thrift')

CSRC :=
CHDR :=
CXXSRC :=
HDR :=
PYSRC :=
CXXEXE :=
PYEXE :=
CEXE :=
TEST :=

OBJ :=

BUILT_SOURCES :=
CLEANFILES :=

CSRC += $(shell find * -name '*.c' | grep -v "$(GENCDIR)")
CXXSRC += $(shell find * -name '*.cpp' | grep -v "$(GENCXXDIR)")
CXXSRC := $(filter-out %-test.cpp,$(CXXSRC))
PYSRC += $(shell find * -name '*.py' | grep -v "$(GENPYDIR)")
TEST += $(shell find * -name '*-test.cpp')

TESTEXE := $(TEST:.cpp=)

# $1 := base of the thrift filename
# $2 := $1 with first letter capitalized
#define sources-thrift-to-c =
#CSRC += $(GENCDIR)/$(1)_constants.c $(GENCDIR)/$(1)_types.c $(GENCDIR)/$(2).c
#BUILT_SOURCES += $(GENCDIR)/$(1)_constants.c $(GENCDIR)/$(1)_types.c $(GENCDIR)/$(2).c
#CLEANFILES += $(GENCDIR)/$(2)_server.skeleton.c
#endef

# $1 := base of the thrift filename
# $2 := $1 with first letter capitalized
define sources-thrift-to-cpp =
CXXSRC += $(GENCXXDIR)/$(1)_constants.cpp $(GENCXXDIR)/$(1)_types.cpp $(GENCXXDIR)/$(2).cpp
BUILT_SOURCES += $(GENCXXDIR)/$(1)_constants.cpp $(GENCXXDIR)/$(1)_types.cpp $(GENCXXDIR)/$(2).cpp
CLEANFILES += $(GENCXXDIR)/$(2)_server.skeleton.cpp
endef

# $1 := base of the thrift filename
# $2 := $1 with first letter capitalized
define sources-thrift-to-py =
PYSRC += $(GENPYDIR)/$(1)/constants.py $(GENPYDIR)/$(1)/ttypes.py $(GENPYDIR)/$(2).py
BUILT_SOURCES += $(GENPYDIR)/$(1)/constants.py $(GENPYDIR)/$(1)/ttypes.py $(GENPYDIR)/$(2).py
CLEANFILES += $(GENPYDIR)/$(1)/$(2)-remote
endef

$(foreach _t, $(TSRC:.thrift=), \
	$(eval _T=$(shell echo $(_t) | sed -e 's/^\([a-z]\)/\U\1/')) \
	$(eval $(call sources-thrift-to-cpp,$(_t),$(_T))) \
	$(eval CXXEXE += $(_T)-cxx-client) \
	$(eval CXXEXE += $(_T)-cxx-server) \
	$(eval $(call sources-thrift-to-py,$(_t),$(_T))) \
	$(eval PYEXE += $(_T)-py-client) \
	$(eval PYEXE += $(_T)-py-server) \
)

#	$(eval CEXE += $(_T)-c-client) \
#	$(eval CEXE += $(_T)-c-server) \
#	$(eval $(call sources-thrift-to-c,$(_t),$(_T))) \
#

EXE += $(CXXEXE) $(CEXE) $(PYEXE) $(TESTEXE)

all: $(LIB) $(EXE)

# $1 := base of the thrift filename
# $2 := $1 with first letter capitalized
#define transform-thrift-to-c =
#$(GENCDIR)/$(1)_constants.h: $(1).thrift
#	@echo "[THRIFT][C] $$<"
#	$(QUIET)thrift --gen c $$<
#$(GENCDIR)/$(1)_constants.c: $(GENCDIR)/$(1)_constants.h
#	@touch $$@
#$(GENCDIR)/$(1)_types.h: $(GENCDIR)/$(1)_constants.c
#	@touch $$@
#$(GENCDIR)/$(1)_types.c: $(GENCDIR)/$(1)_types.h
#	@touch $$@
#$(GENCDIR)/$(2).h: $(GENCDIR)/$(1)_types.c
#	@touch $$@
#$(GENCDIR)/$(2).c: $(GENCDIR)/$(2).h
#	@touch $$@
#$(GENCDIR)/$(2)_server.skeleton.c: $(GENCDIR)/$(2).c
#	@touch $$@
#endef

# $1 := base of the thrift filename
# $2 := $1 with first letter capitalized
define transform-thrift-to-cpp =
$(GENCXXDIR)/$(1)_constants.h: $(1).thrift
	@echo "[THRIFT][CXX] $$<"
	$(QUIET)thrift --gen cpp $$<
$(GENCXXDIR)/$(1)_constants.cpp: $(GENCXXDIR)/$(1)_constants.h
	@touch $$@
$(GENCXXDIR)/$(1)_types.h: $(GENCXXDIR)/$(1)_constants.cpp
	@touch $$@
$(GENCXXDIR)/$(1)_types.cpp: $(GENCXXDIR)/$(1)_types.h
	@touch $$@
$(GENCXXDIR)/$(2).h: $(GENCXXDIR)/$(1)_types.cpp
	@touch $$@
$(GENCXXDIR)/$(2).cpp: $(GENCXXDIR)/$(2).h
	@touch $$@
$(GENCXXDIR)/$(2)_server.skeleton.cpp: $(GENCXXDIR)/$(2).cpp
	@touch $$@
endef

# $1 := base of the thrift filename
# $2 := $1 with first letter capitalized
define transform-thrift-to-py =
$(GENPYDIR)/$(1)/constants.py: $(1).thrift
	@echo "[THRIFT][PY] $$<"
	$(QUIET)thrift --gen py $$<
$(GENPYDIR)/$(1)/ttypes.py: $(GENPYDIR)/$(1)/constants.py
	@touch $$@
$(GENPYDIR)/$(1)/$(2).py: $(GENPYDIR)/$(1)/ttypes.py
	@touch $$@
$(GENPYDIR)/$(1)/$(2)-remote: $(GENPYDIR)/$(1)/ttypes.py
	@touch $$@
$(2)-py-server: $(2)-py-server.py $(GENPYDIR)/$(1)/$(2).py
	@echo "[PY] $$@"
	$(QUIET)ln -sf $$< $$@
$(2)-py-client: $(2)-py-client.py $(GENPYDIR)/$(1)/$(2).py
	@echo "[PY] $$@"
	$(QUIET)ln -sf $$< $$@
endef

$(foreach _t, $(TSRC:.thrift=), \
	$(eval _T=$(shell echo $(_t) | sed -e 's/^\([a-z]\)/\U\1/')) \
	$(eval $(call transform-thrift-to-cpp,$(_t),$(_T))) \
	$(eval $(call transform-thrift-to-py,$(_t),$(_T))) \
	$(eval $(call transform-thrift-to-c,$(_t),$(_T))) \
)

CXXSRC := $(filter-out $(addsuffix .cpp,$(CXXEXE)),$(CXXSRC))
CXXOBJ = $(CXXSRC:.cpp=.o)
OBJ += $(CXXOBJ)

CSRC := $(filter-out $(addsuffix .c,$(CEXE)),$(CSRC))
#COBJ := $(CSRC:.c=.o) $(GENCDIR)/ThriftTest.o $(GENCDIR)/ThriftTest_types.o $(GENCDIR)/ThriftTest_constants.o
COBJ := $(CSRC:.c=.o)
OBJ += $(COBJ)

$(CLIB): $(COBJ)
	@echo "[AR] $@"
	$(QUIET)$(AR) $(ARFLAGS) $@ $^

$(CXXLIB): $(CXXOBJ)
	@echo "[AR] $@"
	$(QUIET)$(AR) $(ARFLAGS) $@ $^

%-cxx-server: %-cxx-server.cpp $(CXXLIB)
	@echo "[CXXLD] $@"
	$(QUIET)$(CXX) $(CPPFLAGS) -I$(GENCXXDIR) $(CXXFLAGS) $(LDFLAGS) $< -o $@ $(CXXLIB) $(THRIFT_CXX_LIBS)

%-test: %-test.cpp $(CXXLIB)
	@echo "[CXXLD] $@"
	$(QUIET)$(CXX) $(CPPFLAGS) -I$(GENCXXDIR) $(CXXFLAGS) $(GTEST_CXXFLAGS) $(LDFLAGS) $< -o $@ $(CXXLIB) $(THRIFT_CXX_LIBS) $(GTEST_LIBS)

%-cxx-client: %-cxx-client.cpp $(CXXLIB)
	@echo "[CXXLD] $@"
	$(QUIET)$(CXX) $(CPPFLAGS) -I$(GENCXXDIR) $(CXXFLAGS) $(LDFLAGS) $< -o $@ $(CXXLIB) $(THRIFT_CXX_LIBS)

%-c-server: %-c-server.c $(CLIB)
	@echo "[LD] $@"
	$(QUIET)$(CC) $(CPPFLAGS) -I$(GENCDIR) $(CFLAGS) $(LDFLAGS) $< -o $@ $(CLIB)

%-c-client: %-c-client.c $(CLIB)
	@echo "[LD] $@"
	$(QUIET)$(CC) $(CPPFLAGS) -I$(GENCDIR) $(CFLAGS) $(LDFLAGS) $< -o $@ $(CLIB)

%.o: %.cpp $(HDR)
	@echo "[CXX] $@"
	$(QUIET)$(CXX) $(CPPFLAGS) -I$(GENCXXDIR) $(CXXFLAGS) -c $< -o $@

%.o: %.c $(HDR)
	@echo "[CC] $@"
	$(QUIET)$(CC) $(CPPFLAGS) -I$(GENCDIR) $(CFLAGS) -c $< -o $@

clean:
	@echo "[RM] LIB"
	$(QUIET)rm -Rf $(LIB)
	@echo "[RM] EXE"
	$(QUIET)rm -Rf $(EXE)
	@echo "[RM] OBJ"
	$(QUIET)rm -Rf $(OBJ)
	@echo "[RM] GEN"
	$(QUIET)rm -Rf $(GENDIR) $(BUILT_SOURCES) $(CLEANFILES)
	$(QUIET)chmod -x *.py

check: $(TESTEXE)
	for i in $$^; do \
		./$$i || break; \
	done
