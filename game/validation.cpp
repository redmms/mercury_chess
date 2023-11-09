#pragma once
#include "validation.h"
#include "board.h"
using namespace std;

void Validation::showPossible(Tile * from)
{
    findValid(from);
    for (auto tile : m_valid_moves)
        tile->setStyleSheet("background: orange;");
    from->setStyleSheet("background: green;");
}

void Validation::hidePossible()
{
    for (auto tile : m_valid_moves)
        tile->dyeNormal();
    m_valid_moves.clear();
}

void Validation::findValid(Tile *from)
{
    pair coord(from->m_coord);
    auto onBoard = [](pair<int, int> coord) {
        return coord.first >= 0 && coord.first < 8 && coord.second >= 0 && coord.second < 8;
        };
    auto differentColor = [this, coord]() {
        // FIX: need to consider field names better
        // FIX: need to change i and j to x and y;
        return m_board[coord.second][coord.first]->m_white_piece != m_white_piece;
        };
    auto occupied = [&](pair <int, int> coord) {
        return m_board[coord.second][coord.first]->m_has_piece;
        };
    auto pieceName = [&](pair <int, int> coord) {
        return m_board[coord.second][coord.first]->m_piece_name;
    };
    auto runThrough = [&](pair<int, int> from, pair<int, int> add, bool& check_func()) {
        for (; !check_func(); from.first += add.first, from.second += add.second)
            if (!onBoard(from))
                return false;
        return true;
        };
    auto onDiagonals = [&](pair<int, int> from, bool& check_func()) {
        return runThrough(from, { 1, 1 }, check_func) ||
            runThrough(from, { 1, -1 }, check_func) ||
            runThrough(from, { -1, 1 }, check_func) ||
            runThrough(from, { -1, -1 }, check_func);
        };
    auto onPerp = [&](pair<int, int> from, bool& check_func()) {
        return runThrough(from, { 0, 1 }, check_func) ||
            runThrough(from, { 0, -1 }, check_func) ||
            runThrough(from, { 1, 0 }, check_func) ||
            runThrough(from, { -1, 0 }, check_func);
        };
    auto onLines = [&](pair<int, int> from, pair<int, int> to, bool& check_func()) {
        int X = to.first - from.first;
        int Y = to.second - from.second;
        if (!Y)  // horizontal line
            if (X > 0) // to the right
                return runThrough(from, { 1, 0 }, check_func);
            else  // to the left
                return runThrough(from, { -1, 0 }, check_func);
        else if (!X)  // vertical line
            if (Y > 0) // to the top
                return runThrough(from, { 0, 1 }, check_func);
            else  // to the bottom
                return runThrough(from, { 0, -1 }, check_func);
        else if (X == Y)  // diagonal line
            if (X > 0 && Y > 0) // to the top right
                return runThrough(from, { 1, 1 }, check_func);
            else if (X > 0 && Y < 0) // to the bottom right
                return runThrough(from, { 1, -1 }, check_func);
            else if (X < 0 && Y > 0) // to the top left
                return runThrough(from, { -1, 1 }, check_func);
            else // to the bottom left  
                return runThrough(from, { -1, -1 }, check_func);
        };


        //int X =  board->king_coord.first - tile->m_col;
        //int Y = board->king_coord.second - tile->m_row;

        pair king(board->king_coord);
        int X = king.first - tile->m_col;
        int Y = king.second - tile->m_row;

        bool on_same_line = X == Y || !X || !Y;
        bool nothing_between = fromTo(pair(this_tile->m_col, this_tile->m_row), coord, !occupied);
        bool is_attacked = ;

        auto
            if (&& throughDiagonal(pair(m_col, m_row), temp.coord);



























    int return_count = 0;

    auto validatePawn = [&](Tile* temp) {
        int row, col;

        row = temp->m_row;
        col = temp->m_col;

        if (temp->m_white_piece)
        {
            if (row - 1 >= 0 && !m_board[row - 1][col]->m_has_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[row - 1][col]->m_tile_num;
                return_count = 1;
            }

            if (row == 6 && !m_board[5][col]->m_has_piece && !m_board[4][col]->m_has_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[row - 2][col]->m_tile_num;
                return_count = 1;
            }

            if (row - 1 >= 0 && col - 1 >= 0)
            {
                if (m_board[row - 1][col - 1]->m_white_piece != temp->m_white_piece && m_board[row - 1][col - 1]->m_has_piece)
                {
                    m_valid_moves[m_valid_n++] = m_board[row - 1][col - 1]->m_tile_num;
                    return_count = 1;
                }
            }

            if (row - 1 >= 0 && col + 1 <= 7)
            {
                if (m_board[row - 1][col + 1]->m_white_piece != temp->m_white_piece && m_board[row - 1][col + 1]->m_has_piece)
                {
                    m_valid_moves[m_valid_n++] = m_board[row - 1][col + 1]->m_tile_num;
                    return_count = 1;
                }
            }
        }
        else
        {
            if (row + 1 <= 7 && !m_board[row + 1][col]->m_has_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[row + 1][col]->m_tile_num;
                return_count = 1;
            }

            if (row == 1 && !m_board[2][col]->m_has_piece && !m_board[3][col]->m_has_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[row + 2][col]->m_tile_num;
                return_count = 1;
            }

            if (row + 1 <= 7 && col - 1 >= 0)
            {
                if (m_board[row + 1][col - 1]->m_white_piece != temp->m_white_piece && m_board[row + 1][col - 1]->m_has_piece)
                {
                    m_valid_moves[m_valid_n++] = m_board[row + 1][col - 1]->m_tile_num;
                    return_count = 1;
                }
            }

            if (row + 1 <= 7 && col + 1 <= 7)
            {
                if (m_board[row + 1][col + 1]->m_white_piece != temp->m_white_piece && m_board[row + 1][col + 1]->m_has_piece)
                {
                    m_valid_moves[m_valid_n++] = m_board[row + 1][col + 1]->m_tile_num;
                    return_count = 1;
                }
            }
        }


        };
    auto validateRook = [&](Tile* temp) {
        int r, c;

        int return_count = 0;

        r = temp->m_row;
        c = temp->m_col;
        while (r-- > 0)
        {
            if (!m_board[r][c]->m_has_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r][c]->m_tile_num;
                return_count = 1;
            }

            else if (m_board[r][c]->m_white_piece == temp->m_white_piece)
                break;

            else if (m_board[r][c]->m_white_piece != temp->m_white_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r][c]->m_tile_num;
                return_count = 1;
                break;
            }
        }

        r = temp->m_row;
        c = temp->m_col;
        while (r++ < 7)
        {
            if (!m_board[r][c]->m_has_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r][c]->m_tile_num;
                return_count = 1;
            }

            else if (m_board[r][c]->m_white_piece == temp->m_white_piece)
                break;

            else if (m_board[r][c]->m_white_piece != temp->m_white_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r][c]->m_tile_num;
                return_count = 1;
                break;
            }
        }

        r = temp->m_row;
        c = temp->m_col;
        while (c++ < 7)
        {
            if (!m_board[r][c]->m_has_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r][c]->m_tile_num;
                return_count = 1;
            }

            else if (m_board[r][c]->m_white_piece == temp->m_white_piece)
                break;

            else if (m_board[r][c]->m_white_piece != temp->m_white_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r][c]->m_tile_num;
                return_count = 1;
                break;
            }
        }

        r = temp->m_row;
        c = temp->m_col;
        while (c-- > 0)
        {
            if (!m_board[r][c]->m_has_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r][c]->m_tile_num;
                return_count = 1;
            }

            else if (m_board[r][c]->m_white_piece == temp->m_white_piece)
                break;

            else if (m_board[r][c]->m_white_piece != temp->m_white_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r][c]->m_tile_num;
                return_count = 1;
                break;
            }
        }
        };
    auto validateKnight = [&](Tile* temp) {
        int r, c;
        int return_count = 0;

        r = temp->m_row;
        c = temp->m_col;

        if (r - 2 >= 0 && c - 1 >= 0)
        {
            if (m_board[r - 2][c - 1]->m_white_piece != temp->m_white_piece || !m_board[r - 2][c - 1]->m_has_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r - 2][c - 1]->m_tile_num;
                return_count = 1;
            }
        }

        if (r - 2 >= 0 && c + 1 <= 7)
        {
            if (m_board[r - 2][c + 1]->m_white_piece != temp->m_white_piece || !m_board[r - 2][c + 1]->m_has_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r - 2][c + 1]->m_tile_num;
                return_count = 1;
            }
        }

        if (r - 1 >= 0 && c - 2 >= 0)
        {
            if (m_board[r - 1][c - 2]->m_white_piece != temp->m_white_piece || !m_board[r - 1][c - 2]->m_has_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r - 1][c - 2]->m_tile_num;
                return_count = 1;
            }
        }

        if (r - 1 >= 0 && c + 2 <= 7)
        {
            if (m_board[r - 1][c + 2]->m_white_piece != temp->m_white_piece || !m_board[r - 1][c + 2]->m_has_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r - 1][c + 2]->m_tile_num;
                return_count = 1;
            }
        }

        if (r + 2 <= 7 && c + 1 <= 7)
        {
            if (m_board[r + 2][c + 1]->m_white_piece != temp->m_white_piece || !m_board[r + 2][c + 1]->m_has_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r + 2][c + 1]->m_tile_num;
                return_count = 1;
            }
        }

        if (r + 2 <= 7 && c - 1 >= 0)
        {
            if (m_board[r + 2][c - 1]->m_white_piece != temp->m_white_piece || !m_board[r + 2][c - 1]->m_has_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r + 2][c - 1]->m_tile_num;
                return_count = 1;
            }
        }

        if (r + 1 <= 7 && c - 2 >= 0)
        {
            if (m_board[r + 1][c - 2]->m_white_piece != temp->m_white_piece || !m_board[r + 1][c - 2]->m_has_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r + 1][c - 2]->m_tile_num;
                return_count = 1;
            }
        }

        if (r + 1 <= 7 && c + 2 <= 7)
        {
            if (m_board[r + 1][c + 2]->m_white_piece != temp->m_white_piece || !m_board[r + 1][c + 2]->m_has_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r + 1][c + 2]->m_tile_num;
                return_count = 1;
            }
        }
        };
    auto validateKing = [&](Tile* temp) {
        int r, c;
        int return_count = 0;

        r = temp->m_row;
        c = temp->m_col;

        if (r - 1 >= 0)
        {
            if (!m_board[r - 1][c]->m_has_piece || m_board[r - 1][c]->m_white_piece != temp->m_white_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r - 1][c]->m_tile_num;
                return_count = 1;
            }
        }

        if (r + 1 <= 7)
        {
            if (!m_board[r + 1][c]->m_has_piece || m_board[r + 1][c]->m_white_piece != temp->m_white_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r + 1][c]->m_tile_num;
                return_count = 1;
            }
        }

        if (c - 1 >= 0)
        {
            if (!m_board[r][c - 1]->m_has_piece || m_board[r][c - 1]->m_white_piece != temp->m_white_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r][c - 1]->m_tile_num;
                return_count = 1;
            }
        }

        if (c + 1 <= 7)
        {
            if (!m_board[r][c + 1]->m_has_piece || m_board[r][c + 1]->m_white_piece != temp->m_white_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r][c + 1]->m_tile_num;
                return_count = 1;
            }
        }

        if (r - 1 >= 0 && c - 1 >= 0)
        {
            if (!m_board[r - 1][c - 1]->m_has_piece || m_board[r - 1][c - 1]->m_white_piece != temp->m_white_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r - 1][c - 1]->m_tile_num;
                return_count = 1;
            }
        }

        if (r - 1 >= 0 && c + 1 <= 7)
        {
            if (!m_board[r - 1][c + 1]->m_has_piece || m_board[r - 1][c + 1]->m_white_piece != temp->m_white_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r - 1][c + 1]->m_tile_num;
                return_count = 1;
            }
        }

        if (r + 1 <= 7 && c - 1 >= 0)
        {
            if (!m_board[r + 1][c - 1]->m_has_piece || m_board[r + 1][c - 1]->m_white_piece != temp->m_white_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r + 1][c - 1]->m_tile_num;
                return_count = 1;
            }
        }

        if (r + 1 <= 7 && c + 1 <= 7)
        {
            if (!m_board[r + 1][c + 1]->m_has_piece || m_board[r + 1][c + 1]->m_white_piece != temp->m_white_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r + 1][c + 1]->m_tile_num;
                return_count = 1;
            }
        }
        };
    auto validateQueen = [&](Tile* temp) {
        int r, c;

        int return_count = 0;

        r = temp->m_row;
        c = temp->m_col;
        while (r-- > 0)
        {
            if (!m_board[r][c]->m_has_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r][c]->m_tile_num;
                return_count = 1;
            }

            else if (m_board[r][c]->m_white_piece == temp->m_white_piece)
                break;

            else if (m_board[r][c]->m_white_piece != temp->m_white_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r][c]->m_tile_num;
                return_count = 1;
                break;
            }
        }

        r = temp->m_row;
        c = temp->m_col;
        while (r++ < 7)
        {
            if (!m_board[r][c]->m_has_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r][c]->m_tile_num;
                return_count = 1;
            }

            else if (m_board[r][c]->m_white_piece == temp->m_white_piece)
                break;

            else if (m_board[r][c]->m_white_piece != temp->m_white_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r][c]->m_tile_num;
                return_count = 1;
                break;
            }
        }

        r = temp->m_row;
        c = temp->m_col;
        while (c++ < 7)
        {
            if (!m_board[r][c]->m_has_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r][c]->m_tile_num;
                return_count = 1;
            }

            else if (m_board[r][c]->m_white_piece == temp->m_white_piece)
                break;

            else if (m_board[r][c]->m_white_piece != temp->m_white_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r][c]->m_tile_num;
                return_count = 1;
                break;
            }
        }

        r = temp->m_row;
        c = temp->m_col;
        while (c-- > 0)
        {
            if (!m_board[r][c]->m_has_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r][c]->m_tile_num;
                return_count = 1;
            }

            else if (m_board[r][c]->m_white_piece == temp->m_white_piece)
                break;

            else if (m_board[r][c]->m_white_piece != temp->m_white_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r][c]->m_tile_num;
                return_count = 1;
                break;
            }
        }

        r = temp->m_row;
        c = temp->m_col;
        while (r-- > 0 && c++ < 7)
        {
            if (!m_board[r][c]->m_has_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r][c]->m_tile_num;
                return_count = 1;
            }

            else if (m_board[r][c]->m_white_piece == temp->m_white_piece)
                break;

            else if (m_board[r][c]->m_white_piece != temp->m_white_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r][c]->m_tile_num;
                return_count = 1;
                break;
            }
        }

        r = temp->m_row;
        c = temp->m_col;
        while (r-- > 0 && c-- > 0)
        {
            if (!m_board[r][c]->m_has_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r][c]->m_tile_num;
                return_count = 1;
            }

            else if (m_board[r][c]->m_white_piece == temp->m_white_piece)
                break;

            else if (m_board[r][c]->m_white_piece != temp->m_white_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r][c]->m_tile_num;
                return_count = 1;
                break;
            }
        }

        r = temp->m_row;
        c = temp->m_col;
        while (r++ < 7 && c++ < 7)
        {
            if (!m_board[r][c]->m_has_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r][c]->m_tile_num;
                return_count = 1;
            }

            else if (m_board[r][c]->m_white_piece == temp->m_white_piece)
                break;

            else if (m_board[r][c]->m_white_piece != temp->m_white_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r][c]->m_tile_num;
                return_count = 1;
                break;
            }
        }

        r = temp->m_row;
        c = temp->m_col;
        while (r++ < 7 && c-->0)
        {
            if (!m_board[r][c]->m_has_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r][c]->m_tile_num;
                return_count = 1;
            }

            else if (m_board[r][c]->m_white_piece == temp->m_white_piece)
                break;

            else if (m_board[r][c]->m_white_piece != temp->m_white_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r][c]->m_tile_num;
                return_count = 1;
                break;
            }
        }
        };
    auto validateBishop = [&](Tile* temp) {
        int r, c;
        int return_count = 0;

        r = temp->m_row;
        c = temp->m_col;
        while (r-- > 0 && c++ < 7)
        {
            if (!m_board[r][c]->m_has_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r][c]->m_tile_num;
                return_count = 1;
            }

            else if (m_board[r][c]->m_white_piece == temp->m_white_piece)
                break;

            else if (m_board[r][c]->m_white_piece != temp->m_white_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r][c]->m_tile_num;
                return_count = 1;
                break;
            }
        }

        r = temp->m_row;
        c = temp->m_col;
        while (r-- > 0 && c-- > 0)
        {
            if (!m_board[r][c]->m_has_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r][c]->m_tile_num;
                return_count = 1;
            }

            else if (m_board[r][c]->m_white_piece == temp->m_white_piece)
                break;

            else if (m_board[r][c]->m_white_piece != temp->m_white_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r][c]->m_tile_num;
                return_count = 1;
                break;
            }
        }

        r = temp->m_row;
        c = temp->m_col;
        while (r++ < 7 && c++ < 7)
        {
            if (!m_board[r][c]->m_has_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r][c]->m_tile_num;
                return_count = 1;
            }

            else if (m_board[r][c]->m_white_piece == temp->m_white_piece)
                break;

            else if (m_board[r][c]->m_white_piece != temp->m_white_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r][c]->m_tile_num;
                return_count = 1;
                break;
            }
        }

        r = temp->m_row;
        c = temp->m_col;
        while (r++ < 7 && c-->0)
        {
            if (!m_board[r][c]->m_has_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r][c]->m_tile_num;
                return_count = 1;
            }

            else if (m_board[r][c]->m_white_piece == temp->m_white_piece)
                break;

            else if (m_board[r][c]->m_white_piece != temp->m_white_piece)
            {
                m_valid_moves[m_valid_n++] = m_board[r][c]->m_tile_num;
                return_count = 1;
                break;
            }
        }
        };

    switch(temp->m_piece_name)
    {
    case 'P': validatePawn(temp);
              break;

    case 'R': validateRook(temp);
              break;

    case 'N': validateKnight(temp);
              break;

    case 'K': validateKing(temp);
              break;

    case 'Q': validateQueen(temp);
              break;

    case 'B': validateBishop(temp);
              break;

    }

    dyeOrange();
    return return_count;
}