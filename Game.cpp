#include "Tile.h"
#include "Game.h"
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <iostream>


Game::Game(int rows, int cols, std::vector<Player> playerList) : players(playerList), currentPlayer(0) {
    board.resize(rows, std::vector<Tile>(cols)); // Initialize a 2D vector for the board


    // Initialize board with different tile Types
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if ((i + j) % 5 == 0) 
                board[i][j] = Tile{ TileType::RESOURCE, "Resource Node" };
            else if ((i + j) % 7 == 0) 
                board[i][j] = Tile{ TileType::SAFE_ZONE, " Safe Zone"};
            else if ((i + j) % 9 == 0)
                board[i][j] = Tile{ TileType::HAZARD, "Hazard" };
            else if ((i + j) % 11 == 0) 
                board[i][j] = Tile{ TileType::MARKET, "Market" };
            else if ((i + j) % 13 == 0)
                board[i][j] = Tile{ TileType::CHEAT, "Cheat Tile" };
            else 
                board[i][j] = Tile{ TileType::EMPTY, "Empty Land" };
        }
    }
}

void Game::saveGame() {
    std::ofstream outFile("savegame.txt");
    if (outFile.is_open()) {
        // save game state
        outFile << currentPlayer << std::endl;
        for (const auto &player : players) {
            outFile << player.getName() << " " << player.getTeamName() << " " << player.getSoldiers() << " "
            << player.getResources() << " " << player.getPosition() << " " << player.isAI() << std::endl;
        }

        outFile.close();
        std::cout <<"Game saved successfully!" << std::endl;
    } else {
        std::cout << "Error saving game!" << std::endl;
    }
}

void Game::loadGame() {
    std::ifstream inFile("savegame.txt");
    if (inFile.is_open()) {
        // Load game state
        inFile >> currentPlayer;
        players.clear();

        std::string name, team;
        int soldiers, resources, position;
        bool isAI;

        while (inFile >> name >> team >> soldiers >> resources >>position >> isAI) {
            players.emplace_back(name, team, isAI);
            players.back().setSoldiers(soldiers);
            players.back().setResources(resources);
            players.back().setPosition(position);
        }

        inFile.close();
        std::cout << "Game loaded successfully!" << std::endl;
    } else {
        std::cout << "Error loading game!" << std::endl;
    }
}

void Game::displayStoryIntro() {
    std::cout << "============================================================================================\n";
    std::cout << "                    🚀 Battle for Nexus! 🌌\n";
    std::cout << "============================================================================================\n\n";
    std::cout << "   🌍 Humanity has colonized Nexus, a planet on the brink of destruction. 🔥\n";
    std::cout << "   ⚔️ Two factions—The Alliance of Light ✨ and Coalition of Shadows 🌑—have declared war.\n";
    std::cout << "   The goal: gather resources, fortify positions 🏰, and control Safe Zones 🛡️. Only one will win.\n";
    std::cout << "   💫 The future of Nexus is in your hands...\n";
    std::cout << "--------------------------------------------------------------------------------------------\n\n";

    std::cout << "🔑 Game Rules & Strategy:\n";
    std::cout << "----------------------------\n";
    std::cout << "1. **Objective**: Defeat the enemy by: \n";
    std::cout << "   - Capturing all enemy soldiers. 👑\n";
    std::cout << "   - Controlling Safe Zones or reaching 100 points. 🎯\n";
    std::cout << "2. **Setup**: Each player controls 10 soldiers on a 13-square grid.\n";
    std::cout << "3. **Turns**: Move, gather resources 🪙, fortify, heal, and enter Safe Zones 🛡️.\n";
    std::cout << "4. **Hazards**: Avoid damaging squares ⚠️.\n";
    std::cout << "5. **Market Nodes**: Trade resources for upgrades. 💸\n\n";

    std::cout << "🎯 Winning the Game:\n";
    std::cout << "----------------------\n";
    std::cout << "   - **Eliminate enemy soldiers** ⚔️\n";
    std::cout << "   - **Control Safe Zones** 🏰: Hold for 3 consecutive turns.\n";
    std::cout << "   - **Reach 100 Points** 🌟: Through objectives and strategic moves.\n\n";

    std::cout << "💡 Strategy Tips:\n";
    std::cout << "----------------------------\n";
    std::cout << "   - Collect resources early for an advantage.\n";
    std::cout << "   - Control Safe Zones for defense and healing.\n";
    std::cout << "   - Avoid Hazards ⚠️ and trade at Market Nodes 💸.\n";
    std::cout << "   - Attack wisely and adapt your strategy.\n";
    std::cout << "   - Focus on earning points through objectives and resources. 📈\n\n";

    std::cout << "Good luck, Commander. The Battle for Nexus has begun! 🌟\n";
    std::cout << "============================================================================================\n";
}


