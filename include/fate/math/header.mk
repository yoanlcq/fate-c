.PHONY: all clean mrproper re

all:  u32v2.h u32v3.h u32v4.h \
	  i32v2.h i32v3.h i32v4.h \
      f32v2.h f32v3.h f32v4.h \
      f64v2.h f64v3.h f64v4.h \
	    wv2.h   wv3.h   wv4.h \
	  u64v2.h u64v3.h u64v4.h

clean: 
	rm -f *v?.h
mrproper: clean all
re: mrproper

