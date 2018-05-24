# @copyright MbientLab License
M_DIR:=src/bleat
GEN:= $(GEN) $(M_DIR)/cpp/lib_def.h

$(M_DIR)/cpp/lib.cpp: $(M_DIR)/cpp/lib_def.h

.PHONY: $(M_DIR)/cpp/lib_def.h
$(M_DIR)/cpp/lib_def.h:
	@echo '#pragma once' > $@
	@echo '#define BLEAT_LIB_VERSION "$(VERSION)"' >> $@
	@echo '#define BLEAT_LIB_CONFIG "$(CONFIG)"' >> $@
