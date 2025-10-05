set -e
gcc -Wall $1.c -lGL -lGLU -lglut -lm
./a.out
rm -fr a.out
