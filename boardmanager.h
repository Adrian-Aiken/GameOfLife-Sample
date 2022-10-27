#pragma once

#include <bitset>
#include <map>
#include <memory>

class SuperCell;

class BoardManager
{
    std::map<std::pair<int64_t, int64_t>, std::unique_ptr<SuperCell>> boardPieces;

public:
    BoardManager();

    bool superCellExists(int64_t x, int64_t y);
    void openSuperCell(int64_t x, int64_t y);
    void setCell(int64_t x, int64_t y);

    void simulateStep();
    void printBoard();
    
    std::bitset<BITSET_SIZE> getEdge(int64_t x, int64_t y, CellEdge edge);
    bool getCorner(int64_t x, int64_t y, CellCorner corner);
};