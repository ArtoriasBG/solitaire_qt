#ifndef CLICKABLEVIEW_H
#define CLICKABLEVIEW_H

// similar to the one in PA4, make the graphicview to a cliackable one and emit position signal to map_clicked()
#include <QGraphicsView>

class ClickableView : public QGraphicsView
{
    Q_OBJECT
public:
    ClickableView(QWidget *parent);

protected:
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void mouseClicked(int row, int col);
};

#endif // CLICKABLEVIEW_H
