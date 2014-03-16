CPP  = g++
CC   = gcc

OBJ  = main.o $(RES)
LINKOBJ  = main.o $(RES)

BIN  = izi
CXXFLAGS =  $(INCS)    -O3
INCS =  -I"./algorithms"  -I"./data/file"  -I"./data/memory"  -I"./dataStructures/trie"  -I"./problems/frequent"  -I"./problems/key" -I"./problems/essential" -I"./problems/DI" -I"./problems/notRedundant" -I"./util"  -I"./problems"

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o $@


main.o: main.cpp
	$(CPP) -c main.cpp -o main.o $(CXXFLAGS)


