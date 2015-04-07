# Irix Makefile =============================================================

# the OpenGL library includes (+glut + pui)
GL_FLAGS= ../lib/libpui.a ../lib/libglut.a -lGLU -lGL -lXmu -lXext -lX11 -lm -lC

# the name of the output executable
EXE_NAME= program

# these are the files that will be compiled to make the final project.
# At the moment, this uses all cpp, c and cxx files found in the 
# directory.
FILES= *.cpp

# this directory is the one that has the includes for pui in it
INCLUDE_DIR= ../includes/.

# the name of the compiler to use
COMPILER= CC

# create a macro to represent the project
$(EXE_NAME): $(EXE_NAME)

# link the objects in order to create the executable
$(EXE_NAME):
	$(COMPILER) -I $(INCLUDE_DIR) -o $(EXE_NAME) $(FILES) $(GL_FLAGS) 

	
clean:
	rm *.o $(EXE_NAME)
