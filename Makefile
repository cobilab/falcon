#
# FALCON INSTALL
#
# make all to install all
all: 
	cd src ; cmake . ; make clean ; make ; cp FALCON .. ; cd ..
# make clean to remove
clean:
	cd src ; make clean ; rm -rf *.o ; rm -rf FALCON ; cd ..

