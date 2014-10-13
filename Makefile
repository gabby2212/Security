FLAGS = -std=c++0x
FILE := userfile.txt

exec:
	./$(TARGET) $(ARGS) $(FILE)

build: all

test: build
	@read -r -p "Are you sure? This will erase your current file system " response; \
	[ $$response = "y" ] || [ $$response = "Y" ] || (echo "Exiting."; exit 1;)
	rm -f u1.* u2.*
	> aclFile
	./testFiles/objput-tests.sh
	./testFiles/objget-tests.sh
	./testFiles/objlist-tests.sh
	./testFiles/objsetacl-tests.sh
	./testFiles/objgetacl-tests.sh
	./testFiles/objtestacl-tests.sh

all: objput objget objlist objsetacl objgetacl objtestacl

objput:
	g++ -o objput objput.cpp $(FLAGS)

objget:
	g++ -o objget objget.cpp $(FLAGS)

objlist:
	g++ -o objlist objlist.cpp $(FLAGS)

objsetacl:
	g++ -o objsetacl objsetacl.cpp $(FLAGS)

objgetacl:
	g++ -o objgetacl objgetacl.cpp $(FLAGS)

objtestacl:
	g++ -o objtestacl objtestacl.cpp $(FLAGS)

clean:
	rm -f *.o objput objget objlist objsetacl objgetacl objtestacl