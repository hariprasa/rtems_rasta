###############################################################################
# Type:
#  - "make"     to compile the example
#  - "make run" to compile and run
###############################################################################

# C source names
CSRCS = init.c rtems_spw.c rtems_tcpip.c

############# https://docs.rtems.org/releases/rtemsdocs-4.10.2/share/rtems/html/networking/networking00016.html
##MANAGERS = io event semaphore
##CFLAGS_LD += -Wl,--defsym -Wl,HeapSize=0x80000


############# https://docs.rtems.org/doc-current/share/rtems/html/networking/Network-Driver-Makefile.html#Network-Driver-Makefile
#DEFINES += -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
#MANAGERS = io event semaphore
#CFLAGS_LD += -Wl,--defsym -Wl,HeapSize=0x80000
###############################################################################
# DO NOT EDIT AFTER THIS LINE

EXEC=test.exe
PGM=${ARCH}/$(EXEC)

COBJS_ = $(CSRCS:.c=.o)
COBJS = $(COBJS_:%=${ARCH}/%)

MANAGERS = all        # optional managers required
LIBS = -lrtemsall -lc # libraries

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

CFLAGS += -g -O0 -ggdb -gdwarf-2

OBJS= $(COBJS) $(CXXOBJS) $(ASOBJS)

all: clean ${ARCH} $(PGM)

$(PGM): $(OBJS)
	$(make-exe)

run: all
	tsim-leon3 ${ARCH}/$(EXEC)

help:
	@echo Type:
	@echo  - "make"     to compile the example
	@echo  - "make run" to compile and run
send:
	scp ./o-optimize/test.exe root@prise-space-3:~/taste_examples/
