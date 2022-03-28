COMPILER=g++-9

clean:
	rm -rf bin/*
	rm -rf include/*.gch
	rm -rf *.o

board:
	@$(COMPILER) -std=c++2a -c -g src/Button.cpp -Iinclude/ -o bin/button.o
	@$(COMPILER) -std=c++2a -c -g src/Bag.cpp -Iinclude/ -o bin/bag.o
	@$(COMPILER) -std=c++2a -c -g src/Sounds.cpp -Iinclude/ -o bin/sounds.o
	@$(COMPILER) -std=c++2a -c -g src/Factory.cpp -Iinclude/ -o bin/factory.o
	@$(COMPILER) -std=c++2a -c -g src/Location.cpp -Iinclude/ -o bin/location.o
	@$(COMPILER) -std=c++2a -c -g src/RoundVisualizer.cpp -Iinclude/ -o bin/RoundVisualizer.o
	@$(COMPILER) -std=c++2a -c -g src/Board.cpp -Iinclude/ -o bin/board.o
	@$(COMPILER) -std=c++2a -c -g src/Player.cpp -Iinclude/ -o bin/player.o
	@$(COMPILER) -std=c++2a -c -g src/PlayerInfo.cpp -Iinclude/ -o bin/playerinfo.o
	@$(COMPILER) -std=c++2a -c -g src/RandomAI.cpp -Iinclude/ -o bin/randomAI.o
	@$(COMPILER) -std=c++2a -c -g src/ColourTargetAI.cpp -Iinclude/ -o bin/colourTargetAI.o
	@$(COMPILER) -std=c++2a -c -g src/Human.cpp -Iinclude/ -o bin/Human.o
	@$(COMPILER) -std=c++2a -c -g src/PlayerVisualizer.cpp -Iinclude/ -o bin/playervisualizer.o -lsfml-system
	@$(COMPILER) -std=c++2a -c -g src/Game.cpp -Iinclude/ -o bin/game.o
	@$(COMPILER) -std=c++2a -c -g src/MainMenu.cpp -Iinclude/ -o bin/mainmenu.o
	@$(COMPILER) -std=c++2a -c -g main.cpp -Iinclude/ -o bin/main.o -lsfml-graphics -lsfml-window -lsfml-system -lX11
	@echo "Built .o's"
#
# main: board
# 	@$(COMPILER) -std=c++11 -c src/main.cpp -Iheaders/ -o bin/main.o -lsfml-graphics -lsfml-window -lsfml-system
# 	@echo "Built main.o"

all: clean board
	@$(COMPILER) -std=c++2a -o azul bin/button.o bin/bag.o bin/sounds.o bin/factory.o bin/location.o bin/RoundVisualizer.o bin/board.o bin/player.o bin/playerinfo.o bin/randomAI.o bin/colourTargetAI.o bin/Human.o bin/playervisualizer.o bin/game.o bin/mainmenu.o bin/main.o -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system -pthread -lX11
	@echo "Linked all into executable"
