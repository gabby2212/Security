FLAGS = -std=c++0x
FILE := userfile.txt

exec:
	./$(TARGET) $(ARGS) $(FILE)

build: all

test: build
	./objput -u u1 -g g1 test.txt< test.txt userfile.txt
	./objgetacl -u u1 -g g1 test.txt userfile.txt
	./objget -u u1 -g g1 test.txt userfile.txt

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