#include "Player.h"
#include <cstdlib>
#include <ctime>


bool Player::isAI() const { return aiControlled; }

std::string Player::getTeamName() const { return teamName; }
std::string Player::getName() const { return name; }

int Player::getPosition() const { return position; }
int Player::getSoldiers() const { return soldiers; }
int Player::getResources() const { return resources; }

void Player::setSoldiers(int soldiers) { this->soldiers = soldiers; }
void Player::setResources(int resources) { this->resources = resources; }
void Player::setPosition(int position) { this->position = position; }


Player::Player(std::string name, std::string team, bool isAI) 
    : name(name), teamName(team), soldiers(10), resources(0), position(0), aiControlled(isAI), moveSteps(0) {
    // Dynamically allocate resources
    soldierTypes = std::make_unique<int[]>(soldiers);
}

Player::Player(const Player& other)
    : name(other.name), teamName(other.teamName), soldiers(other.soldiers), resources(other.resources), 
    position(other.position), aiControlled(other.aiControlled), moveSteps(other.moveSteps) {
    soldierTypes = std::make_unique<int[]>(soldiers); // Copy dynamically allocated array
    for (int i = 0; i < soldiers; i++) {
        soldierTypes[i] = other.soldierTypes[i];
    }
}

Player& Player::operator=(const Player& other) {
    if (this != &other) {
        name = other.name;
        teamName = other.teamName;
        soldiers = other.soldiers;
        resources = other.resources;
        position = other.position;
        aiControlled = other.aiControlled;
        moveSteps = other.moveSteps;

        // Allocate new memory and copy values
        soldierTypes = std::make_unique<int[]>(soldiers);
        for (int i = 0; i < soldiers; i++) {
            soldierTypes[i] = other.soldierTypes[i];
        }
    }
    return *this;
}

Player::~Player(){
    // Destructor to clean up dynamic memory
}
    
void Player::move(const std::string& direction, int rows, int cols) {
    int newRow = position / cols; // Calculate current row based on position
    int newCol = position % cols; // Calculate current column based on position

    if (direction == "up" && newRow > 0) {
        newRow--; // Move up (decrease row)
    } else if (direction == "down" && newRow < rows -1) {
        newRow++; // Move down (increase row)
    } else if (direction == "left" && newCol > 0) {
        newCol--; // Move left (decrease column)
    } else if (direction == "right" && newCol < cols -1) {
        newCol++; // Move right (increase column)
    } else if (direction == "up-left" && newRow > 0 && newCol > 0) {
        newRow--; //Move up-left (diagonal)
    } else if (direction == "up-right" && newRow > 0 && newCol < cols - 1) {
        newRow--; newCol++; // Move up-right (diagonal)
    } else if (direction == "down-left" && newRow < rows -1 && newCol > 0) {
        newRow++; newCol--; // Move down-left (diagonal)
    } else if (direction == "down-right" && newRow < rows -1 && newCol < cols - 1) {
        newRow++; newCol++; // Move down-right (diagonal)
    }

    // Update the player's position
    position = newRow * cols + newCol; // Calculate new position based on Updated row and column
}

int Player::getMove() const {
    if (aiControlled) {
        return rand() % 6 + 1; // AI moves randomly between 1 - 6 steps
    } else {
        int steps;
        while (true) {
            std::cout << "You can only move from 1 to 6 on the board!";
            std::cout << name << "'s turn. Enter steps to move: ";
            if (!(std::cin >> steps)) {
                if (std::cin.eof()) exit(0);
                std::cin.clear();              // recover from non-numeric input
                std::cin.ignore(1000, '\n');
                steps = 0;
            }
            if (steps >= 1 && steps <= 6) break;
            std::cout << "Invalid input. Please enter a number between 1 and 6." << std::endl;
        }
        return steps;
    }
}
        

void Player::collectResources(int amount) {
    resources += amount;
}

void Player::addSoldiers(int count) {
    soldiers +=count; 
}

void Player::loseSoldier() {
    if (soldiers > 0) {
        soldiers--;
    }
}

void Player::displayStatus() const {
    std::cout << name << " (" << teamName << ") - Soldiers: " << soldiers << " Resource: " << resources << " Position: " << position << std::endl;
}