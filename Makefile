.PHONY: all games
all: games

include make/os_arch_cc.mk
include make/$(CC).mk
include make/fate.mk

GAME = cube
include make/game.mk
GAME = toast
include make/game.mk

games: $(GAMES) 

