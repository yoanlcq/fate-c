see_obj_cmd :=$(if see_obj_cmd,,@)
see_exe_cmd :=$(if see_exe_cmd,,@)
see_misc_cmd:=$(if see_misc_cmd,,@)

ifdef see_all_cmd
see_obj_cmd:=
see_exe_cmd:=
see_misc_cmd:=
endif

