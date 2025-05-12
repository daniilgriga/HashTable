CC = gcc

FLAGS = -O3 -g -msse4.2 -mavx2 -DNDEBUG #-ggdb3 -Wall -Wextra  -Waggressive-loop-optimizations         \
	   	-Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts             				  \
		-Wconversion -Wempty-body -Wfloat-equal										  				  \
		-Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline 				  \
		-Wlogical-op -Wopenmp-simd -Wpacked                                           				  \
		-Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion      				  \
		-Wstrict-overflow=2 -Wsuggest-attribute=noreturn                              				  \
		-Wsuggest-final-methods -Wsuggest-final-types -Wswitch-default                				  \
		-Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused                 				  \
		-Wvariadic-macros -Wno-missing-field-initializers -Wno-narrowing              				  \
		-Wno-varargs -Wstack-protector                                                				  \
		-fstack-protector -fstrict-overflow -flto -fno-omit-frame-pointer             				  \
		-pie -fPIE -Werror=vla                                                        				  \

CFLAGS = -c $(FLAGS)
LDFLAGS = $(FLAGS) -lm

SOURCES_LIST = main.c list.c tools.c hashtbl.c file.c

SOURCES = $(SOURCES_LIST:%=src/%)
OBJECTS = $(SOURCES_LIST:%.c=build/%.o)
DEPS = $(OBJECTS:%.o=%.d)
EXECUTABLE = build/test

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	@$(CC) $(LDFLAGS) $^ build/crc32.o -o $@

-include $(DEPS)

build/%.o: src/%.c
	@$(CC) -I./include $(CFLAGS) -MMD -MP $< -o $@

clean:
	rm -f build/*.o $(EXECUTABLE)

callgrind:
	valgrind --dump-instr=yes --collect-jumps=yes --tool=callgrind --callgrind-out-file=test/callgrind/callgrind.out.$(prog) ./$(EXECUTABLE)
	kcachegrind test/callgrind/callgrind.out.$(prog)

perf:
	perf record -o test/perf/$(prog).data -e instructions,cycles,cache-misses,branches,branch-misses ./$(EXECUTABLE)
	hotspot ./test/perf/$(prog).data

nasm:
	nasm -f elf64 src/crc32.nasm -o build/crc32.o

