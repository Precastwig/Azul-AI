COMPILER=g++

clean:
	rm -rf bin/*
	rm -rf include/*.gch

board:
	@$(COMPILER) -std=c++11 -c -g src/Factory.cpp -Iinclude/ -o bin/factory.o
	@$(COMPILER) -std=c++11 -c -g src/Board.cpp -Iinclude/ -o bin/board.o
	@$(COMPILER) -std=c++11 -c -g src/Player.cpp -Iinclude/ -o bin/player.o
	@$(COMPILER) -std=c++11 -c -g src/RandomAI.cpp -Iinclude/ -o bin/randomAI.o
	@$(COMPILER) -std=c++11 -c -g src/HumanCommandLine.cpp -Iinclude/ -o bin/HumanCommandLine.o
	@$(COMPILER) -std=c++11 -c -g src/Game.cpp -Iinclude/ -o bin/game.o
	@$(COMPILER) -std=c++11 -c -g src/Main.cpp -Iinclude/ -o bin/main.o -lsfml-graphics -lsfml-window -lsfml-system
	@echo "Built .o's"
#
# main: board
# 	@$(COMPILER) -std=c++11 -c src/main.cpp -Iheaders/ -o bin/main.o -lsfml-graphics -lsfml-window -lsfml-system
# 	@echo "Built main.o"

all: clean board
	@$(COMPILER) -std=c++11 bin/factory.o bin/board.o bin/player.o bin/randomAI.o bin/game.o bin/main.o -o azul -lsfml-graphics -lsfml-window -lsfml-system
	@echo "Linked all into executable"
