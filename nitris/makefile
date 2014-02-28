CXX      = g++
CXXFLAGS = -Wall
EXE      = nitris
OBJS     = nitris.c game.c screen.c

$(EXE):
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ -lncurses
clean:
	$(RM) $(EXE)