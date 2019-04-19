DEBUG_FLAG := -g -Og
OPTIMIZATE := -O2
LIBS := -Iinclude -Llib -lALF_std -lpthread
# W_FLAGS := -Wall -Wextra -Wpedantic -Wshadow -Wfatal-errors
W_FLAGS := -Wall -Wextra -Wpedantic -Wfatal-errors

all: debug

release:
	gcc src/*.c -o shs.out $(LIBS) $(W_FLAGS) $(OPTIMIZATE) -std=gnu11 -DNDEBUG

debug:
	gcc src/*.c -o shs.out $(LIBS) $(W_FLAGS) $(DEBUG_FLAG) -std=gnu11