void Game::displayFactionInfo(Player &player) {
    std::cout << "\n";
    if (player.getTeamName() == "Alliance of Light") {
        std::cout << "The Alliance of Light believes in unity and peace. Their mision is to restore order to Nexus and protectits resources.\n";
    } else if (player.getTeamName() == "Coalition of Shadows") {
        std::cout << "The Coalition of Shadows will stop at nothing to control Nexus and dominate its resources.\n";
    } else if (player.isAI()) {
        std::cout << "I am determined to take control of the planet.\n";
    }
}


void Game::displayMenu(Player &player) {
    int choice = 0;
    std::cout << "-----------------------------------------\n";
    std::cout << "1. Continue playing\n";
    std::cout << "2. Save game\n";
    std::cout << "3. Load game\n";
    std::cout << "4. Quit game\n";
    std::cout << "Enter your choice: ";
    if (!(std::cin >> choice)) {
        if (std::cin.eof()) exit(0);
        std::cin.clear();              // recover from non-numeric input
        std::cin.ignore(1000, '\n');
        choice = 0;
    }

    switch (choice) {
        case 1:
            // Continue playing
            break;
        case 2:
            saveGame(); // Call the existing saveGame function
            break;
        case 3:
            loadGame(); // Call the existing loadGame function
            break;
        case 4:
            std::cout << "Quitting game...\n";
            exit(0); // Exit the game
            break;
        default:
            std::cout << "Invalid choice! Continuing the game...\n";
            break;
    }
}


void Game::startGame() {
    while (true) {
        playTurn();
        if (playerWins()) {
            break;
        }
    }
}

void Game::playTurn() {
    Player &player = players.at(currentPlayer);

    if (!player.isAI()) {
        displayMenu(player);
    }

    // Get the number of steps
    int steps = player.getMove();

    // Display faction info and separator
    displayFactionInfo(player);
    std::cout << "-----------------------------------------\n";

    // Move the player
    std::string direction = getMoveDirection(player);
    movePlayer(player, direction, steps);

    // Handle tile effects
    Tile &tile = getTileAtPlayerPosition(player);
    handleTileEffects(player, tile);

    // Display status for all players
    displayPlayerStatuses();

    // Switch to the next player
    currentPlayer = (currentPlayer + 1) % players.size();
}

// Helper function to get the move direction from the player or AI
std::string Game::getMoveDirection(Player &player) {
    if (player.isAI()) {
        std::vector<std::string> directions = {
            "up", "down", "left", "right", 
            "up-left", "up-right", "down-left", "down-right"
        };
        return directions[rand() % directions.size()];
    } else {
        std::string direction;
        printBoard();
        std::cout << "Remember, if you reach 100, you win the game, so think carefully about your move.";
        std::cout << player.getName() << "'s turn. Enter direction to move (up, down, left, right, up-left, up-right, down-left, down-right): ";
        std::cin >> direction;
        return direction;
    }
}

// Helper function to move the player
void Game::movePlayer(Player &player, const std::string &direction, int steps) {
    for (int i = 0; i < steps; ++i) {
        player.move(direction, board.size(), board[0].size());
    }
    printBoard();
}

// Helper function to get the tile at the player's current position
Tile& Game::getTileAtPlayerPosition(Player &player) {
    int position = player.getPosition();
    int row = position / board[0].size();
    int col = position % board[0].size();
    return board[row][col];
}

