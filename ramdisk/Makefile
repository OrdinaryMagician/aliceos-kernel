CFLAGS := -std=c99 -O2 -pipe -Wall -Wextra -Werror -pedantic -funsigned-char
SOURCES := $(patsubst %.c,%,$(wildcard *.c))
.PHONY: clean
all: $(SOURCES)
.c:
	@echo -e "\e[1;32m[CLANG]\e[0m `basename ${<}` -> `basename ${@}`"
	@clang $(CFLAGS) -o bin/$@ $<
clean:
	@$(foreach F,$(SOURCES),\
		if [[ -e bin/$F ]]; then\
		  echo -e "\e[1;32m[RM]\e[0m `basename ${F}`";\
		  rm -f bin/$F;\
		fi;)
