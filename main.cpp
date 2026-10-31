#include "Game.h"
#include "Player.h"
#include <iostream>
#include <limits>
using namespace std;

// The game is a square shape


void setupGame() {
    srand(time(NULL)); // Initialize random seed
    int mode;
    string team1, team2;

    vector<Player> players;

    // Display the story intro
    Game game(10, 10, {}); // Temporary Game object to display the intro
    game.displayStoryIntro();

    // Mode selection with input validation
    while (true) {
        cout << "Select Mode: 1 for Player vs Player, 2 for Player vs AI: ";
        cin >> mode;

        if (cin.fail() || (mode != 1 && mode != 2)) {
            cin.clear(); // Clear the error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
            cout << "Invalid input. Please enter 1 or 2.\n";
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the newline character
            break; // Valid input, exit the loop
        }
    }

    // Team name input
    cout << "Team name option (Coalition of Shadows, Alliance of Light): ";
    cout << "Enter Team Name for Player 1: ";
    getline(cin, team1);

    if (mode == 1) {
        cout << "Team name option (Coalition of Shadows, Alliance of Light): ";
        cout << "Enter Team Name for Player 2: ";
        getline(cin, team2);
        players.push_back(Player("Player 1", team1));
        players.push_back(Player("Player 2", team2));
    } else {
        players.push_back(Player("Player 1", team1));
        players.push_back(Player("AI","Coalition of Shadows", true));
    }

    // Display faction info for each player
    for (auto &player : players) {
        game.displayFactionInfo(player);
    }

    // Start the game with the players
    Game newGame(10, 10, players);
    newGame.startGame();
}

bool Game::playerWins() const {
    for (const auto &player : players) {
        if (player.getResources() >= 100) {
            std::cout << player.getName() << " (" << player.getTeamName() << ") wins the game!" << std::endl;
            return true;
        }
    }
    return false;
}

int main() {
    setupGame();
    return 0;
}