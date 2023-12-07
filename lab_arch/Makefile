.PHONY: all, run, clean

run: all
        ./archiver arch -i main.c;
        ./archiver arch -s;
        ./archiver arch -e main.c;
        ./archiver arch -s
clean:
        rm ./archiver   
all:
        gcc main.c -o archiver
