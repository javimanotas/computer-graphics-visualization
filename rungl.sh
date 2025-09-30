set -e
gcc -Wall $1.c -lGL -lGLU -lglut
./a.out
rm -fr a.out
