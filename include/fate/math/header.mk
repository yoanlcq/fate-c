.PHONY: all clean mrproper re

all:      uvec4.h      uvec3.h      uvec2.h \
	      ivec4.h      ivec3.h      ivec2.h \
           vec4.h       vec3.h       vec2.h \
          dvec4.h      dvec3.h      dvec2.h \
		  wvec4.h      wvec3.h      wvec2.h \
	    ullvec4.h    ullvec3.h    ullvec2.h

clean: 
	rm -f *vec?.h
mrproper: clean all
re: mrproper

