.PHONY: default

UNIVERSAL_FLAGS:=-Wimplicit-int-conversion -I layers -fsanitize=address

default:
	cc -g3 $(UNIVERSAL_FLAGS) apps/scratch/main.c -o build/main && build/main
