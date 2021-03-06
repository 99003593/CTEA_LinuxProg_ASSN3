
# Control printing with verbosity
ifeq ($(V),1)
	Q :=
else
	Q := @
endif

.PHONY: all consumer producer

all: producer consumer

producer:
	$(Q)$(MAKE) -C $@ OBJ_DIR=$(abspath ./build/$@)

consumer:
	$(Q)$(MAKE) -C $@ OBJ_DIR=$(abspath ./build/$@)

run: all
	$(Q)$(MAKE) -C producer OBJ_DIR=$(abspath ./build/producer) run &
	$(Q)$(MAKE) -C consumer OBJ_DIR=$(abspath ./build/consumer) run

clean:
	$(Q)$(MAKE) -C producer OBJ_DIR=$(abspath ./build) $@
	$(Q)$(MAKE) -C consumer OBJ_DIR=$(abspath ./build) $@
	$(Q)rm -rf ./build
