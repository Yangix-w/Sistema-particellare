# $@ nome del target
# @^ lista delle dipendenze

CC = g++
CCFLAGS = -O3 -s -DNDEBUG

ifeq ($(OS),Windows_NT)
	BASEDIR = ../base
	INCLUDEDIRS += -I$(BASEDIR)/freeglut/include
	INCLUDEDIRS += -I$(BASEDIR)/glew/include
	INCLUDEDIRS += -I$(BASEDIR)/glm
	INCLUDEDIRS += -I$(BASEDIR)/assimp

	LIBDIRS += -L$(BASEDIR)/freeglut/lib
	LIBDIRS += -L$(BASEDIR)/glew/lib/Release/Win32
	LIBDIRS += -L$(BASEDIR)/assimp/lib

	LIBS += -lfreeglut 
	LIBS += -lopengl32 
	LIBS += -lglew32
	LIBS += -lassimp

else
	LIBS += -lglut 
	LIBS += -lGLEW
	LIBS += -lGL
	LIBS += -lassimp
endif

OBJS = main.o utilities.o common.o transform.o camera.o shaderclass.o particleShader.o
OBJS += texture2D.o particle.o particleSystem.o 

main.exe : $(OBJS)
	$(CC) $(CCFLAGS) $^ $(LIBDIRS) $(LIBS) -o $@

main.o : main.cpp
	$(CC) -c $(CCFLAGS) $(INCLUDEDIRS) $? -o $@

utilities.o : utilities.cpp
	$(CC) -c $(CCFLAGS) $(INCLUDEDIRS) $? -o $@

common.o : common.cpp
	$(CC) -c $(CCFLAGS) $(INCLUDEDIRS) $? -o $@

transform.o : transform.cpp
	$(CC) -c $(CCFLAGS) $(INCLUDEDIRS) $? -o $@

camera.o : camera.cpp
	$(CC) -c $(CCFLAGS) $(INCLUDEDIRS) $? -o $@

shaderclass.o : shaderclass.cpp
	$(CC) -c $(CCFLAGS) $(INCLUDEDIRS) $? -o $@

particleShader.o : particleShader.cpp
	$(CC) -c $(CCFLAGS) $(INCLUDEDIRS) $? -o $@

texture2D.o : texture2D.cpp
	$(CC) -c $(CCFLAGS) $(INCLUDEDIRS) $? -o $@

particle.o : particle.cpp
	$(CC) -c $(CCFLAGS) $(INCLUDEDIRS) $? -o $@

particleSystem.o : particleSystem.cpp
	$(CC) -c $(CCFLAGS) $(INCLUDEDIRS) $? -o $@


.PHONY clean:
clean:
	rm *.o *.exe


