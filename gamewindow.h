#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H
/* gamewindow controls logic of the whole game
 * in a solitaire game, the number of cards in the gui scene is always 52, so at the start of each game, we add all the
 * front image and back image of each card into scene, and use 2 QList to store the pointers.
 * then we create a card list to store all the 52 cards, and when initializing each card, we use a data member in Card class
 * to store the corresponding index of images in QList, and then do shuffling.
 * the benefit is that when doing refreshing of gui, we only need to go through all the cards in the card list, then manage
 * the images in scene according to card status, which simplifies the logic.
 *
 * the function of deck and pile is to store cards and manage the movement between two structures
 *
 * to achieve the undo function, each time a valid movement occurs, we use two stack structures in the card class to record
 * the position and status of each card. when using undo, the stack pop the back element and return to last step.
 * since this function kind of breaks the game logic, the pile and deck structure can not manage the card moement well, so we
 * just clear the structures and add cards back like doing initializing.
 *
 * with the undo function, we can easily jump to the swap or reveal scene and then use undo to jump back, without affecting
 * the normal game logic.
 */
#include <QMainWindow>
#include <QGraphicsScene>

#include "card.h"
#include "clickableview.h"
#include "deck.h"
#include "pile.h"


#include <string>
#include <vector>
using std:: vector;
using std::string;
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QList>

QT_BEGIN_NAMESPACE
namespace Ui { class GameWindow; }
QT_END_NAMESPACE

const int ROW_LENGTH = 113;
const int COL_LENGTH = 147;

class GameWindow : public QMainWindow
{
    Q_OBJECT

public:
    GameWindow(QWidget *parent = nullptr);
    ~GameWindow();

    // pictures
    const QString card_image_lookup[4][13]
    {
        {
            ":/images/cards/AS.png",
            ":/images/cards/2S.png",
            ":/images/cards/3S.png",
            ":/images/cards/4S.png",
            ":/images/cards/5S.png",
            ":/images/cards/6S.png",
            ":/images/cards/7S.png",
            ":/images/cards/8S.png",
            ":/images/cards/9S.png",
            ":/images/cards/10S.png",
            ":/images/cards/JS.png",
            ":/images/cards/QS.png",
            ":/images/cards/KS.png"
        },
        {
            ":/images/cards/AH.png",
            ":/images/cards/2H.png",
            ":/images/cards/3H.png",
            ":/images/cards/4H.png",
            ":/images/cards/5H.png",
            ":/images/cards/6H.png",
            ":/images/cards/7H.png",
            ":/images/cards/8H.png",
            ":/images/cards/9H.png",
            ":/images/cards/10H.png",
            ":/images/cards/JH.png",
            ":/images/cards/QH.png",
            ":/images/cards/KH.png"
        },
        {
            ":/images/cards/AD.png",
            ":/images/cards/2D.png",
            ":/images/cards/3D.png",
            ":/images/cards/4D.png",
            ":/images/cards/5D.png",
            ":/images/cards/6D.png",
            ":/images/cards/7D.png",
            ":/images/cards/8D.png",
            ":/images/cards/9D.png",
            ":/images/cards/10D.png",
            ":/images/cards/JD.png",
            ":/images/cards/QD.png",
            ":/images/cards/KD.png"
        },
        {
            ":/images/cards/AC.png",
            ":/images/cards/2C.png",
            ":/images/cards/3C.png",
            ":/images/cards/4C.png",
            ":/images/cards/5C.png",
            ":/images/cards/6C.png",
            ":/images/cards/7C.png",
            ":/images/cards/8C.png",
            ":/images/cards/9C.png",
            ":/images/cards/10C.png",
            ":/images/cards/JC.png",
            ":/images/cards/QC.png",
            ":/images/cards/KC.png"
        },
    };

    const QString card_icons[5]
    {
        ":/images/cards/spade.png",
        ":/images/cards/heart.png",
        ":/images/cards/diamond.png",
        ":/images/cards/club.png",
        ":/images/cards/poker.png"
    };

    const QString card_back_image{":/images/cards/blue_back.png"};


private slots:
    void on_actionNew_Game_triggered();
    void map_clicked(int row, int col);
    void on_actionUndo_triggered();
    void on_actionHint_triggered();
    void on_actionReveal_triggered();
    void on_actionFinish_Reveal_triggered();
    void on_actionSwap_triggered();
    void on_actionCancel_Swap_triggered();

private:
    enum Mode {EASY, HARD};

    void clear_last_game(); // reset all datas
    void load_card(); // generate 52 cards to card_list
    void shuffle_card(); // messup the order of the cards
    void render_and_print_gui(); // refresh image position
    void indicate_card_selected(int row, int col, int num); // after selecting a card, a purple rectangle will occur to indicate whtich card is been selected
    void reset_card_indicator(); // clear the indicator
    int get_card_index_by_pos(int row, int col); // find a card according to clicking position
    bool is_game_over() const; // check if game is over
    void swap_the_card(int row, int col); //for swap

    void record_cards(); // record position and status of all cards

    bool find_card_in_hint(Card* card) const; // find if a card is hinted

    Deck* stock {nullptr};
    Deck* waste {nullptr};
    Deck* foundations[4];
    Pile* tableau_piles[7];

    QList<QGraphicsPixmapItem*> card_front_images;
    QList<QGraphicsPixmapItem*> card_back_images;
    QList<QGraphicsItem*> position_images;

    QGraphicsRectItem* map_indicator {nullptr};

    std::vector<Card*> card_list; // 52 cards
    std::vector<Card*> revealed_card_record; // record cards that is revealed and later cover them
    std::list<Card*> hint_record; // hinted card record, to stop hinting the same card repeatedly

    QGraphicsScene scene;

    int active_card_index{-1};
    int command_stage{-1}; // -1 hasnt started the game. 0 wait for selecting a card, 1 wait for selecting a target palce. 2 for waiting a swap cardd

    int steps{0}; // for undo, record how many steps have occured
    int swap_time{0};//only 3 chances to do swap
    bool reveal_used = false; // reveal should only be able to used once

    std::vector<Card*> swap_card; //for swap

    Mode gamemode;

private:
    Ui::GameWindow *ui;
};
#endif // GAMEWINDOW_H
