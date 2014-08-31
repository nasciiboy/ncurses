CXX      = g++
CXXFLAGS = -Wall
EXE      = nitris
OBJS     = nitris.o game.o screen.o

$(EXE): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ -lncurses
%.o: %.c
	$(CXX) -c $^

clean:
	$(RM) $(OBJS) $(EXE) *~