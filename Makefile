all: 
	cd src ; cmake . ; make clean ; make ; cp FALCON .. ; cd ..
clean:
	cd src ; make clean ; rm -rf *.o ; rm -rf FALCON ; cd ..

