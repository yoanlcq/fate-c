.PHONY: all game
all: game

include fatefile.mk
include $(fate)/make/cc.mk
include $(fate)/make/os.mk
include $(fate)/make/arch.mk
include $(fate)/make/os_utils.mk
include $(fate)/make/shell_fix.mk
include $(fate)/make/toolchains.mk
include $(fate)/make/windres.mk
include $(fate)/make/rglob.mk
include $(fate)/make/see_cmd.mk
include $(fate)/make/clean.mk
include $(fate)/make/$(cc_id).mk
include $(fate)/make/fate.mk
ifeq ($(os),android)
include $(fate)/make/game_android.mk
else
include $(fate)/make/game.mk
endif

game: $(game)
