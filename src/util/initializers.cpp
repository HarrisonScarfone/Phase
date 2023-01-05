#include <iostream>
#include <cstdint>

#include "initializers.hpp"
#include "util.hpp"

Game::Position Util::Initializers::fen_string_to_position(std::string fen_string)
{
    Game::Position new_position;
    std::vector<std::string> tokens = Util::tokenize_string_by_whitespace(fen_string);

    if (tokens.size() != 5)
    {
        throw new std::invalid_argument("Unable to generate 5 tokens from FEN string.");
    }

    Util::Initializers::update_position_from_fen_token_0(&new_position, tokens.at(0));
    // update_position_from_fen_token_1(&new_position, tokens.at(1));
    // update_position_from_fen_token_2(&new_position, tokens.at(2));
    // update_position_from_fen_token_3(&new_position, tokens.at(3));
    // update_position_from_fen_token_4(&new_position, tokens.at(4));

    return new_position;
}

void Util::Initializers::update_position_from_fen_token_0(Game::Position *position, std::string token)
{
    std::string::iterator element;
    uint64_t square = 1;

    for (element = token.begin(); element != token.end(); element++)
    {
        if (isdigit(*element))
        {
            square <<= int(*element) - 48;
        }
        else
        {
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
            case '/':
                continue;
            default:
                throw new std::invalid_argument("Invalid value in first FEN token.");
            }
            square <<= 1;
        }
    }
}
