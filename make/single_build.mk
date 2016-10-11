.PHONY: all game
all: game

fate:=$(FE_PATH)
include fatefile.mk
include $(fate)/make/cc_id.mk
include $(fate)/make/os.mk
include $(fate)/make/arch.mk
include $(fate)/make/host.mk
include $(fate)/make/installed_toolchains.mk
include $(fate)/make/def_tools.mk
include $(fate)/make/os_utils.mk
include $(fate)/make/cc_utils.mk
include $(fate)/make/shell_fix.mk
# include $(fate)/make/toolchains.mk
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
