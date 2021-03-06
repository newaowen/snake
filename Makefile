COMPILER=G++

# todo: object files into output path, processing c / c++ files in the same time (?), nested directories for source files (?)
C = c
#OUTPUT_PATH = out/production/snake/
OUTPUT_PATH = bin/
SOURCE_PATH = src/
EXE = $(OUTPUT_PATH)snake

ifeq ($(COMPILER), G++)
  ifeq ($(OS),Windows_NT)
    OBJ = obj
  else
    OBJ = o
  endif
  COPT = -O2
  CCMD = g++
  OBJFLAG = -o
  EXEFLAG = -o
# INCLUDES = -I../.includes
  INCLUDES = -I/usr/include/SDL/
# LIBS = -lgc
  LIBS =
# LIBPATH = -L../gc/.libs
  LIBPATH = -LSDL -lSDLmain -lSDL_image -lSDL_gfx -lSDL_ttf
  CPPFLAGS = $(COPT) -g $(INCLUDES)
  LDFLAGS = $(LIBPATH) -g $(LIBS)
  DEP = dep
else
  OBJ = obj
  COPT = /O2
  CCMD = cl
  OBJFLAG = /Fo
  EXEFLAG = /Fe
# INCLUDES = /I..\\.includes
  INCLUDES =
# LIBS = ..\\.libs\\libgc.lib
  LIBS =
  CPPFLAGS = $(COPT) /DEBUG $(INCLUDES)
  LDFLAGS = /DEBUG
endif

#OBJ = $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(wildcard *.cpp)))
#remove source path c to source path.o
#and source path.o -> out path.o
#OBJS := $(patsubst %.$(C),%.$(OBJ),$(wildcard $(SOURCE_PATH)*.$(C)))
OBJS := $(subst $(SOURCE_PATH), $(OUTPUT_PATH), $(patsubst %.$(C),%.$(OBJ),$(wildcard $(SOURCE_PATH)*.$(C))))

#compile obj rule, attention middle path is not same 
$(OUTPUT_PATH)/%.$(OBJ): $(SOURCE_PATH)/%.$(C)
	@echo Compiling $(basename $<)...
	$(CCMD) -c $(CPPFLAGS) $(CXXFLAGS) $< $(OBJFLAG) $@

all: $(OBJS)
	@echo Linking $^
	$(CCMD) $(LDFLAGS) $^ $(LIBS) $(EXEFLAG) $(EXE)

clean:
	rm -rf $(SOURCE_PATH)*.$(OBJ) $(EXE)

rebuild: clean all
#rebuild is not entirely correct
