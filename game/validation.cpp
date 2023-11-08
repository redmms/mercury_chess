#pragma once
#include "validation.h"
#include "board.h"

void Validation::validMove(Tile* from, Tile* to)
{
    int ret_value = 0, i = 0;

    // FIX: it's better to read this method carefully, the error may be here

    // if valid moves number is more than 0
    // because 1 is probably for the tile itseft?
    // or just because in the original project it was always max++
    // FIX: instead of valid_n checking we can just check if m_from_tile is 
    // not null
    if (valid_n == 1)
    {
        if (temp->m_has_piece && temp->m_white_piece == m_white_turn)
        {
            //exp[m_valid_n++]=temp->m_tile_num;
            ret_value = valid->findValid(temp);
            // will paint all valid tiles in orange

            if (ret_value)
            {
                // m_from_tile= new Tile(); // FIX: the source of the problem is here
                temp->setStyleSheet("QLabel {background-color: green;}");
                m_from_tile = temp;
            }
            // and it will paint the first click in green

            else
            {
                //temp->setStyleSheet("QLabel {background-color: red;}");
                m_valid_n = 0;
            }
            // reset the counter of valid moves for future use

        }
        else
        {
            //qDebug()<<"Rascel, clicking anywhere";
            m_valid_n = 0;
        }
    }

    // if the color of piece differ from the color of turn
    // or if the tile is empty
    // then disOrange valid tiles (return back their normal color)
    else
    {

        // the first click is disoranged apart from valid moves
        // it shouldn't be in the Validation::m_valid_moves[] actually
        if (temp->m_tile_num == m_from_tile->m_tile_num)
        {
            m_from_tile->dyeNormal();
            valid->disOrange(); // FIX: this line is not necessary, is it?
            valid->m_valid_n = 0;
            m_valid_n = 0;
        }

        // FIX: why would we need this cycle?
        // if we can call valid->disOrange() only once?
        for (i = 0; i < valid->m_valid_n; i++)  // FIX: maybe the problem with
            // validation is because of m_valid_n is bigger than needed?
        {
            if (temp->m_tile_num == valid->m_valid_moves[i])
            {
                m_from_tile->m_has_piece = 0;
                temp->m_has_piece = 1;

                temp->m_white_piece = m_from_tile->m_white_piece;
                temp->m_piece_name = m_from_tile->m_piece_name;

                m_from_tile->drawPiece(m_from_tile->m_piece_name);
                temp->drawPiece(m_from_tile->m_piece_name);

                m_from_tile->dyeNormal();
                temp->dyeNormal();

                ret_value = valid->check(m_from_tile);

                // FIX: what does this do? Some very misterious line
                // and the place where wR and wC are used
                /*
                if(ret_value)
                {
                    m_tiles[m_wR][m_wC]->setStyleSheet("QLabel {background-color: red;}");
                }
                */

                valid->disOrange();

                valid->m_valid_n = 0;

                // shouldn't it be outside of the cycle?
                m_valid_n = 0;
            }

            else
                m_valid_n = 1;
        }
    }
}

int Validation::findValid(Tile *temp)
{
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

void Validation::dyeOrange()
{
    int i/*,n*/;  // FIX: What is n here? Should here be some more code?
    for(i=0; i < m_valid_n;i++)
        m_board[m_valid_moves[i]/8][m_valid_moves[i]%8]->setStyleSheet("QLabel {background-color: orange;}");
}

void Validation::disOrange()
{
    int i;
    for (i = 0; i < m_valid_n; i++)
        m_board[m_valid_moves[i] / 8][m_valid_moves[i] % 8]->dyeNormal();

}

int Validation::check(Tile *temp)  // FIX: Why there's no check? Was it planned as TODO?
 //FIX: P.S. I suppose it's meant to check if a king (temp) is under the check
{
    int r,c,m_flag;
    int return_count=0;

    return return_count;
}


