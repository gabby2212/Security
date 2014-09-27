all: objput objget objlist

objput:
	g++ -o objput objput.cpp

objget:
	g++ -o objget objget.cpp

objlist:
	g++ -o objlist objlist.cpp

clean:
	rm -f *.o objput objget objlist