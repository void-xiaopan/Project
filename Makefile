
MAIN ?= staff_management_main.c
FUNC ?= staff_management_func.c
INCLUDE ?= staff_management_include.h
NAME ?= client

$(NAME):$(MAIN) $(FUNC) $(INCLUDE)
	@gcc -g -O1 $^ -o $@ -pthread -Wall
clean:$(NAME)
	@rm -rf $<
	@clear
	@pwd
