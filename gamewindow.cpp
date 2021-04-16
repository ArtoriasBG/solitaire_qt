#include "gamewindow.h"
#include "ui_gamewindow.h"

#include <QDebug>
#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QString>
#include <iostream>
#include <random>
using namespace std;

#include "clickableview.h"

GameWindow::GameWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::GameWindow)
{
    // set up the preparing stage UI

    this->setWindowIcon(QPixmap(":/images/cards/spade.png"));

    ui->setupUi(this);

    ui->graphicsView->setScene(&scene);

    QColor color;
    color.setGreen(180);
    scene.setBackgroundBrush(color);

    ui->graphicsView->show();

    connect(ui->graphicsView, &ClickableView::mouseClicked, this, &GameWindow::map_clicked);
    for (int i = 0; i != 7; ++i)
    {
        if (i <= 3)
            foundations[i] = nullptr;
        tableau_piles[i] = nullptr;
    }
    ui->actionFinish_Reveal->setVisible(false);
    ui->actionHint->setVisible(false);
    ui->actionReveal->setVisible(false);
    ui->actionSwap->setVisible(false);
    ui->actionUndo->setVisible(false);
    ui->actionCancel_Swap->setVisible(false);
}

GameWindow::~GameWindow()
{
    delete ui;
}

void GameWindow::load_card()
{

    QSize picSize(COL_LENGTH, 2 * ROW_LENGTH); // size of each image

    QPen pen(Qt::green, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

    // set up the foundations and stock position image
    QGraphicsPixmapItem *pix_item = scene.addPixmap(QPixmap(card_icons[4]).scaled(picSize, Qt::KeepAspectRatio));
    QGraphicsRectItem *rect = scene.addRect(0, 0, COL_LENGTH, 2 * ROW_LENGTH, pen);
    position_images.push_back(pix_item);
    position_images.push_back(rect);

    pix_item->setOffset(0, 40);
    pix_item->setZValue(-520);

    for (int i = 0; i < 4; i++)
    {
        QGraphicsPixmapItem *pix_item = scene.addPixmap(QPixmap(card_icons[i]).scaled(picSize, Qt::KeepAspectRatio));
        QGraphicsRectItem *rect = scene.addRect(16 * COL_LENGTH, 4 * i * ROW_LENGTH, COL_LENGTH, 2 * ROW_LENGTH, pen);
        position_images.push_back(pix_item);
        position_images.push_back(rect);
        pix_item->setOffset(16 * COL_LENGTH, 4 * i * ROW_LENGTH + 40);
        pix_item->setZValue(-1);
    }

    //  initialize and  add all the cards to the card list respectively
    for (int i = 0; i < 4; ++i)
    {
        Card::Suit suit;

        if (i == 0)
        {
            suit = Card::SPADES;
        }
        else if (i == 1)
        {
            suit = Card::HEARTS;
        }
        else if (i == 2)
        {
            suit = Card::DIAMONDS;
        }
        else
        {
            suit = Card::CLUBS;
        }

        for (int j = 0; j < 13; j++)
        {
            QGraphicsPixmapItem *pix_item_front = scene.addPixmap(QPixmap(card_image_lookup[i][j]).scaled(picSize, Qt::KeepAspectRatio));
            QGraphicsPixmapItem *pix_item_back = scene.addPixmap(QPixmap(card_back_image).scaled(picSize, Qt::KeepAspectRatio));

            card_front_images.push_back(pix_item_front);
            card_back_images.push_back(pix_item_back);

            Card *new_card = new Card(suit, j + 1);
            card_list.push_back(new_card);
        }
    }

    shuffle_card();

    // add certain number of cards into stock, waste, foundations respectively
    int card_index = 0;

    stock = new Deck(Deck::DeckType::STOCK);

    for (; card_index < 24; card_index++)
    {
        stock->addCard(card_list[card_index]);
    }

    waste = new Deck(Deck::DeckType::WASTE);

    for (int i = 0; i < 4; i++)
    {
        foundations[i] = new Deck(Deck::DeckType::FOUNDATION, i);
    }

    for (int j = 0; j < 7; j++)
    {
        tableau_piles[j] = new Pile(j);
        for (int k = -1; k < j; k++)
        {
            tableau_piles[j]->initialize_card(card_list[card_index]);
            if (k == j - 1)
            {
                card_list[card_index]->reveal_the_card();
            }
            card_index++;
        }
        tableau_piles[j]->refresh_cards_pos();
    }
    render_and_print_gui();
    record_cards();
}

// shuffle the cards in the card list
void GameWindow::shuffle_card()
{
    static std::random_device rng;

    static std::mt19937 urng(rng());

    std::shuffle(card_list.begin(), card_list.end(), urng);
}

// refresh the gui according to the position and
void GameWindow::render_and_print_gui()
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 13; j++)
        {
            Card *current_card = card_list[i * 13 + j];
            int index = current_card->get_image_index();

            QGraphicsPixmapItem *pix_item_front = card_front_images[index];

            QGraphicsPixmapItem *pix_item_back = card_back_images[index];

            if (current_card->is_revealed())
            {
                pix_item_front->setVisible(true);
                pix_item_front->setZValue(current_card->get_zvalue());
                pix_item_front->setOffset(current_card->get_col() * COL_LENGTH, current_card->get_row() * ROW_LENGTH);

                pix_item_back->setVisible(false);
            }
            else
            {
                pix_item_back->setVisible(true);
                pix_item_back->setZValue(current_card->get_zvalue());
                pix_item_back->setOffset(current_card->get_col() * COL_LENGTH, current_card->get_row() * ROW_LENGTH);

                pix_item_front->setVisible(false);
            }
        }
    }
    scene.views()[0]->fitInView(scene.itemsBoundingRect(), Qt::KeepAspectRatio);
}

