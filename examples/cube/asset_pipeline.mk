res:=$(dirs_res)
art:=$(dirs_art)

$(res)/android/%.pkm: $(art)/%.tex.png
	$(call mkdir_p,$(@D))
	$(etc1tool) $< -o $@

$(res)/common/%.png: $(art)/%.pix.png
	$(call mkdir_p,$(@D))
	$(call cp_r) $< $@

$(res)/desktop/%.dds: $(art)/%.tex.png
	$(call mkdir_p,$(@D))
	$(dxttool) $< $@

$(res)/android/common:
	$(call ln_sf,$(res)/common,$@)
$(res)/emscripten/common:
	$(call ln_sf,$(res)/common,$@)

