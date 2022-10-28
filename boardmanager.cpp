#pragma once

#include <memory>
#include <iostream>

#include "boardmanager.h"
#include "supercell.h"

BoardManager::BoardManager()
{
}

bool BoardManager::superCellExists(int64_t x, int64_t y)
{
    return boardPieces.count(std::make_pair(x, y));
}

void BoardManager::openSuperCell(int64_t x, int64_t y)
{
    if (!superCellExists(x, y))
    {
        boardPieces[std::make_pair(x, y)] = std::make_unique<SuperCell>(x, y, this);
    }
}

void BoardManager::setCell(int64_t x, int64_t y)
{
    // Translates x/y into SuperCell's x/y
    auto metaX = x/BITSET_SIZE;
    auto metaY = y/BITSET_SIZE;

    // Adjustment for negative values
    if (x < 0) metaX--;
    if (y < 0) metaY--;
    
    openSuperCell(metaX, metaY);

    // Converts global x/y to local (to the supercell) x/y
    auto localX = x%BITSET_SIZE;
    auto localY = y%BITSET_SIZE;

    if (localX < 0) localX += BITSET_SIZE;
    if (localY < 0) localY += BITSET_SIZE;

    boardPieces[std::make_pair(metaX, metaY)]->setCell(localX, localY);
}

void BoardManager::printBoard()
{
    for (auto const& piece : boardPieces)
    {
        std::cout << "** SUPERCELL AT (" << piece.first.first << "," << piece.first.second << ") **" << std::endl;
        auto scell = piece.second.get();

        for (int i = BITSET_SIZE - 1; i >= 0; i--) 
        {
            auto row = scell->board->at(i).get();
            for (int j = 0; j < BITSET_SIZE; j++)
            {
                std::cout << (row->test(j) ? "■" : ".");
            }
            std::cout << std::endl;
        }

        std::cout << std::endl;
    }
}

void BoardManager::simulateStep()
{
    // Check if new supercells are needed
    for (auto const& piece : boardPieces)
    {
        auto coords = piece.first;
        auto scell = piece.second.get();

        // Check up
        if (!superCellExists(coords.first+1, coords.second)) {
            if (scell->getEdge(CellEdge::top).any()) {
                openSuperCell(coords.first+1, coords.second);
            }
        }

        // Check down
        if (!superCellExists(coords.first-1, coords.second)) {
            if (scell->getEdge(CellEdge::bottom).any()) {
                openSuperCell(coords.first-1, coords.second);
            }
        }

        // Check left
        if (!superCellExists(coords.first, coords.second-1)) {
            if (scell->getEdge(CellEdge::left).any()) {
                openSuperCell(coords.first, coords.second-1);
            }
        }

        // Check right
        if (!superCellExists(coords.first, coords.second+1)) {
            if (scell->getEdge(CellEdge::right).any()) {
                openSuperCell(coords.first, coords.second+1);
            }
        }
    }

    // Trigger next generation (does not commit board to new generation)
    for (auto const& piece : boardPieces)
    {
        piece.second->simulateStep(); 
    }

    // Commit each board to the new generated generation
    for (auto const& piece : boardPieces)
    {
        piece.second->commitStep();
    }

    // Remove supercells that are now empty
    for (auto it = begin(boardPieces); it != end(boardPieces); it++)
    {
        if (it->second->isEmpty())
        {
            boardPieces.erase(it++);
        }
    }
}

std::bitset<BITSET_SIZE> BoardManager::getEdge(int64_t x, int64_t y, CellEdge edge)
{
    if (!boardPieces.count(std::make_pair(x, y)))
    {
        return std::bitset<BITSET_SIZE>();
    }

    return boardPieces[std::make_pair(x, y)]->getEdge(edge);
}

bool BoardManager::getCorner(int64_t x, int64_t y, CellCorner corner)
{
    if (!boardPieces.count(std::make_pair(x, y)))
    {
        return false;
    }

    return boardPieces[std::make_pair(x, y)]->getCorner(corner);
}