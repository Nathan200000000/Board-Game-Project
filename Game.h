#ifndef GAME_H
#define GAME_H

#include <string>
#include <iostream>
#include <vector>
#include "Tile.h"
#include "Player.h"
using namespace std;

// Game class
class Game {
private:

    vector<vector<Tile>> board; // 2D vector for the board
    vector<Player> players;
    int currentPlayer;

    void saveGame();
    void loadGame();
    bool playerWins() const;

    // Helper function
    std::string getMoveDirection(Player &player);
    void movePlayer(Player &player, const std::string &direction, int steps);
    Tile& getTileAtPlayerPosition(Player &player);
    void handleTileEffects(Player &player, Tile &tile);
    void handleMarketInteraction(Player &player);
    void handleCheatCode(Player &player);
    void displayPlayerStatuses();
    void displayMenu(Player &player);

public:

    Game(int rows, int cols, vector<Player> playerList); // Initialize with rows and cols
    void playTurn();
    void startGame();
    void displayStoryIntro();
    void displayFactionInfo(Player &player);

    // Function to Initialize the board with different types of tiles
    void InitializeBoard(int rows, int cols);

    // Function to display the game board
    void printBoard();
};

#endif