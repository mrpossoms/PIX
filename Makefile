NAME=PIX
EDITOR=
FLAGS=-Wint-to-pointer-cast
SRC= main.c ./src/*.c ./include/GL/*.c
INC=-I./include -I /usr/local/include
DST=./lib
LINK= -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo
LIB = -L /usr/local/lib/

TSTINC=./tests/include
TST=./tests
TSTS=./tests/*.c

all:
ifdef NAME
ifdef IS_LIBRARY
		gcc $(INC) -c $(SRC)
		ar rcs $(DST)/$(LIB) *.o
		rm *.o
else
		gcc $(INC) $(LIB) -o $(NAME) $(SRC) $(LINK)
endif
else
	@echo "Project not setup! Please run 'make setup' first."
endif

testlib: $(SRC)
ifdef NAME
	gcc -I$(INC) -I$(TSTINC) -c $(SRC)
	mkdir -p $(TST)/lib
	ar rcs $(TST)/lib/$(LIB) *.o
	rm *.o
else
	@echo "Project not setup! Please run 'make setup' first."
endif

tests: testlib
ifndef NAME
	find ./$(TST) -name "*.c" -exec gcc {} -o $(TST)/bin/{} -l$(NAME)
else
	@echo "Project not setup! Please run 'make setup' first."
endif

package:
ifdef NAME
	make
	mkdir -p $(DST)/include
	cat /dev/null >| $(DST)/include/$(NAME).h
	find ./$(INC) -name "*.h" -exec cat {} > $(DST)/include/$(NAME).h \;
else
	@echo "Project not setup! Please run 'make setup' first."
endif	

inc:
ifdef NAME
	@read -p "Enter header path (with .h): " file;\
	mkdir -p $(INC)/"$$file" ;\
	rmdir $(INC)/"$$file" ;\
	touch $(INC)/"$$file" ;\
	tmp=$${file////_};\
	def=$${tmp/.h/ };\
	cat /dev/null >| $(INC)/"$$file" ;\
	echo "#ifndef __$(NAME)_$$def" >> $(INC)/"$$file";\
	echo "#define __$(NAME)_$$def" >> $(INC)/"$$file";\
	echo "\n\n" >> $(INC)/"$$file";\
	echo "#endif" >> $(INC)/"$$file";\
	if [ ! -z  "$(EDITOR)" ]; then \
		$(EDITOR) $(INC)/"$$file";\
	fi
else
	@echo "Project not setup! Please run 'make setup' first."
endif	

install:
	echo -n "Installing the library"
	cp $(DST)/include/*.h /usr/include
	cp $(DST)/*.a /usr/lib
clean:
	find ./ -iname "*.a" -exec rm {} \;
	find ./ -iname "*.o" -exec rm {} \;
	find ./ -iname "*.out" -exec rm {} \;
	find ./ -iname "*.bin" -exec rm {} \;

setup:
ifndef NAME
	@echo "Setting up project"
	@mkdir -p $(TST) $(DST) $(INC) ./src
	@mkdir -p $(TSTINC) $(TST)/bin
	@read -p "Enter project name: " name;\
	read -p "Text editor path: " editor;\
	read -p "Is project a library? (y/n): " isLibrary;\
	exec 3<> Makefile && awk -v TEXT="NAME=$$name\nEDITOR=$$editor" 'BEGIN {print TEXT}{print}' Makefile >&3;\
	if [ $isLibrary == "y" ]; then \
		exec 3<> Makefile && awk -v TEXT="IS_LIBRARY" 'BEGIN {print TEXT}{print}' Makefile >&3; \
	fi
endif
