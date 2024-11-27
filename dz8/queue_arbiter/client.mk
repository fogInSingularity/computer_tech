CFLAGS = -Waggressive-loop-optimizations -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconversion -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wopenmp-simd -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wswitch-default -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wvariadic-macros -Wno-missing-field-initializers -Wno-narrowing -Wno-varargs -Wstack-usage=8192 -Wstack-protector
CC = gcc
ASAN_FLAGS = -fsanitize=address,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr
DEBUG_FLAGS = -Og -g3 -DDEBUG -ggdb -DFORTIFY_SOURCES=3
# RELEASE_FLAGS = -O2 -march=native
OPT_FLAGS = $(DEBUG_FLAGS) $(ASAN_FLAGS)
# OPT_FLAGS = $(RELEASE_FLAGS)
#-D_FORTIFY_SOURCE=3

LINK_FLAGS = -lpthread -lm

EXEC = client
SOURCE = main_client.c log.c

all:
	@$(CC) $(CFLAGS) $(SOURCE) $(OPT_FLAGS) -o $(EXEC) $(LINK_FLAGS)
