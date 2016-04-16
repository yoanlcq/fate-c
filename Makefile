.PHONY: all games
all: games

include make/os_arch_cc.mk
include make/$(CC).mk
include make/fate.mk

#We don't support having multiple games with this.
GAME = toast
include make/game.mk
GAME = cube
include make/game.mk

games: $(GAMES) 

