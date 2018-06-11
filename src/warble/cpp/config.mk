# @copyright MbientLab License
M_DIR:=src/warble
GEN:= $(GEN) $(M_DIR)/cpp/lib_def.h

$(M_DIR)/cpp/lib.cpp: $(M_DIR)/cpp/lib_def.h

ifndef SKIP_VERSION
    .PHONY: $(M_DIR)/cpp/lib_def.h
endif
$(M_DIR)/cpp/lib_def.h:
	@echo '#pragma once' > $@
	@echo '#define WARBLE_LIB_VERSION "$(VERSION)"' >> $@
	@echo '#define WARBLE_LIB_CONFIG "$(CONFIG)"' >> $@
