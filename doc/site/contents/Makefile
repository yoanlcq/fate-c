

    




    




    







# This Makefile was generated automagically via M4.
# See 'content_list.m4'.

.PHONY: all clean re mrproper

all:  ./intro/index_en.html ./intro/index_fr.html ./devenv/index_en.html ./devenv/index_fr.html ./devenv/cloning/index_en.html ./devenv/cloning/index_fr.html

 
./intro/index_en.html:  \
                      ./intro/properties.m4 \
                      ./devenv/properties.m4 \
                      ./intro/index.m4 \
                      ./intro/content_en.html
	m4 --include=./intro -Dlc=en \
        -Dget_prev="" \
        -Dget_next=./devenv \
        -Dget_path=./intro -Dget_root=../.. \
        ./intro/index.m4 > ./intro/index_en.html
 
./intro/index_fr.html:  \
                      ./intro/properties.m4 \
                      ./devenv/properties.m4 \
                      ./intro/index.m4 \
                      ./intro/content_fr.html
	m4 --include=./intro -Dlc=fr \
        -Dget_prev="" \
        -Dget_next=./devenv \
        -Dget_path=./intro -Dget_root=../.. \
        ./intro/index.m4 > ./intro/index_fr.html
 
./devenv/index_en.html: ./intro/properties.m4 \
                      ./devenv/properties.m4 \
                      ./devenv/cloning/properties.m4 \
                      ./devenv/index.m4 \
                      ./devenv/content_en.html
	m4 --include=./devenv -Dlc=en \
        -Dget_prev=./intro \
        -Dget_next=./devenv/cloning \
        -Dget_path=./devenv -Dget_root=../.. \
        ./devenv/index.m4 > ./devenv/index_en.html
 
./devenv/index_fr.html: ./intro/properties.m4 \
                      ./devenv/properties.m4 \
                      ./devenv/cloning/properties.m4 \
                      ./devenv/index.m4 \
                      ./devenv/content_fr.html
	m4 --include=./devenv -Dlc=fr \
        -Dget_prev=./intro \
        -Dget_next=./devenv/cloning \
        -Dget_path=./devenv -Dget_root=../.. \
        ./devenv/index.m4 > ./devenv/index_fr.html
 
./devenv/cloning/index_en.html: ./devenv/properties.m4 \
                      ./devenv/cloning/properties.m4 \
                       \
                      ./devenv/cloning/index.m4 \
                      ./devenv/cloning/content_en.html
	m4 --include=./devenv/cloning -Dlc=en \
        -Dget_prev=./devenv \
        -Dget_next="" \
        -Dget_path=./devenv/cloning -Dget_root=../../.. \
        ./devenv/cloning/index.m4 > ./devenv/cloning/index_en.html
 
./devenv/cloning/index_fr.html: ./devenv/properties.m4 \
                      ./devenv/cloning/properties.m4 \
                       \
                      ./devenv/cloning/index.m4 \
                      ./devenv/cloning/content_fr.html
	m4 --include=./devenv/cloning -Dlc=fr \
        -Dget_prev=./devenv \
        -Dget_next="" \
        -Dget_path=./devenv/cloning -Dget_root=../../.. \
        ./devenv/cloning/index.m4 > ./devenv/cloning/index_fr.html


clean:
	rm -f  ./intro/index_en.html ./intro/index_fr.html ./devenv/index_en.html ./devenv/index_fr.html ./devenv/cloning/index_en.html ./devenv/cloning/index_fr.html

re: clean all

mrproper: re

