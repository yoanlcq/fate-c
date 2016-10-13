see_obj_cmd  :=$(if $(show_obj_cmd),,@)
see_exe_cmd  :=$(if $(show_exe_cmd),,@)
see_misc_cmd :=$(if $(show_misc_cmd),,@)
see_build_cmd:=$(if $(show_build_cmd),,@)

ifdef show_all_cmd
see_obj_cmd:=
see_exe_cmd:=
see_misc_cmd:=
see_build_cmd:=
endif

