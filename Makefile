# CS4102 CG Practical 4 [-] Shading and Texture

# https://sites.google.com/site/michaelsafyan/software-engineering/how-to-write-a-makefile
program_NAME := shadingAndTexture
program_C_SRCS := $(wildcard src/*.c)
program_CXX_SRCS := $(wildcard src/*.cpp)
# make sure external libraries are built
OBJ_LOADER_CXX_SRCS += $(wildcard Resources/objLoader/*.cpp)
program_C_OBJS := ${program_C_SRCS:.c=.o}
program_CXX_OBJS := ${program_CXX_SRCS:.cpp=.o}
OBJ_LOADER_CXX_OBJS := ${OBJ_LOADER_CXX_SRCS:.cpp=.o}
program_OBJS := $(program_C_OBJS) $(program_CXX_OBJS)
solution_OBJS := $(program_OBJS) $(OBJ_LOADER_CXX_OBJS)
program_INCLUDE_DIRS := Resources/objLoader/
program_INCLUDE_DIRS += Resources/CImg/
program_LIBRARY_DIRS := 
program_LIBRARIES := m

CPPFLAGS += $(foreach includedir,$(program_INCLUDE_DIRS),-I$(includedir))
LDFLAGS += $(foreach librarydir,$(program_LIBRARY_DIRS),-L$(librarydir))
LDFLAGS += $(foreach library,$(program_LIBRARIES),-l$(library))
CXXFLAGS = -Wall -ggdb

UNAME := $(shell uname)

# GL Flags
ifeq ($(UNAME), Darwin)
	GLFLAGS = -framework OpenGL -framework GLUT        
endif
ifeq ($(UNAME), Linux)
	GLFLAGS = -lGL -lGLU -lglut
endif

# Flags needed for CImg
ifeq ($(UNAME), Darwin)
	CImgFLAGS = -lpthread -I/usr/X11R6/include -L/usr/X11R6/lib -lm -lpthread -lX11        
endif
ifeq ($(UNAME), Linux)
	CImgFLAGS = -L/usr/X11R6/lib -lm -lpthread -lX11
endif

.PHONY: all clean distclean

all: $(program_NAME)

release: CXXFLAGS += -O2
release: $(program_NAME)

$(program_NAME): $(solution_OBJS)
	$(LINK.cc) $(solution_OBJS) $(GLFLAGS) $(CImgFLAGS) -o $(program_NAME)

clean:
	@- $(RM) $(program_NAME)
	@- $(RM) $(program_OBJS)

# Cleans all object files, even from external libraries
distclean: clean
	@- $(RM) $(solution_OBJS)