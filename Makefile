FLAGS = -std=c++11

all: objput objget objlist objsetacl objgetacl

objput:
	g++ -o objput objput.cpp

objget:
	g++ -o objget objget.cpp

objlist:
	g++ -o objlist objlist.cpp

objsetacl:
	g++ -o objsetacl objsetacl.cpp $(FLAGS)

objgetacl:
	g++ -o objgetacl objgetacl.cpp $(FLAGS)

clean:
	rm -f *.o objput objget objlist objsetacl objgetacl