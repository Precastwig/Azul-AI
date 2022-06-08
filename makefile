COMPILER=g++-9

clean:
	rm -rf bin/*
	rm -rf include/*.gch
	rm -rf *.o

all: clean
	@$(COMPILER) -std=c++2a -o azul -I ./include *.cpp ./src/*.cpp -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system -pthread -lX11
	@echo "Linked all into executable"
