PROJ ?= $(notdir $(abspath .))
SRCS = $(wildcard src/*.c)
INCS = inc
SHM_PREFIX = $(notdir $(abspath ../))

LDFLAGS += -lrt -lpthread
CFLAGS += -DCOM_PREFIX="\"$(SHM_PREFIX)\""
RM_OBJS := /dev/shm/*$(SHM_PREFIX)*

include ../../../common.mk
