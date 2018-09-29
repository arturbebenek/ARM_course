##############################################################################
# Product: Makefile for QUTEST; QP/C on POSIX *Host*
# Last updated for version 6.3.3
# Last updated on  2018-07-10
#
#                    Q u a n t u m     L e a P s
#                    ---------------------------
#                    innovating embedded systems
#
# Copyright (C) 2005-2018 Quantum Leaps, LLC. All rights reserved.
#
# This program is open source software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Alternatively, this program may be distributed and modified under the
# terms of Quantum Leaps commercial licenses, which expressly supersede
# the GNU General Public License and are specifically designed for
# licensees interested in retaining the proprietary status of their code.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#
# Contact information:
# https://www.state-machine.com
# mailto:info@state-machine.com
##############################################################################
#
# examples of invoking this Makefile:
# make -f posix_host.mak        # make and run the tests in the current directory
# make -f posix_host.mak TESTS=philo*.tcl  # make and run the selected tests
# make -f posix_host.mak SCRIPT=py # make and run the Python tests
# make -f posix_host.mak HOST=localhost:7705 # connect to host:port
# make -f posix_host.mak norun   # only make but not run the tests
# make -f posix_host.mak clean   # cleanup the build
#

#-----------------------------------------------------------------------------
# project name
#
PROJECT := test_qutest

#-----------------------------------------------------------------------------
# project directories
#

# location of the QP/C framework (if not provided in an environemnt var.)
ifeq ($(QPC),)
QPC := ../../..
endif

# make sure that QTOOLS exists...
ifeq ("$(wildcard $(QTOOLS))","")
$(error QTOOLS not found. Please install QTools and define QTOOLS env. variable)
endif


# QP port used in this project
QP_PORT_DIR := $(QPC)/ports/posix-qutest

# list of all source directories used by this project
VPATH = \
	. \
	$(QPC)/src/qf \
	$(QPC)/src/qs \
	$(QP_PORT_DIR)

# list of all include directories needed by this project
INCLUDES  = \
	-I. \
	-I$(QPC)/include \
	-I$(QPC)/src \
	-I$(QP_PORT_DIR)

#-----------------------------------------------------------------------------
# files
#

# C source files...
C_SRCS := \
	test_qutest.c

# C++ source files...
CPP_SRCS :=

QP_SRCS := \
	qep_hsm.c \
	qep_msm.c \
	qf_act.c \
	qf_actq.c \
	qf_defer.c \
	qf_dyn.c \
	qf_mem.c \
	qf_ps.c \
	qf_qact.c \
	qf_qeq.c \
	qf_qmact.c \
	qs.c \
	qs_64bit.c \
	qs_rx.c \
	qs_fp.c \
	qutest.c \
	qutest_port.c

LIB_DIRS  :=
LIBS      :=

# defines...
DEFINES   :=

#-----------------------------------------------------------------------------
# GNU toolset
#
CC    := gcc
CPP   := g++
LINK  := gcc    # for C programs
#LINK  := g++   # for C++ programs

# basic utilities

MKDIR  := mkdir -p
RM     := rm -f

ifeq ($(SCRIPT),py)
PYTHON := python -m
QUTEST := qspypy.qutest
else
TCLSH  := tclsh
QUTEST := $(QTOOLS)/qspy/tcl/qutest.tcl
endif

#============================================================================
# Typically you should not need to change anything below this line

#-----------------------------------------------------------------------------
# build options
#

# combine all the soruces...
C_SRCS += $(QP_SRCS)

BIN_DIR := posix_host

CFLAGS = -g -O -Wall -Wstrict-prototypes -W $(INCLUDES) $(DEFINES) \
	-DQ_SPY -DQ_UTEST -DQ_HOST

CPPFLAGS = -g -O -Wall -W -fno-rtti -fno-exceptions $(INCLUDES) $(DEFINES) \
	-DQ_SPY -DQ_UTEST -DQ_HOST

LINKFLAGS := -Wl,-Map,$(BIN_DIR)/$(PROJECT).map,--cref,--gc-sections

