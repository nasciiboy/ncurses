CXX = g++
CXXFLAGS = -Wall
LIBS = -lncurses
OBJS = utils.o file.o map.o pathfinding.o game.o menu.o editor.o main.o
EXE = test

$(EXE): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $^

clean:
	$(RM) $(OBJS) $(EXE) *~