all:
	g++ -o temple_renderer main.cpp -I"./include" -L"./lib" -lsfml-graphics -lsfml-window -lsfml-system