// Helper function to handle tile effects
void Game::handleTileEffects(Player &player, Tile &tile) {
    std::cout << player.getName() << " (" << player.getTeamName() << ") landed on: " << tile.description << std::endl;

    switch (tile.type) {
        case TileType::RESOURCE: 
            player.collectResources(10); 
            break;
        case TileType::HAZARD: 
            player.loseSoldier(); 
            break;
        case TileType::MARKET: 
            handleMarketInteraction(player); 
            break;
        case TileType::SAFE_ZONE: 
            player.addSoldiers(1); 
            break;
        case TileType::CHEAT: // Handle cheat tiles
            std::cout << "You found a hidden cheat tile! Here's how to use cheat codes:\n";
            std::cout << "\n=== Cheat Code Guide ===\n";
            std::cout << "1. RESOURCES: Gain 100 resources.\n";
            std::cout << "2. SOLDIERS: Gain 10 soldiers.\n";
            std::cout << "3. TELEPORT: Teleport to a specific tile (enter row and column).\n";
            std::cout << "4. REVEAL: Reveal the entire map.\n";
            std::cout << "5. SABOTAGE: Sabotage the opponent (lose 1 soldier and 50 resources).\n";
            std::cout << "=======================\n\n";
            handleCheatCode(player); // Allow the player to use a cheat code
            break;
        default: 
            break;
    }
}

// Helper function to handle market interactions
void Game::handleMarketInteraction(Player &player) {
    int choice;
    std::cout << "You have found a Market! Do you want to trade? (1 = Yes, 2 = No): ";

    while (!(std::cin >> choice) || (choice != 1 && choice != 2)) {
        std::cout << "Invalid input! Please enter 1 for Yes or 2 for No: ";
        std::cin.clear();  // Clear error flag
        std::cin.ignore(1000, '\n');  // Discard invalid input
    }

    if (choice == 1) {
        int tradeAmount;
        std::cout << "Enter amount of resources to trade for soldiers (5 resources = 1 soldier): ";

        while (!(std::cin >> tradeAmount) || tradeAmount < 0) {
            std::cout << "Invalid input! Please enter a valid non-negative integer: ";
            std::cin.clear();  // Clear error flag
            std::cin.ignore(1000, '\n');  // Discard invalid input
        }

        if (player.getResources() >= tradeAmount) {
            player.collectResources(-tradeAmount);
            player.addSoldiers(tradeAmount / 5);
            std::cout << "Trade successful! You gained " << (tradeAmount / 5) << " soldiers." << std::endl;
        } else {
            std::cout << "Not enough resources!" << std::endl;
        }
    }
}



/*void Game::handleMarketInteraction(Player &player) {
    int choice;
    std::cout << "You have found a Market! Do you want to trade? (1 = Yes, 2 = No): ";
    std::cin >> choice;
    if (choice == 1) {
        int tradeAmount;
        std::cout << "Enter amount of resources to trade for soldiers (5 resources = 1 soldier): ";
        std::cin >> tradeAmount;
        if (player.getResources() >= tradeAmount) {
            player.collectResources(-tradeAmount);
            player.addSoldiers(tradeAmount / 5);
            std::cout << "Trade successful! You gained " << (tradeAmount / 5) << " soldiers." << std::endl;
        } else {
            std::cout << "Not enough resources!" << std::endl;
        }
    }
} */

// Helper function to display all players' statuses
void Game::displayPlayerStatuses() {
    std::cout << "-----------------------------------------\n";
    for (const auto &p : players) {
        p.displayStatus();
    }
    std::cout << "-----------------------------------------\n";
}

