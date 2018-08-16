# @copyright MbientLab License

APP_NAME?=warble
MODULES?=warble

KERNEL?=$(shell uname -s)

ifndef MACHINE
    MACHINE_RAW:=$(shell uname -m)
    ifeq ($(MACHINE_RAW),x86_64)
        MACHINE:=x64
    else ifeq ($(MACHINE_RAW),amd64)
        MACHINE:=x64
    else ifneq (,$(findstring arm, $(MACHINE_RAW)))
        MACHINE:=arm
    else ifeq ($(MACHINE_RAW),aarch64)
        MACHINE:=arm
    else
        MACHINE:=x86
    endif
endif

INSTALL_PATH?=/usr/local
DOC_DIR?=doc
BUILD_DIR?=build
DIST_DIR?=dist
CONFIG?=release
