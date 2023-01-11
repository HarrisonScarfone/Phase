#include "initializers.hpp"
#include "util.hpp"
#include "lookup.hpp"

#include <iostream>
#include <cstdint>

Game::Position Util::Initializers::starting_position()
{
    std::string starting_position_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    return Util::Initializers::fen_string_to_position(starting_position_fen);
}

Game::Position Util::Initializers::fen_string_to_position(std::string fen_string)
{
    Game::Position new_position;
    std::vector<std::string> tokens = Util::tokenize_string_by_whitespace(fen_string);

    // A fen string needs to generate 6 tokens. use - for whitespace (standard)
    if (tokens.size() != 6)
    {
        throw new std::invalid_argument("Unable to generate 6 tokens from FEN string.");
    }

    Util::Initializers::update_position_from_fen_token_0(&new_position, tokens.at(0));
    Util::Initializers::update_position_from_fen_token_1(&new_position, tokens.at(1));
    Util::Initializers::update_position_from_fen_token_2(&new_position, tokens.at(2));
    Util::Initializers::update_position_from_fen_token_3(&new_position, tokens.at(3));
    Util::Initializers::update_position_from_fen_token_4(&new_position, tokens.at(4));
    Util::Initializers::update_position_from_fen_token_5(&new_position, tokens.at(5));

    return new_position;
}

void Util::Initializers::update_position_from_fen_token_0(Game::Position *position, std::string token)
{
    std::string::iterator element;
    uint64_t square = 1;

    for (element = token.begin(); element != token.end(); element++)
    {
        if (*element == '/')
        {
            continue;
        }

        if (isdigit(*element))
        {
            square <<= int(*element) - 48;
        }
        else
        {
            if (isupper(*element))
            {
                position->white |= square;
            }
            else
            {
                position->black |= square;
            }

            switch (tolower(*element))
            {
            case 'k':
                position->kings |= square;
                break;
            case 'q':
                position->queens |= square;
                break;
            case 'r':
                position->rooks |= square;
                break;
            case 'b':
                position->bishops |= square;
            case 'n':
                position->knights |= square;
                break;
            case 'p':
                position->pawns |= square;
                break;
            default:
                throw new std::invalid_argument("Invalid value in first FEN token.");
            }
            square <<= 1;
        }
    }
}

void Util::Initializers::update_position_from_fen_token_1(Game::Position *position, std::string token)
{
    position->white_to_move = token == "w";
}

void Util::Initializers::update_position_from_fen_token_2(Game::Position *position, std::string token)
{
    std::string::iterator element;

    for (element = token.begin(); element != token.end(); element++)
    {
        switch (*element)
        {
        case 'K':
            position->white_can_castle_kingside = true;
            break;
        case 'Q':
            position->white_can_castle_queenside = true;
            break;
        case 'k':
            position->black_can_castle_kingside = true;
            break;
        case 'q':
            position->black_can_castle_queenside = true;
            break;
        }
    }
}

void Util::Initializers::update_position_from_fen_token_3(Game::Position *position, std::string token)
{
    std::string::iterator element;

    for (element = token.begin(); element != token.end(); element++)
    {
        if (*element == '-')
        {
            position->enPassantTarget = 0;
        }
        else
        {
            position->enPassantTarget = Util::Lookup::coordinate_to_uint64_t(token);
        }
    }
}

void Util::Initializers::update_position_from_fen_token_4(Game::Position *position, std::string token)
{
    // Integers as char start at 0 = 48
    position->half_move_clock = token.at(0) - 48;
}

void Util::Initializers::update_position_from_fen_token_5(Game::Position *position, std::string token)
{
    // Integers as char start at 0 = 48
    position->full_move_clock = token.at(0) - 48;
}
