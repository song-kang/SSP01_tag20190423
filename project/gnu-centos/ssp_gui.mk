include /usr/src/uk/ssp_bases.mk

CC_INCLUDES := $(CC_INCLUDES) -I$(SSP_DIR)/include/ssp_gui

CC_LINK     := $(CC_LINK) -lssp_gui
CC_LINK_D   := $(CC_LINK_D) -lssp_guid
