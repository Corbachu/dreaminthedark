# ~CA, 11/10/22
# KOS stand-alone libtess.a, really needed for GLdc and any gluTESS that GLdc does not have. . 


CC=kos-cc
CXX=kos-c++
PROGRAM = libgltess.a

KOS_CFLAGS=-O0 -fno-omit-frame-pointer -fno-common -fno-builtin -fexceptions -ffast-math -fno-strict-aliasing -Wall -pipe -ml -m4-single-only -D_GNU_SOURCE=1 -D_REENTRANT -DINLINE_G=__inline__
#CFLAGS= -fPIC -O0 -g -Wall -c -fpermissive
LDFLAGS=-lm -Wl,--warn-common
#Map=output.map

CFLAGS += -I. -Dnullptr=NULL

CFLAGS += $(KOS_CFLAGS)

INC = -I../Import

OBJ_DIR=./obj

#INC_DIR=./include

OUT_DIR=./lib


# Enumerating of every *.c as *.o and using that as dependency
$(PROGRAM): $(patsubst %.c,$(OBJ_DIR)/%.o,$(wildcard *.c))
	ar -r -o $(OUT_DIR)/$@ $^

# Send all headers to folder named 'include' (for KOS-PORTS)...
#$(OUT_FILE_NAME): $(patsubst %.h,$(wildcard *.h))
#	ar -r -o $(OUT_DIR)/$@ $^


all: makedirs $(PROGRAM)

stripped: $(PROGRAM)
	kos-strip --strip-unneeded $(PROGRAM)


makedirs:
	@mkdir -p $(OUT_DIR)
	@mkdir -p $(OBJ_DIR)

clean:
	rm -f $(OBJ_DIR)/*.o $(OUT_DIR)/$(PROGRAM) Makefile.bak

rebuild: clean build

.PHONY: all stripped clean makedirs rebuild

# ---------- FINAL LINK STEP -----------

$(OBJ_DIR)/%.o: %.c makedirs
	$(CC) -c $(INC) $(CFLAGS) -o $@  $<
	