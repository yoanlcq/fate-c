res:=$(dirs_res)
art:=$(dirs_art)

$(res)/%.jpg: $(art)/%.jpg
	$(call mkdir_p,$(@D))
	$(call copyfiles,$<,$@)

$(res)/%.ogg: $(art)/%.ogg
	$(call mkdir_p,$(@D))
	$(call copyfiles,$<,$@)

game_art := \
	$(call rglob,$(art),*.jpg) \
	$(call rglob,$(art),*.ogg)

game_res := $(patsubst $(art)%,$(res)%,$(game_art))

$(res)/android/%.pkm: $(art)/%.tex.png
	$(call mkdir_p,$(@D))
	$(etc1tool) $< -o $@

$(res)/common/%.png: $(art)/%.pix.png
	$(call mkdir_p,$(@D))
	$(call copyfiles) $< $@

$(res)/desktop/%.dds: $(art)/%.tex.png
	$(call mkdir_p,$(@D))
	$(dxttool) $< $@

$(res)/android/common:
	$(call ln_sf,$(res)/common,$@)
$(res)/emscripten/common:
	$(call ln_sf,$(res)/common,$@)