#-----------------------------------------------------------------------------
# combine all the soruces...
INCLUDES  += -I$(QP_PORT_DIR)
LIB_DIRS  += -L$(QP_PORT_DIR)/$(BIN_DIR)

C_OBJS       := $(patsubst %.c,%.o,   $(C_SRCS))
CPP_OBJS     := $(patsubst %.cpp,%.o, $(CPP_SRCS))

TARGET_EXE   := $(BIN_DIR)/$(PROJECT)
C_OBJS_EXT   := $(addprefix $(BIN_DIR)/, $(C_OBJS))
C_DEPS_EXT   := $(patsubst %.o,%.d, $(C_OBJS_EXT))
CPP_OBJS_EXT := $(addprefix $(BIN_DIR)/, $(CPP_OBJS))
CPP_DEPS_EXT := $(patsubst %.o,%.d, $(CPP_OBJS_EXT))

# create $(BIN_DIR) if it does not exist
ifeq ("$(wildcard $(BIN_DIR))","")
$(shell $(MKDIR) $(BIN_DIR))
endif

#-----------------------------------------------------------------------------
# rules
#

.PHONY : run norun

ifeq ($(MAKECMDGOALS),norun)
all : $(TARGET_EXE)
norun : all
else
all : $(TARGET_EXE) run
endif

ifeq (, $(TESTS))
ifeq ($(SCRIPT),py)
TESTS := *.py
else
TESTS := *.tcl
endif
endif

$(TARGET_EXE) : $(C_OBJS_EXT) $(CPP_OBJS_EXT)
	$(CC) $(CFLAGS) -c $(QPC)/include/qstamp.c -o $(BIN_DIR)/qstamp.o
	$(LINK) $(LINKFLAGS) $(LIB_DIRS) -o $@ $^ $(BIN_DIR)/qstamp.o $(LIBS)

ifeq ($(SCRIPT),py)
run : $(TARGET_EXE)
	$(PYTHON) $(QUTEST) $(TESTS) $(TARGET_EXE) $(HOST)
else
run : $(TARGET_EXE)
	$(TCLSH) $(QUTEST) $(TESTS) $(TARGET_EXE) $(HOST)
endif

$(BIN_DIR)/%.d : %.cpp
	$(CPP) -MM -MT $(@:.d=.o) $(CPPFLAGS) $< > $@

$(BIN_DIR)/%.d : %.c
	$(CC) -MM -MT $(@:.d=.o) $(CFLAGS) $< > $@

$(BIN_DIR)/%.o : %.cpp
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(BIN_DIR)/%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY : clean show norun

# include dependency files only if our goal depends on their existence
ifneq ($(MAKECMDGOALS),clean)
  ifneq ($(MAKECMDGOALS),show)
-include $(C_DEPS_EXT) $(CPP_DEPS_EXT)
  endif
endif

clean :
	-$(RM) $(BIN_DIR)/*.o \
	$(BIN_DIR)/*.d \
	$(BIN_DIR)/*.map \
	$(TARGET_EXE)

show :
	@echo PROJECT      = $(PROJECT)
	@echo TARGET_EXE   = $(TARGET_EXE)
	@echo CONF         = $(CONF)
	@echo VPATH        = $(VPATH)
	@echo C_SRCS       = $(C_SRCS)
	@echo CPP_SRCS     = $(CPP_SRCS)
	@echo C_DEPS_EXT   = $(C_DEPS_EXT)
	@echo C_OBJS_EXT   = $(C_OBJS_EXT)
	@echo C_DEPS_EXT   = $(C_DEPS_EXT)
	@echo CPP_DEPS_EXT = $(CPP_DEPS_EXT)
	@echo CPP_OBJS_EXT = $(CPP_OBJS_EXT)
	@echo LIB_DIRS     = $(LIB_DIRS)
	@echo LIBS         = $(LIBS)
	@echo DEFINES      = $(DEFINES)
	@echo QTOOLS       = $(QTOOLS)
	@echo QUTEST       = $(QUTEST)
	@echo TESTS        = $(TESTS)
	@echo HOST         = $(HOST)

