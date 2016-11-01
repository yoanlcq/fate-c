.PHONY: all game game_res
all: game_res game

fate:=$(subst \\,/,$(FE_PATH))
include $(fate)/make/rglob.mk
include $(fate)/make/newline.mk
include fatefile.mk
ifneq ($(os),android)
include $(fate)/make/cc_id.mk
include $(fate)/make/os.mk
include $(fate)/make/arch.mk
include $(fate)/make/host.mk
endif
include $(fate)/make/installed_toolchains.mk
include $(fate)/make/def_tools.mk
include $(fate)/make/os_utils.mk
include $(fate)/make/cc_utils.mk
include $(fate)/make/shell_fix.mk
include $(fate)/make/windres.mk
include $(fate)/make/see_cmd.mk
include $(fate)/make/clean.mk
ifeq ($(os),android)
include $(fate)/make/game_android.mk
include $(fate)/make/read_cflags.mk
else
include $(fate)/make/$(cc_id).mk
include $(fate)/make/read_cflags.mk
include $(fate)/make/fate.mk
include $(fate)/make/game.mk
endif

game: $(game)
game_res: $(game_res)
