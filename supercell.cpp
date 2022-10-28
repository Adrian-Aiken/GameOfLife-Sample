#pragma once

#include <algorithm>
#include <bitset>
#include <memory>

#include "supercell.h"
#include "boardmanager.cpp"

SuperCell::SuperCell(int64_t x, int64_t y, BoardManager* boardManager)
{
    this->coordX = x;
    this->coordY = y;
    this->boardManager = boardManager;

    board = std::make_unique<std::vector<BOARD_ROW>>();
    
    // Initialization
    for (int i = 0; i < BITSET_SIZE; i++)
    {
        board->push_back(std::make_unique<std::bitset<BITSET_SIZE>>());
    }
}

void SuperCell::setCell(int64_t x, int64_t y, bool alive)
{
    board->at(x)->set(y, alive);
}

void SuperCell::commitStep()
{
    // Promotes next gen to the current board
    board.reset();
    board = std::move(nextGen);
}

void SuperCell::simulateStep()
{
    /********************************************************************************
     * The main simulation done in a few steps:
     * 1) Generate a 2d grid of living neighbors, corresponding to each cell in this
     *    SuperCell's grid
     * 2) Add in the neighbors from neighboring SuperCells for this SuperCell's edges
     * 3) Using the neighbor map, generate the next generation map based on the 
     *    Game of Life rules (3 neighbors for dead cells = living, 2/3 neighbors
     *    for living cells = living)
     ********************************************************************************/

    std::vector<std::vector<int_fast16_t>> nBoard;
    for (int i = 0; i < BITSET_SIZE; i++)
    {
        nBoard.push_back(std::vector<int_fast16_t>(BITSET_SIZE, 0));
    }    

    // Generate neighbor counts for the current generation
    for (int x = 0; x < BITSET_SIZE; x++)
    for (int y = 0; y < BITSET_SIZE; y++)
    {
        if (board->at(x)->test(y)) {
            for (int i = -1; i <= 1; i++)
            for (int j = -1; j <= 1; j++)
            {
                if (i == 0 && j == 0) continue; // skip current cell
                if (0 <= (x+i) && (x+i) < BITSET_SIZE && 0 <= (y+j) && (y+j) < BITSET_SIZE)
                {
                    nBoard[x+i][y+j]++;
                }                    
            }
        }
    }

    ///////
    // Edge neighbors
    ///////

    // Top edge neighbors
    auto top = boardManager->getEdge(coordX+1, coordY, CellEdge::bottom);
    for (int i = 0; i < BITSET_SIZE; i++)
    {
        if (top.test(i))
        {
            for (int j = std::max(i-1, 0); j <= std::min(i+1, BITSET_SIZE-1); j++)
            {
                nBoard[BITSET_SIZE-1][j]++;
            }
        }
    }

    // Bottom edge neighbors
    auto bottom = boardManager->getEdge(coordX-1, coordY, CellEdge::top);
    for (int i = 0; i < BITSET_SIZE; i++)
    {
        if (bottom.test(i))
        {
            for (int j = std::max(i-1, 0); j <= std::min(i+1, BITSET_SIZE-1); j++)
            {
                nBoard[0][j]++;
            }
        }
    }

    // Left edge neighbors
    auto left = boardManager->getEdge(coordX, coordY-1, CellEdge::right);
    for (int i = 0; i < BITSET_SIZE; i++)
    {
        if (left.test(i))
        {
            for (int j = std::max(i-1, 0); j <= std::min(i+1, BITSET_SIZE-1); j++)
            {
                nBoard[j][0]++;
            }
        }
    }

    // Right edge neighbors
    auto right = boardManager->getEdge(coordX, coordY+1, CellEdge::left);
    for (int i = 0; i < BITSET_SIZE; i++)
    {
        if (right.test(i))
        {
            for (int j = std::max(i-1, 0); j <= std::min(i+1, BITSET_SIZE-1); j++)
            {
                nBoard[j][BITSET_SIZE-1]++;
            }
        }
    }

    ///////
    // Corner neighbors
    ///////

    // Lower Left
    if (boardManager->getCorner(coordX-1, coordY-1, CellCorner::upperRight))
    {
        nBoard[0][0]++;
    }

    // Lower Right
    if (boardManager->getCorner(coordX-1, coordY+1, CellCorner::upperLeft))
    {
        nBoard[0][BITSET_SIZE-1]++;
    }

    // Upper Left
    if (boardManager->getCorner(coordX+1, coordY-1, CellCorner::lowerRight))
    {
        nBoard[BITSET_SIZE-1][0]++;
    }

    // Upper Right
    if (boardManager->getCorner(coordX+1, coordY+1, CellCorner::lowerLeft))
    {
        nBoard[BITSET_SIZE-1][BITSET_SIZE-1]++;
    }

    ///////
    // Prepare next generation's grid
    ///////
    nextGen = std::make_unique<std::vector<BOARD_ROW>>();    
    for (int i = 0; i < BITSET_SIZE; i++)
    {
        nextGen->push_back(std::make_unique<std::bitset<BITSET_SIZE>>());
    }

    // Generate cells based on current generation and neighbor count
    for (int x = 0; x < BITSET_SIZE; x++)
    for (int y = 0; y < BITSET_SIZE; y++)
    {
        // Case: Cell is currently alive
        if (board->at(x)->test(y))
        {
            if (nBoard[x][y] == 2 || nBoard[x][y] == 3)
            {
                nextGen->at(x)->set(y);
            }
        }
        // Case: Cell is currently dead
        else 
        {
            if (nBoard[x][y] == 3)
            {
                nextGen->at(x)->set(y);
            }
        }
    }
}

std::bitset<BITSET_SIZE> SuperCell::getEdge(CellEdge edge)
{
    std::bitset<BITSET_SIZE> toReturn;
    switch (edge)
    {
        case CellEdge::top:
            toReturn = *board->at(BITSET_SIZE-1).get();
            break;

        case CellEdge::bottom:
            toReturn = *board->at(0).get();
            break;

        case CellEdge::left:
            for (int i = 0; i < BITSET_SIZE; i++)
            {
                toReturn[i] = board->at(i)->test(0);
            }
            break;

        case CellEdge::right:
            for (int i = 0; i < BITSET_SIZE; i++)
            {
                toReturn[i] = board->at(i)->test(BITSET_SIZE-1);
            }
            break;
    }

    return toReturn;
}

bool SuperCell::getCorner(CellCorner corner)
{
    switch (corner)
    {
        case CellCorner::lowerLeft:
            return board->at(0)->test(0);

        case CellCorner::lowerRight:
            return board->at(0)->test(BITSET_SIZE-1);

        case CellCorner::upperLeft:
            return board->at(BITSET_SIZE-1)->test(0);

        case CellCorner::upperRight:
            return board->at(BITSET_SIZE-1)->test(BITSET_SIZE-1);
    }

    return false;
}

bool SuperCell::isEmpty()
{
    for (int i = 0; i < BITSET_SIZE; i++)
    {
        if (board->at(i)->any()) {
            return false;
        }
    }

    return true;
}