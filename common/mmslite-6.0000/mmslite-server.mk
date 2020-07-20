include /usr/src/uk/sbase.mk

MMSLITE_DIR := /home/SoftwarePlatform/SSP01/common/mmslite-6.0000

DFLAG = _l
LIBS = \
       $(MMSLITE_DIR)/lib/mvlu$(DFLAG).a \
       $(MMSLITE_DIR)/lib/mlogl$(DFLAG).a \
       $(MMSLITE_DIR)/lib/mmsle$(DFLAG).a \
       $(MMSLITE_DIR)/lib/mmsl$(DFLAG).a \
       $(MMSLITE_DIR)/lib/asn1l$(DFLAG).a \
       $(MMSLITE_DIR)/lib/mem$(DFLAG).a \
       $(MMSLITE_DIR)/lib/slog$(DFLAG).a \
       $(MMSLITE_DIR)/lib/util$(DFLAG).a \
       $(MMSLITE_DIR)/lib/ositcps$(DFLAG).a

DFLAG_D = _ld
LIBS_D = \
		 $(MMSLITE_DIR)/lib/mvlu$(DFLAG).a \
		 $(MMSLITE_DIR)/lib/mlogl$(DFLAG).a \
         $(MMSLITE_DIR)/lib/mmsle$(DFLAG).a \
         $(MMSLITE_DIR)/lib/mmsl$(DFLAG).a \
         $(MMSLITE_DIR)/lib/asn1l$(DFLAG).a \
         $(MMSLITE_DIR)/lib/mem$(DFLAG).a \
         $(MMSLITE_DIR)/lib/slog$(DFLAG).a \
         $(MMSLITE_DIR)/lib/util$(DFLAG).a \
         $(MMSLITE_DIR)/lib/ositcps$(DFLAG).a

MMS_DEFS = -DMMS_LITE -DMOSI -DETHERNET -DLEAN_T -DDEBUG_SISCO -DMVL_UCA -DS_MT_SUPPORT -DSMMS_SERVER

CC_INCLUDES := $(CC_INCLUDES) -I$(MMSLITE_DIR)/inc  -I$(MMSLITE_DIR)/mvl/usr/client $(MMS_DEFS)
CC_LINK     := $(CC_LINK) $(LIBS)
CC_LINK_D   := $(CC_LINK_D) $(LIBS_D)

