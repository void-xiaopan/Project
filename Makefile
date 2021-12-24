
MAIN ?= staff_management_main.c
FUNC ?= staff_management_func.c
INCLUDE ?= staff_management_include.h
NAME ?= server

$(NAME):$(MAIN) $(FUNC) $(INCLUDE)
	@gcc -g -O1 $^ -o $@ -Wall -lsqlite3
clean:$(NAME)
	@rm -rf $<
	@clear
	@pwd
