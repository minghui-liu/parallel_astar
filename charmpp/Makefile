
CHARMC = charmc

default: all
all: astar

astar : main.o worker.o
	$(CHARMC) -language charm++ -o astar main.o worker.o

main.o : main.C main.h main.decl.h main.def.h worker.decl.h
	$(CHARMC) -o main.o main.C

main.decl.h main.def.h : main.ci
	$(CHARMC) main.ci

worker.o : worker.C worker.h worker.decl.h worker.def.h main.decl.h
	$(CHARMC) -o worker.o worker.C

worker.decl.h worker.def.h : worker.ci
	$(CHARMC) worker.ci


clean:
	rm -f main.decl.h main.def.h main.o
	rm -f worker.decl.h worker.def.h worker.o
	rm -f astar charmrun