void GameWindow::clear_last_game()
{
    delete stock;
    delete waste;
    for (int i = 0; i < 4; i++)
        delete foundations[i];
    for (int j = 0; j < 7; j++)
        delete tableau_piles[j];
    ;
    delete map_indicator;
    card_front_images.clear();
    card_back_images.clear();
    position_images.clear();
    card_list.clear();
    scene.clear();
    hint_record.clear();
    reveal_used = false;
    steps = 0;
    swap_time = 0;
}

// indicate the card(s) selected
void GameWindow::indicate_card_selected(int row, int col, int num_of_card)
{
    QPen pen(Qt::magenta, 6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

    int row_length = ROW_LENGTH;
    if (waste->contain(card_list[active_card_index]))
    {
        switch (card_list[active_card_index]->get_row())
        {
        case 4:
            if (get_card_index_by_pos(5, 0) != -1 && card_list[get_card_index_by_pos(5, 0)]->get_zvalue() != 0)
                row_length /= 2;
            break;
        case 5:
            if (get_card_index_by_pos(6, 0) != -1 && card_list[get_card_index_by_pos(6, 0)]->get_zvalue() != 1)
                row_length /= 2;
            break;
        default:
            break;
        }
    }
    map_indicator = scene.addRect(col * COL_LENGTH, row * ROW_LENGTH, COL_LENGTH, (num_of_card + 1) * row_length, pen);

    map_indicator->setZValue(15);
}

// reset indicator
void GameWindow::reset_card_indicator()
{
    delete map_indicator;
    map_indicator = nullptr;
}

// get the active card index in the shuffled card list according to the row and col
int GameWindow::get_card_index_by_pos(int row, int col)
{
    if (row == 0 && col == 0)
        return -1; // clicking stock
    vector<int> possible(0);
    for (unsigned i = 0; i != card_list.size(); ++i)
        if (card_list[i]->get_col() == col && (card_list[i]->get_row() == row || card_list[i]->get_row() + 1 == row))
            possible.push_back(i);

    // get the card with largest zvalue
    int result = (possible.empty() ? -1 : possible[0]);
    for (unsigned i = 0; i != possible.size(); ++i)
        if (card_list[possible[i]]->get_zvalue() > card_list[result]->get_zvalue())
            result = possible[i];

    return result;
}

// the handler of map click event
void GameWindow::map_clicked(int row, int col)
{
    switch (command_stage)
    {
    case 2:
        swap_the_card(row, col);
        return;
    case -1:
        return; // game not started yet
    case 0:     // wait for selecting a card
        reset_card_indicator();
        render_and_print_gui();
        // clicking on stock when not selecting any card, draw cards
        if ((row == 0 || row == 1) && col == 0)
        {
            switch (gamemode)
            {
            case EASY:
                // return all cards from waste to stock
                for (int i = 0; i != 3; ++i)
                {
                    stock->addCard(waste->top_card());
                    waste->removeCard();
                }
                // draw cards from stock to waste
                for (int i = 0; i != 3; ++i)
                {
                    waste->addCard(stock->top_card());
                    stock->removeCard();
                }
                break;
            case HARD: // similar to easy mode
                for (int i = 0; i != 2; ++i)
                {
                    stock->addCard(waste->top_card());
                    waste->removeCard();
                }
                for (int i = 0; i != 2; ++i)
                {
                    waste->addCard(stock->top_card());
                    stock->removeCard();
                }
                break;
            }
            hint_record.clear();
            render_and_print_gui();
            record_cards();
        }

        // clicking on a waste cards
        else if (col == 0)
        {
            int clicked_card_index = get_card_index_by_pos(row, col);
            if ((gamemode == Mode::HARD && (waste->top_card() != card_list[clicked_card_index])) || clicked_card_index == -1)
                return; //not selecting the top one in hard mode, return
            active_card_index = clicked_card_index;
            indicate_card_selected(card_list[active_card_index]->get_row(), card_list[active_card_index]->get_col(), 1);
            command_stage = 1;
        }

        // clicking on a foundation card
        else if (col == 16)
        {
            int clicked_card_index = get_card_index_by_pos(row, col);
            if (clicked_card_index == -1)
                return;
            active_card_index = get_card_index_by_pos(row, col);
            indicate_card_selected(card_list[active_card_index]->get_row(), card_list[active_card_index]->get_col(), 1);
            command_stage = 1;
        }

        // clicking on a tableau card
        else
        {
            int clicked_card_index = get_card_index_by_pos(row, col);
            if (clicked_card_index == -1 || !card_list[clicked_card_index]->is_revealed())
                return; // clicking on a not revealed card
            active_card_index = clicked_card_index;
            int num = 0;
            for (int i = 0; i != 7; ++i)
            {
                if (tableau_piles[i]->contain(card_list[active_card_index]))                     // see which pile contains the card
                    num = tableau_piles[i]->num_of_selected_cards(card_list[active_card_index]); // get the number of following cards
            }
            indicate_card_selected(card_list[active_card_index]->get_row(), card_list[active_card_index]->get_col(), num);
            command_stage = 1;
        }
        return;
    case 1: // selected a card, waiting for its destination
        // clicking on any invalid place, cancel selection
        bool is_tableau_top = false;
        for (int i = 0; i != 7; ++i) {
            if (tableau_piles[i]->top_card() == nullptr) {
                if ((row == tableau_piles[i]->get_base_row() || row == tableau_piles[i]->get_base_row() + 1) && col == tableau_piles[i]->get_base_col()) {
                    is_tableau_top = true;
                    break;
                }
            }
            else if ((row == tableau_piles[i]->top_card()->get_row() || row == tableau_piles[i]->top_card()->get_row() + 1)
                     && col == tableau_piles[i]->top_card()->get_col()) {
                is_tableau_top = true;
                break;
            }
        }
        bool is_foundation_top = false;
        if (col == 16) {
            if (row == 0 || row == 1 || row == 4 || row == 5 || row == 8 || row == 9 || row == 12 || row == 13 || row == 16 || row == 17)
                is_foundation_top = true;
        }
        // clicking on a tableau top || clicking on foundation
        if (!is_tableau_top && !is_foundation_top) {
            command_stage = 0;
            active_card_index = -1;
            reset_card_indicator();
            render_and_print_gui();
            return;
        }
/*
        if (col != 16) {
            // selecting k and clicking an pile base
            bool is_pile_base {false};
            for (int i = 0; i != 7; ++i) {
                if ((row == foundations[i]->get_base_row() || row == foundations[i]->get_base_row() + 1) && col == foundations[i]->get_base_col()) {
                    is_pile_base = true;
                    break;
                }
            }
            if ((get_card_index_by_pos(row, col) == -1 || !card_list[get_card_index_by_pos(row,col)]->is_revealed() || col == 0)
                    && !is_pile_base)
            {
                command_stage = 0;
                active_card_index = -1;
                reset_card_indicator();
                render_and_print_gui();
                return;
            }
        }
*/

        // click on itself to cancel selection
        if (active_card_index == get_card_index_by_pos(row, col))
        {
            active_card_index = -1;
            command_stage = 0;
            reset_card_indicator();
            return;
        }
/*
        // clicking on empty space or covered card
        if (card_list[active_card_index]->get_rank() == 13)
        {
            if (col % 2 == 1 || col == 0) return;
            if (get_card_index_by_pos(row, col) == -1)
            {
                if (row != 1 && row != 0) return;
            }
        }

        else if ((get_card_index_by_pos(row, col) == -1 && col != 16) || col == 0)
        {
            return;
        }

        if (col == 16)
        {
            bool on_foundation = false;

            for (int i = 0; i < 4; i++)
            {
                if (row == foundations[i]->get_base_row() || row == foundations[i]->get_base_row() + 1)
                {
                    on_foundation = true;
                    break;
                }
            }
            if (on_foundation == false)
            {
                return;
            }
        }
        if (get_card_index_by_pos(row, col) != -1)
        {
            if (!card_list[get_card_index_by_pos(row, col)]->is_revealed())
            {
                return;
            }
            if (col != 16)
            {
                if (card_list[get_card_index_by_pos(row, col)] != tableau_piles[(col - 1) / 2]->top_card())
                {
                    return;
                }
            }
        }
*/
        int position = -1; // 0-6 pile, 7-10, foundation, -1 waste
        for (int i = 0; i != 7 && position == -1; ++i)
        {
            if (tableau_piles[i]->contain(card_list[active_card_index]))
                position = i;
        }
        for (int i = 0; i != 4 && position == -1; ++i)
        {
            if (foundations[i]->contain(card_list[active_card_index]))
                position = 7 + i;
        }

        // new selection on foundations
        for (int i = 0; i != 4; ++i)
        {
            if ((row == foundations[i]->get_base_row() || row == foundations[i]->get_base_row() + 1) && col == foundations[i]->get_base_col())
            {
                Deck *target = foundations[i];

                if (position >= 7)
                    return;
                // move from pile to foundation
                else if (position >= 0 && position <= 6)
                {
                    if (!tableau_piles[position]->moveable(target, card_list[active_card_index]))
                        return;
                    tableau_piles[position]->move_cards_to(target, card_list[active_card_index]);
                }

                // move from waste to foundation
                else if (position == -1)
                {
                    bool suit_match = card_list[active_card_index]->get_suit() == target->get_foundation_index();
                    bool rank_match = target->isEmpty() ? (card_list[active_card_index]->get_rank() == 1)
                                                        : (card_list[active_card_index]->get_rank() == target->top_card()->get_rank() + 1);
                    if (!suit_match || !rank_match)
                        return;
                    waste->removeCard(card_list[active_card_index]);
                    target->addCard(card_list[active_card_index]);
                }
                break;
            }
        }
        // new selection on the next place of a pile
        for (int i = 0; i != 7 && command_stage == 1; ++i)
        {
            if (tableau_piles[i]->top_card() == nullptr ? ((row == tableau_piles[i]->get_base_row() || row == tableau_piles[i]->get_base_row() + 1 || row == tableau_piles[i]->get_base_row() + 2) && col == tableau_piles[i]->get_base_col())
                                                        : ((row == tableau_piles[i]->top_card()->get_row() || row == tableau_piles[i]->top_card()->get_row() + 1 || row == tableau_piles[i]->top_card()->get_row() + 2) && col == tableau_piles[i]->top_card()->get_col()))
            {

                Pile *target = tableau_piles[i];
                // move from pile to pile
                if (position >= 0 && position <= 6)
                {
                    if (!tableau_piles[position]->moveable(target, card_list[active_card_index]))
                        return;
                    tableau_piles[position]->move_cards_to(target, card_list[active_card_index]);
                }

                // move from waste to pile
                else if (position == -1)
                {
                    bool color_match = (target->top_card() == nullptr) ? true
                                                                       : card_list[active_card_index]->get_color() != target->top_card()->get_color();
                    bool rank_match = (target->top_card() == nullptr) ? card_list[active_card_index]->get_rank() == 13
                                                                      : card_list[active_card_index]->get_rank() + 1 == target->top_card()->get_rank();

                    if (!color_match || !rank_match)
                        return;
                    waste->removeCard(card_list[active_card_index]);
                    target->add_single_card(card_list[active_card_index]);
                }

                // move from foundation to pile
                else
                {

                    bool color_match = (target->top_card() == nullptr) ? true
                                                                       : card_list[active_card_index]->get_color() != target->top_card()->get_color();
                    bool rank_match = (target->top_card() == nullptr) ? card_list[active_card_index]->get_rank() == 13
                                                                      : card_list[active_card_index]->get_rank() + 1 == target->top_card()->get_rank();
                    if (!color_match || !rank_match)
                        return;
                    foundations[position - 7]->removeCard();
                    target->add_single_card(card_list[active_card_index]);
                }
                break;
            }
        }
        // not clicking on any valid place, cancel selection
        active_card_index = -1;
        command_stage = 0;
        reset_card_indicator();
        render_and_print_gui();
        if (is_game_over())
        {
            command_stage = -1;
            for (int i = 0; i != 52; ++i)
            {
                if (card_list[i]->get_rank() == 13)
                {
                    card_list[i]->set_zvalue(1);
                    switch (card_list[i]->get_suit())
                    {
                    case Card::SPADES:
                        card_list[i]->set_pos(0, 16);
                        break;
                    case Card::HEARTS:
                        card_list[i]->set_pos(4, 16);
                        break;
                    case Card::DIAMONDS:
                        card_list[i]->set_pos(8, 16);
                        break;
                    case Card::CLUBS:
                        card_list[i]->set_pos(12, 16);
                        break;
                    }
                }
                else
                {
                    card_list[i]->set_pos(0, 16);
                    card_list[i]->set_zvalue(0);
                }
            }
            render_and_print_gui();
            QMessageBox::information(this, "Congratulations!", "You Win!");
        }
        record_cards();
    }
}

void GameWindow::on_actionNew_Game_triggered()
{
    clear_last_game();
    // show a window to select difficulty
    int ret = QMessageBox::information(this, "Game Mode",
                                       QStringLiteral("Select Game Difficulty"), QStringLiteral("EASY"), QStringLiteral("HARD"));
    switch (ret)
    {
    case 0:
        gamemode = EASY;
        break;
    case 1:
        gamemode = HARD;
        break;
    default:
        break;
    }

    load_card();
    command_stage = 0;
    ui->actionHint->setVisible(true);
    ui->actionReveal->setVisible(true);
    ui->actionSwap->setVisible(true);
    ui->actionUndo->setVisible(true);
}

bool GameWindow::is_game_over() const
{

    if (!waste->isEmpty())
        return false;
    for (int i = 0; i < 52; i++)
        if (!card_list[i]->is_revealed())
            return false;
    return true;
}

void GameWindow::record_cards()
{
    for (auto it = card_list.begin(); it != card_list.end(); ++it)
        (*it)->record_the_current_pos_and_status();
    steps++;
}

void GameWindow::on_actionUndo_triggered()
{

    if (command_stage == 1 || command_stage == -1 || steps == 1)
    {
        return;
    }

    steps--;

    for (auto it = card_list.begin(); it != card_list.end(); ++it)
        (*it)->return_to_the_last_pos_and_status();
    stock->clear_deck();
    waste->clear_deck();
    for (int i = 0; i != 7; ++i)
    {
        if (i < 4)
            foundations[i]->clear_deck();
        tableau_piles[i]->clear_pile();
    }
    // put cards into piles
    vector<Card *> stock_cards(0);
    vector<Card *> waste_cards(0);
    vector<Card *> foundation_cards(0);
    vector<Card *> pile_cards(0);

    for (auto it = card_list.begin(); it != card_list.end(); ++it)
    {
        // cards belong to stock
        if ((*it)->get_row() == 0 && (*it)->get_col() == 0)
        {
            stock_cards.push_back(*it);
        }
        // cards belong to foundation
        else if ((*it)->get_col() == 16)
        {
            foundation_cards.push_back(*it);
        }
        // cards belong to waste
        else if ((*it)->get_col() == 0)
        {
            waste_cards.push_back(*it);
        }
        // cards belong to piles
        else
        {
            pile_cards.push_back(*it);
        }
    }

    // for pile, max 20 rows
    for (int i = 0; i < 20; i++)
    {
        for (auto it = pile_cards.begin(); it != pile_cards.end(); it++)
        {
            if ((*it)->get_row() == i)
            {
                tableau_piles[(*it)->get_col() / 2 - 1]->initialize_card((*it));
            }
        }
    }

    // for waste, max 3 cards
    for (int i = 4; i < 7; i++)
    {
        for (auto it = waste_cards.begin(); it != waste_cards.end(); it++)
        {
            if ((*it)->get_row() == i)
            {
                waste->initialize_card((*it));
            }
        }
    }

    //for foundation, max 13 cards
    for (int i = 1; i <= 13; i++)
    {
        for (auto it = foundation_cards.begin(); it != foundation_cards.end(); it++)
        {
            if ((*it)->get_rank() == i)
            {
                foundations[(*it)->get_suit()]->initialize_card((*it));
            }
        }
    }

    //for stock, add card according to the zvalue
    for (unsigned i = 0; i < stock_cards.size(); i++)
    {
        for (unsigned j = i + 1; j < stock_cards.size(); j++)
        {
            if (stock_cards[i]->get_zvalue() > stock_cards[j]->get_zvalue())
            {
                Card *temp = stock_cards[i];
                stock_cards[i] = stock_cards[j];
                stock_cards[j] = temp;
            }
        }
    }

    for (auto it = stock_cards.begin(); it != stock_cards.end(); it++)
    {
        stock->initialize_card((*it));
    }

    render_and_print_gui();
}

bool GameWindow::find_card_in_hint(Card *card) const
{
    for (auto it = hint_record.begin(); it != hint_record.end(); ++it)
    {
        if (card == (*it))
            return true;
    }
    return false;
}

void GameWindow::on_actionHint_triggered()
{
    if (command_stage == -1)
        return;
    //cancel current selection
    reset_card_indicator();
    command_stage = 0;
    active_card_index = -1;

    // find movable card in piles
    for (int i = 0; i != 7; ++i)
    {
        const list<Card *> &current_pile_cards = tableau_piles[i]->get_cards();
        for (auto it = current_pile_cards.begin(); it != current_pile_cards.end(); ++it)
        {
            if ((*it)->is_revealed())
            {
                if ((*it)->get_row() == 0 && (*it)->get_rank() == 13)
                    continue;
                if (find_card_in_hint(*it))
                    continue;
                // check if this card can be moved to a foundation
                for (int j = 0; j != 4; ++j)
                {
                    if (tableau_piles[i]->moveable(foundations[j], *it))
                    {
                        for (unsigned index = 0; index != card_list.size(); ++index)
                            if (card_list[index] == (*it))
                            {
                                active_card_index = index;
                                break;
                            }
                        hint_record.push_back(*it);
                        command_stage = 1;
                        indicate_card_selected((*it)->get_row(), (*it)->get_col(), tableau_piles[i]->num_of_selected_cards(*it));
                        render_and_print_gui();
                        return;
                    }
                }

                // check if *it card can be moved to another pile
                for (int j = 0; j != 7; ++j)
                {
                    if (i == j)
                        continue;
                    if (tableau_piles[i]->moveable(tableau_piles[j], *it))
                    {
                        hint_record.push_back(*it);
                        command_stage = 1;
                        for (unsigned index = 0; index != card_list.size(); ++index)
                            if (card_list[index] == (*it))
                            {
                                active_card_index = index;
                                break;
                            }
                        indicate_card_selected((*it)->get_row(), (*it)->get_col(), tableau_piles[i]->num_of_selected_cards(*it));
                        render_and_print_gui();
                        return;
                    }
                }
            }
        }
    }

    // find moveable card in waste
    const list<Card *> &current_cards = waste->get_cards();
    for (auto it = current_cards.begin(); it != current_cards.end(); ++it)
    {
        if (find_card_in_hint(*it))
            continue;
        if (gamemode == HARD && (*it) != waste->top_card())
            continue;
        // check if *it can be moved to foundation
        for (int i = 0; i != 4; ++i)
        {
            bool suit_match = (*it)->get_suit() == foundations[i]->get_foundation_index();
            bool rank_match = foundations[i]->isEmpty() ? ((*it)->get_rank() == 1)
                                                        : ((*it)->get_rank() == foundations[i]->top_card()->get_rank() + 1);
            if (rank_match && suit_match)
            {
                //previous_hint = *it;
                hint_record.push_back(*it);
                command_stage = 1;
                for (unsigned index = 0; index != card_list.size(); ++index)
                    if (card_list[index] == (*it))
                    {
                        active_card_index = index;
                        break;
                    }
                indicate_card_selected((*it)->get_row(), (*it)->get_col(), 1);
                render_and_print_gui();
                return;
            }
        }

        // check if can be moved to pile
        for (int i = 0; i != 7; ++i)
        {
            bool color_match = (tableau_piles[i]->top_card() == nullptr) ? true
                                                                         : (*it)->get_color() != tableau_piles[i]->top_card()->get_color();
            bool rank_match = (tableau_piles[i]->top_card() == nullptr) ? (*it)->get_rank() == 13
                                                                        : (*it)->get_rank() + 1 == tableau_piles[i]->top_card()->get_rank();
            if (color_match && rank_match)
            {
                hint_record.push_back(*it);
                command_stage = 1;
                for (unsigned index = 0; index != card_list.size(); ++index)
                    if (card_list[index] == (*it))
                    {
                        active_card_index = index;
                        break;
                    }
                indicate_card_selected((*it)->get_row(), (*it)->get_col(), 1);
                render_and_print_gui();
                return;
            }
        }
    }

    // find moveable card in foundation
    for (int i = 0; i != 4; ++i)
    {
        if (foundations[i]->top_card() == nullptr)
            continue;
        if (find_card_in_hint(foundations[i]->top_card()))
            continue;
        for (int j = 0; j != 7; ++j)
        {
            if (tableau_piles[j]->top_card() == nullptr)
                continue;
            bool color_match = (tableau_piles[i]->top_card() == nullptr) ? true
                                                                         : foundations[i]->top_card()->get_color() != tableau_piles[i]->top_card()->get_color();
            bool rank_match = (tableau_piles[i]->top_card() == nullptr) ? foundations[i]->top_card()->get_rank() == 13
                                                                        : foundations[i]->top_card()->get_rank() + 1 == tableau_piles[i]->top_card()->get_rank();
            if (color_match && rank_match)
            {
                for (unsigned index = 0; index != card_list.size(); ++index)
                    if (card_list[index] == foundations[i]->top_card())
                    {
                        active_card_index = index;
                        break;
                    }
                indicate_card_selected(foundations[i]->top_card()->get_row(), foundations[i]->top_card()->get_col(), 1);
                hint_record.push_back(card_list[active_card_index]);
                command_stage = 1;
                render_and_print_gui();
                return;
            }
        }
    }

    // can't find a moveable card, indicate the stock
    indicate_card_selected(0, 0, 1);
    render_and_print_gui();
}

// have one chance to reveal all the hidden cards in the current tableau, but do not record
//this status so can not return to this status by undo
// during reveal the cards are all read-only
void GameWindow::on_actionReveal_triggered()
{
    if (command_stage == -1)
        return;
    revealed_card_record.clear();
    active_card_index = -1;
    reset_card_indicator();
    for (int i = 0; i != 7; ++i)
    {
        for (auto it = tableau_piles[i]->get_cards().begin(); it != tableau_piles[i]->get_cards().end(); ++it)
        {
            if (!(*it)->is_revealed())
            {
                revealed_card_record.push_back(*it);
                (*it)->reveal_the_card();
            }
        }
    }
    render_and_print_gui();
    command_stage = -1;
    ui->actionFinish_Reveal->setVisible(true);
    ui->actionHint->setVisible(false);
    ui->actionReveal->setVisible(false);
    ui->actionSwap->setVisible(false);
    ui->actionUndo->setVisible(false);
    reveal_used = true;
}

// handle the end of reveal
void GameWindow::on_actionFinish_Reveal_triggered()
{
    command_stage = 0;
    for (auto it = revealed_card_record.begin(); it != revealed_card_record.end(); ++it)
        (*it)->cover_the_card();
    render_and_print_gui();
    QMessageBox::information(this, "Out of Reveal",
                             QStringLiteral("Can't reveal anymore :("), QStringLiteral("OK..."));

    ui->actionHint->setVisible(true);
    ui->actionUndo->setVisible(true);
    ui->actionFinish_Reveal->setVisible(false);

    if (swap_time != 3)
        ui->actionSwap->setVisible(true);
}

// 3 chances to swap one single card with any cards in the waste and stock
// or any hidden card in the tableau by interchanging the information of these
// two cards
void GameWindow::on_actionSwap_triggered()
{
    if (command_stage != 1)
        return;
    if (card_list[active_card_index]->get_col() == 0 || card_list[active_card_index]->get_col() == 16)
        return;

    if (card_list[active_card_index] != tableau_piles[(card_list[active_card_index]->get_col() - 1) / 2]->top_card())
        return;
    else
    {
        for (auto it = tableau_piles[(card_list[active_card_index]->get_col() - 1) / 2]->get_cards().begin();
             (*it) != card_list[active_card_index]; it++)
        {
            if ((*it)->is_revealed())
            {
                return;
            }
        }
    }
    for (int i = 0; i < 7; i++)
    {
        for (auto it = tableau_piles[i]->get_cards().begin(); it != tableau_piles[i]->get_cards().end(); it++)
        {
            if (!(*it)->is_revealed())
            {
                swap_card.push_back(*it);
            }
        }
    }

    for (auto it = stock->get_cards().begin(); it != stock->get_cards().end(); it++)
    {
        swap_card.push_back(*it);
    }

    for (auto it = waste->get_cards().begin(); it != waste->get_cards().end(); it++)
    {
        swap_card.push_back(*it);
    }

    for (int i = 0; i < 52; i++)
    {
        card_list[i]->set_pos(12, 16);
        card_list[i]->cover_the_card();
    }

    int location = 0;

    for (unsigned i = 0; i < swap_card.size(); i++, location++)
    {
        int location_y = location / 16;
        int location_x = location % 16;

        swap_card[i]->set_pos(2 * location_y, location_x);
        swap_card[i]->reveal_the_card();
    }

    record_cards();

    swap_card.clear();

    reset_card_indicator();

    render_and_print_gui();

    command_stage = 2;

    swap_time++;
    ui->actionHint->setVisible(false);
    ui->actionReveal->setVisible(false);
    ui->actionSwap->setVisible(false);
    ui->actionUndo->setVisible(false);
    ui->actionCancel_Swap->setVisible(true);
}

// must select one suitable card first to activate the swap function
// then a swap interface will be provided for the user to choose the card he wants
// will not record the swap status so can not swap the cards back by undo
void GameWindow::swap_the_card(int row, int col)
{
    int index = get_card_index_by_pos(row, col);

    if (index == -1)
    {
        return;
    }

    if (card_list[index]->get_row() == 12)
    {
        return;
    }

    Card *swap_card = card_list[index];
    Card *active_card = card_list[active_card_index];

    int temp_index = swap_card->get_image_index();
    Card::Suit temp_suit = swap_card->get_suit();
    Card::Color temp_color = swap_card->get_color();
    int temp_rank = swap_card->get_rank();

    swap_card->reset_card(active_card->get_suit(), active_card->get_color(), active_card->get_rank(), active_card->get_image_index());
    active_card->reset_card(temp_suit, temp_color, temp_rank, temp_index);

    on_actionUndo_triggered();

    command_stage = 0;

    int times_remain = 3 - swap_time;

    QMessageBox::information(this, "Swap Info", "Swap times remaining: " + QString::number(times_remain));
    ui->actionHint->setVisible(true);
    if (!reveal_used)
        ui->actionReveal->setVisible(true);
    ui->actionSwap->setVisible(true);
    ui->actionUndo->setVisible(true);
    if (swap_time == 3)
    {
        ui->actionSwap->setVisible(false);
    }
    ui->actionCancel_Swap->setVisible(false);
}

void GameWindow::on_actionCancel_Swap_triggered() {
    ui->actionHint->setVisible(true);
    if (!reveal_used) ui->actionReveal->setVisible(true);
    ui->actionSwap->setVisible(true);
    ui->actionUndo->setVisible(true);
    ui->actionCancel_Swap->setVisible(false);
    on_actionUndo_triggered();
    command_stage = 0;
    active_card_index = -1;
    reset_card_indicator();
    render_and_print_gui();
    --swap_time;
    render_and_print_gui();
}
