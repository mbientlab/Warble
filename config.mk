APP_NAME?=bleat
MODULES?=bleat

KERNEL?=$(shell uname -s)

ifndef MACHINE
    MACHINE_RAW:=$(shell uname -m)
    ifeq ($(MACHINE_RAW),x86_64)
        MACHINE:=x64
    else ifeq ($(MACHINE_RAW),amd64)
        MACHINE:=x64
    else ifneq (,$(findstring arm, $(MACHINE_RAW)))
        MACHINE:=arm
    else
        MACHINE:=x86
    endif
endif


DOC_DIR=doc

SOURCE_DIR?=src
BUILD_DIR?=build
DIST_DIR?=dist
CONFIG?=release