// Helper function to handle cheat codes
void Game::handleCheatCode(Player &player) {
    std::string cheatCode;
    std::cout << "Enter cheat code (type HELP for a list of cheats): ";
    std::cin >> cheatCode;

    if (cheatCode == "HELP") {
        // Display cheat code guide
        std::cout << "\n=== Cheat Code Guide ===\n";
        std::cout << "1. RESOURCES: Gain 100 resources.\n";
        std::cout << "2. SOLDIERS: Gain 10 soldiers.\n";
        std::cout << "3. TELEPORT: Teleport to a specific tile (enter row and column).\n";
        std::cout << "4. REVEAL: Reveal the entire map.\n";
        std::cout << "5. SABOTAGE: Sabotage the opponent (lose 1 soldier and 50 resources).\n";
        std::cout << "=======================\n\n";
    } else if (cheatCode == "RESOURCES") {
        player.collectResources(100); // Add 100 resources
        std::cout << player.getName() << " used the RESOURCES cheat! +100 Resources!" << std::endl;
    } else if (cheatCode == "SOLDIERS") {
        player.addSoldiers(10); // Add 10 soldiers
        std::cout << player.getName() << " used the SOLDIERS cheat! +10 Soldiers!" << std::endl;
    } else if (cheatCode == "TELEPORT") {
        size_t row, col; // Use size_t instead of int
        std::cout << "Enter row and column to teleport to (e.g., 5 7): ";
        std::cin >> row >> col;

        // No need for row >= 0 and col >= 0 checks since size_t is unsigned
        if (row < board.size() && col < board[0].size()) {
            player.setPosition(row * board[0].size() + col); // Teleport to the specified tile
            std::cout << player.getName() << " used the TELEPORT cheat! Moved to (" << row << ", " << col << ")." << std::endl;
        } else {
            std::cout << "Invalid position! Teleport failed." << std::endl;
        }
    } else if (cheatCode == "REVEAL") {
        std::cout << "Revealing the entire map for " << player.getName() << "..." << std::endl;
        printBoard(); // Print the board to reveal all tiles
    } else if (cheatCode == "SABOTAGE") {
        Player &opponent = players.at((currentPlayer + 1) % players.size()); // Get the opponent
        opponent.loseSoldier(); // Reduce opponent's soldiers
        opponent.collectResources(-50); // Reduce opponent's resources
        std::cout << player.getName() << " used the SABOTAGE cheat! Opponent lost 1 soldier and 50 resources!" << std::endl;
    } else {
        std::cout << "Invalid cheat code! Type HELP for a list of available cheats.\n";
    }
}


// Function to display the game board
void Game::printBoard() {
    system("clear"); // Clear the console (use "cls" for Windows)
    std::cout << "===================================================\n";
    std::cout << "                   GAME BOARD (10x10)              \n";
    std::cout << "===================================================\n";

    // Print column numbers
    std::cout << "     ";
    for (size_t col = 0; col < board[0].size(); ++col) {
        std::cout << "  " << col << "  "; // Column numbers with spacing
    }
    std::cout << "\n";

    // Print the board with row numbers and borders
    for (size_t row = 0; row < board.size(); ++row) {
        // Print top border of the row
        std::cout << "     +";
        for (size_t col = 0; col < board[row].size(); ++col) {
            std::cout << "-----+";
        }
        std::cout << "\n";

        // Print row number and left border
        std::cout << " " << row << "  |"; // Row number with spacing

        // Print tile contents
        for (size_t col = 0; col < board[row].size(); ++col) {
            std::string cellContent = "    "; // Default empty space

            // Check if a player or AI is on this tile
            bool isPlayerHere = false;
            for (const auto& player : players) {
                int playerRow = player.getPosition() / board[0].size();
                int playerCol = player.getPosition() % board[0].size();
                if (static_cast<size_t>(playerRow) == row && static_cast<size_t>(playerCol) == col) {
                    cellContent = (player.isAI()) ? "AI  " : "P1  "; // AI is "AI", Player 1 is "P1"
                    isPlayerHere = true;
                    break;
                }
            }

            // If no player is on this tile, display the tile type
            if (!isPlayerHere) {
                const Tile& tile = board[row][col];
                switch (tile.type) {
                    case TileType::RESOURCE: cellContent = " Re "; break;
                    case TileType::SAFE_ZONE: cellContent = " Sa "; break;
                    case TileType::HAZARD: cellContent = " Ha "; break;
                    case TileType::MARKET: cellContent = " Ma "; break;
                    case TileType::CHEAT: cellContent = " Ch "; break; // Add cheat tile display
                    case TileType::EMPTY: cellContent = "    "; break;
                }
            }

            std::cout << " " << cellContent << " |";
        }
        std::cout << "\n";
    }

    // Print bottom border of the board
    std::cout << "     +";
    for (size_t col = 0; col < board[0].size(); ++col) {
        std::cout << "-----+";
    }
    std::cout << "\n";

    // Legend for tile types and players
    std::cout << "\nLegend:\n";
    std::cout << "  Re  - Resource | Sa  - Safe Zone | Ha  - Hazard | Ma  - Market | Ch  - Cheat Tile\n";
    std::cout << "  P1  - Player 1 (Human) | AI  - Artificial Intelligence\n";
    std::cout << "===================================================\n";
}

