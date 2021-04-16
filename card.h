#ifndef CARD_H
#define CARD_H
/* Card is the basic component of this game
 * It contains the basic information of a card (suit, rank, color, position, revealed status，image, history information)
 */
#include<iostream>
using namespace std;
#include <list>
#include <vector>
#include <stack>

class Card
{
public:

    enum Suit {SPADES, HEARTS, DIAMONDS, CLUBS};

    enum Color {RED, BLACK};

    Card(Suit suit, int rank);

    ~Card(){}

    void set_pos(int row, int col); // set the position of this card
    void set_zvalue(int value); // set the height of this card

    Suit get_suit() const; // get the suit of this card
    Color get_color() const; // get the color e.g. BLACK/RED of this color
    int get_rank() const; // get the rank of this card e.g. A 1 2 ... J Q K
    bool is_revealed() const; // return if this card is revealed
    void reveal_the_card(); // reveal this card e.g. set image to front
    void cover_the_card(); // cover this card e.g. set image to back
    int get_row() const; // get position_row
    int get_col() const; // get_position_col
    int get_image_index() const; // get
    int get_zvalue() const; // get the height

    void record_the_current_pos_and_status(); // for undo, record history position and reveal status
    void return_to_the_last_pos_and_status(); // for undo, return to last position and reveal status

    // for swap, change the content (rank, suit, color, image) of two cards instead of actually swapping them
    void reset_card(Suit suit, Color color, int rank, int image_index);

private:

    //basic informations
    Suit suit;
    Color color;
    int rank;
    bool revealed = false;
    int position_row{0}, position_col{0};
    int image_index;   //the front image address in card_image_lookup
    int zvalue{0};

    // for undo, history information of this card
    stack<vector<int>> history_position;
    stack<bool> history_status;
};

#endif // CARD_H
