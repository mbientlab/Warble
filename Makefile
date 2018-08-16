# @copyright MbientLab License

.PHONY: build clean doc publish install

VERSION_MK=version.mk
ifndef SKIP_VERSION
    $(shell ./bin/generate_version.sh $(VERSION_MK))
endif

include config.mk
include $(VERSION_MK)

CXXFLAGS:=-std=c++14 -fPIC -fvisibility=hidden -fvisibility-inlines-hidden -Wall -Werror -DWARBLE_DLL -DWARBLE_DLL_EXPORT -Isrc 

DEPS_BLEPP:=deps/libblepp

MODULES_SRC_DIR= $(addsuffix /cpp, $(addprefix src/, $(MODULES)))
GEN:=
SRCS:=$(foreach src_dir, $(MODULES_SRC_DIR), $(shell find $(src_dir) -name \*.cpp))
EXPORT_HEADERS:=$(foreach module, $(addprefix src/, $(MODULES)), $(shell find $(module) -maxdepth 1 -name \*.h))

ifeq ($(CONFIG),debug)
    CXXFLAGS+=-g
    LD_FLAGS:=-g
else ifeq ($(CONFIG),release)
    CXXFLAGS+=-O3
    LD_FLAGS:=-s
else
    $(error Invalid value for "CONFIG", must be 'release' or 'debug')
endif

LD_FLAGS+=-shared -Wl,
ifneq ($(KERNEL),Darwin)
    EXTENSION:=so
    LIB_SO_NAME:=lib$(APP_NAME).so
    LD_FLAGS:=$(LD_FLAGS)--soname
    CXXFLAGS+= -DAPI_BLEPP -I$(DEPS_BLEPP)
else
    EXTENSION:=dylib
    LD_FLAGS:=-dynamiclib $(LD_FLAGS)-install_name
endif

ifeq ($(MACHINE),x86)
    ARCH=-m32
    INSTALL_LIB?=lib
else ifeq ($(MACHINE),x64)
    ARCH=-m64
    INSTALL_LIB?=lib64
else ifeq ($(MACHINE),arm)
    ARCH=-marm
    INSTALL_LIB?=lib
else
    $(error Unrecognized "MACHINE" value, use 'x86', 'x64', or 'arm')
endif

ifndef NO_MULTILIB
    CXXFLAGS+=$(ARCH)
else
    ARCH=
endif

LIB_SO_NAME:=lib$(APP_NAME).$(EXTENSION)
LIB_SHORT_NAME:=$(LIB_SO_NAME).$(VERSION_MAJOR)
LIB_NAME:=$(LIB_SO_NAME).$(VERSION)
LD_FLAGS:=$(LD_FLAGS),$(LIB_SHORT_NAME) $(ARCH)

REAL_DIST_DIR:=$(DIST_DIR)/$(CONFIG)/lib/$(MACHINE)
REAL_BUILD_DIR:=$(BUILD_DIR)/$(MACHINE)/$(CONFIG)
MODULES_BUILD_DIR:=$(addprefix $(REAL_BUILD_DIR)/, $(MODULES_SRC_DIR))

OBJS:=$(addprefix $(REAL_BUILD_DIR)/,$(SRCS:%.cpp=%.o))
DEPS:=$(OBJS:%.o=%.d)

APP_OUTPUT:=$(REAL_DIST_DIR)/$(LIB_NAME)

build: $(APP_OUTPUT)

install: $(APP_OUTPUT)
	cp -P $(REAL_DIST_DIR)/* $(INSTALL_PATH)/$(INSTALL_LIB)
	install -d $(INSTALL_PATH)/include/$(APP_NAME)
	install -m644  -D $(EXPORT_HEADERS) $(INSTALL_PATH)/include/$(APP_NAME)

$(REAL_BUILD_DIR)/%.o: %.cpp
	$(CXX) -MMD -MP -MF "$(@:%.o=%.d)" -c -o $@ $(CXXFLAGS) $<

include $(addsuffix /config.mk, $(MODULES_SRC_DIR))
-include $(DEPS)

$(MODULES_BUILD_DIR):
	mkdir -p $@

$(REAL_DIST_DIR):
	mkdir -p $@

$(OBJS): | $(MODULES_BUILD_DIR)
$(APP_OUTPUT): $(OBJS) $(DEPS_BLEPP)/libble++.a | $(REAL_DIST_DIR) 
	$(CXX) -o $@ $(LD_FLAGS) $^ -lbluetooth -lpthread
	ln -sf $(LIB_NAME) $(REAL_DIST_DIR)/$(LIB_SHORT_NAME)
	ln -sf $(LIB_SHORT_NAME) $(REAL_DIST_DIR)/$(LIB_SO_NAME)

PUBLISH_NAME:=$(APP_NAME)-$(VERSION).tar
PUBLISH_NAME_ZIP:=$(PUBLISH_NAME).gz

publish: $(DIST_DIR)/$(PUBLISH_NAME_ZIP)

$(DIST_DIR)/$(PUBLISH_NAME_ZIP): $(BUILD_DIR)/$(PUBLISH_NAME)
	gzip --stdout $< > $@

$(BUILD_DIR)/$(PUBLISH_NAME): build
	tar -cf $@ --transform 's,src,include,' $(EXPORT_HEADERS)
	tar -rf $@ -C $(DIST_DIR) .

clean:
	rm -Rf $(BUILD_DIR) $(DIST_DIR) $(GEN)

cleanest: clean
	make clean -C $(DEPS_BLEPP)
	rm $(DEPS_BLEPP)/Makefile

doc:
	rm -Rf $(DOC_DIR)
	mkdir $(DOC_DIR)
	doxygen Doxyfile

$(DEPS_BLEPP)/libble++.a: $(DEPS_BLEPP)/Makefile
	make -C $(DEPS_BLEPP) -j

$(DEPS_BLEPP)/Makefile:
	cd $(DEPS_BLEPP); LDFLAGS=$(ARCH) ./configure
