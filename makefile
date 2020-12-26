COMPILER=g++

clean:
	rm -rf bin/*
	rm -rf include/*.gch

board:
	@$(COMPILER) -std=c++11 -c -g src/board.cpp -Iinclude/ -o bin/board.o
	@$(COMPILER) -std=c++11 -c -g src/player.cpp -Iinclude/ -o bin/player.o
	@$(COMPILER) -std=c++11 -c -g src/randomAI.cpp -Iinclude/ -o bin/randomAI.o
	@$(COMPILER) -std=c++11 -c -g src/game.cpp -Iinclude/ -o bin/game.o
	@$(COMPILER) -std=c++11 -c -g src/main.cpp -Iinclude/ -o bin/main.o
	@echo "Built .o's"
#
# main: board
# 	@$(COMPILER) -std=c++11 -c src/main.cpp -Iheaders/ -o bin/main.o -lsfml-graphics -lsfml-window -lsfml-system
# 	@echo "Built main.o"

all: clean board
	@$(COMPILER) -std=c++11 bin/board.o bin/player.o bin/randomAI.o bin/game.o bin/main.o -o azul
	@echo "Linked all into executable"
