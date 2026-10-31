#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include <string>
#include <memory> // for dynamic allocation

// Player class
class Player {
private:
    std::string name;
    std::string teamName;
    int soldiers;
    int resources;
    int position;
    bool aiControlled;
    int moveSteps;

    // Dynamic allocation (dynamically allocated array for soldier types)
    std::unique_ptr<int[]> soldierTypes;

public:

    Player(std::string name, std::string team, bool isAI = false);
    Player(const Player& other); // Copy Constructor
    Player& operator=(const Player& other); // Assignment operator
    ~Player(); // Destructor to handle dynamic memory

    std::string getTeamName() const;
    std::string getName() const;
    bool isAI () const;
    int getMove() const;
    int getPosition() const;
    int getSoldiers() const;
    int getResources() const;
    void setSoldiers( int soldiers);
    void setResources(int resources);
    void setPosition(int position);
    void addSoldiers(int count);
    
    void move(const std::string& direction, int rows, int cols);
    void collectResources(int amount);
    void loseSoldier();
    void displayStatus() const;
};

#endif