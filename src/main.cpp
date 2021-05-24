#include <iostream>
#include "GameController.hpp"

int main() {
    GameController* gc = new GameController();
    gc->startGame();
	delete gc;
    return 0;
}
