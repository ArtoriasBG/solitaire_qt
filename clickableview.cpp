#include <QMouseEvent>

#include "clickableview.h"

ClickableView::ClickableView(QWidget *parent):QGraphicsView(parent) {}

void ClickableView::mousePressEvent(QMouseEvent *event) {

    QPointF point = mapToScene(event->pos());

    emit mouseClicked(point.ry()/113, point.rx()/147); // 1 row = half of card height, 1 col = card width

}
