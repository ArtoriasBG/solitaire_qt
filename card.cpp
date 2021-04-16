#include "card.h"

Card::Card(Suit suit, int rank): suit(suit), rank(rank) // constructor, set color according to suit
{
    if (suit == Suit::SPADES || suit == Suit::CLUBS)
    {
        color = Color::BLACK;
    }
    else
    {
        color = Color::RED;
    }

    if (suit == Suit::SPADES)
    {
        image_index = rank - 1;
    }
    else if (suit == Suit::HEARTS)
    {
        image_index = rank + 12;
    }
    else if (suit == Suit::DIAMONDS)
    {
        image_index = rank + 25;
    }
    else
    {
        image_index = rank + 38;
    }

    record_the_current_pos_and_status();
}

Card::Suit Card::get_suit() const
{
    return suit;
}


Card::Color Card::get_color() const
{
    return color;
}


int Card::get_rank() const
{
    return rank;
}

bool Card::is_revealed() const
{
    return revealed;
}

void Card::reveal_the_card()
{
    revealed = true;
}

void Card::cover_the_card()
{
    revealed = false;
}

int Card::get_row() const
{
    return position_row;
}

int Card::get_col() const
{
    return position_col;
}

int Card::get_image_index() const
{
    return image_index;
}


void Card::record_the_current_pos_and_status()
{
    vector<int> to_be_pushed(0); // store the position(row, col, height) in a vector<int>(3)
    to_be_pushed.push_back(position_row);
    to_be_pushed.push_back(position_col);
    to_be_pushed.push_back(zvalue);
    history_position.push(to_be_pushed);
    history_status.push(is_revealed());
}

void Card::return_to_the_last_pos_and_status() // pop current data, current = last
{
    if (history_position.size() <= 1) return; // starting status, cant undo
    history_position.pop();
    history_status.pop();
    position_row = history_position.top()[0];
    position_col = history_position.top()[1];
    zvalue = history_position.top()[2];
    revealed = history_status.top();
}

void Card::set_pos(int row, int col)
{
    position_row = row;
    position_col = col;
}

int Card::get_zvalue() const
{
    return zvalue;
}

void Card::set_zvalue(int value)
{
    zvalue = value;
}

void Card::reset_card(Suit suit, Color color, int rank, int image_index) // swap information with another card
{
    this->suit = suit;
    this->color = color;
    this->rank = rank;
    this->image_index = image_index;
}


