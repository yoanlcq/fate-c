.PHONY: all games
all: games

CAI_EXCLUDE := $(wildcard \
	src/fate/cai/* \
	src/fate/crash.c \
)
	#src/fate/log.c
	#src/fate/hw.c
	#src/fate/hash.c
	#src/cube/main.c
#CAI_ENABLE := yes

ifneq ($(OS),android)
include make/os_arch_cc.mk
include make/$(CC).mk
include make/fate.mk
endif

# We don't support having multiple games with this.
# GAME = toast
# include make/game.mk
GAME = cube
ifeq ($(OS),android)
include make/game_android.mk
else
include make/game.mk
endif

games: $(GAMES) 

