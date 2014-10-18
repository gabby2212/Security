FLAGS = -std=c++0x
PHONY: exec
 
ifneq "$(strip $(userfile))" ""
exec: build
	./setUp.sh $(userfile)
else
exec: build
	./setUp.sh
endif

build: all

test: build
	@read -r -p "Are you sure? This will erase your current file system " response; \
	[ $$response = "y" ] || [ $$response = "Y" ] || (echo "Exiting."; exit 1;)
	rm -f ./fileSystem/u1.* ./fileSystem/u2.*
	> ./config/aclFile
	./testFiles/objput-tests.sh
	./testFiles/objget-tests.sh
	./testFiles/objlist-tests.sh
	./testFiles/objsetacl-tests.sh
	./testFiles/objgetacl-tests.sh
	./testFiles/objtestacl-tests.sh

all: objput objget objlist objsetacl objgetacl objtestacl

objput:
	g++ -o objput ./source/objput.cpp $(FLAGS)

objget:
	g++ -o objget ./source/objget.cpp $(FLAGS)

objlist:
	g++ -o objlist ./source/objlist.cpp $(FLAGS)

objsetacl:
	g++ -o objsetacl ./source/objsetacl.cpp $(FLAGS)

objgetacl:
	g++ -o objgetacl ./source/objgetacl.cpp $(FLAGS)

objtestacl:
	g++ -o objtestacl ./source/objtestacl.cpp $(FLAGS)

clean:
	rm -fr *.o config fileSystem objput objget objlist objsetacl objgetacl objtestacl

emptyFs:
	rm -fr fileSystem config
	./setUp.sh