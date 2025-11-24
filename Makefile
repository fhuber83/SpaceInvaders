space_invaders:	main.o
				g++ -o space_invaders main.o `sdl2-config --cflags --libs` -lSDL2_ttf -lSDL2_image

main.o:			main.cpp
				g++ -c main.cpp -o main.o `sdl2-config --cflags`
