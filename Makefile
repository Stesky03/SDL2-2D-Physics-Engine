OBJS = engine.cpp
DEPS = engine.h
TARGET = engine
LIBS = -lSDL2 -lSDL2_ttf

all : $(OBJS)
	g++ $(OBJS) $(DEPS) -w $(LIBS) -o $(TARGET)