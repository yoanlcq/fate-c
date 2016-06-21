.PHONY: all games
all: games

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

