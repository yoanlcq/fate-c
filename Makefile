.PHONY: all games
all: games

include fatefile.mk

FATE:=$(FE_PATH)

FE_CAI_EXCLUDE := $(wildcard \
	$(FATE)/src/fate/cai/* \
	$(FATE)/src/fate/crash.c \
)
	#$(FATE)/src/fate/log.c
	#$(FATE)/src/fate/hw.c
	#$(FATE)/src/fate/hash.c
	#$(FATE)/src/cube/main.c
FE_CAI_ENABLE := yes

ifneq ($(OS),android)
include $(FATE)/make/recursive_wildcard.mk
include $(FATE)/make/os_arch_cc.mk
include $(FATE)/make/$(CC).mk
include $(FATE)/make/fate.mk
endif

ifeq ($(OS),android)
include $(FATE)/make/game_android.mk
else
include $(FATE)/make/game.mk
endif

games: $(GAMES) 

