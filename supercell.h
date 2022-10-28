#pragma once

#include <vector>
#include <bitset>
#include <memory>

class BoardManager;

#define BITSET_SIZE 64

enum CellEdge { top, bottom, left, right };
enum CellCorner { upperLeft, upperRight, lowerLeft, lowerRight };

typedef std::unique_ptr<std::bitset<BITSET_SIZE>> BOARD_ROW;

/// @brief Represents an NxN section of the grid. 
class SuperCell
{
public: 
    // Supercell coordinates (in major grid)
    int64_t coordX;
    int64_t coordY;

    BoardManager* boardManager;

    std::unique_ptr<std::vector<BOARD_ROW>> board;
    std::unique_ptr<std::vector<BOARD_ROW>> nextGen;

public: 

    SuperCell(int64_t x, int64_t y, BoardManager* boardManager);

    void setCell(int64_t x, int64_t y, bool alive = true);
    void simulateStep();
    void commitStep();
    
    std::bitset<BITSET_SIZE> getEdge(CellEdge edge);
    bool getCorner(CellCorner corner);
    bool isEmpty();
};