#ifndef WIREITEM_H
#define WIREITEM_H

#include "GateItem.h"
#include "InfoPopup.h"

#include <QGraphicsObject>
#include <QGraphicsObject>
#include <QPainter>

class WireItem : public QGraphicsObject {
    Q_OBJECT
public:
    InfoPopup* infoPopup = nullptr;
    QTimer* hoverTimer = nullptr;

    WireItem(GateItem* source, int sourceOutputIndex, GateItem* target, int targetInputIndex);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

    QPainterPath shape() const override;


    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    GateItem* source;
    GateItem* target;

    int sourceOutputIndex;
    int targetInputIndex;


    QPointF sourcePos;
    QPointF targetPos;

    bool badTarget;

    bool hovering;

private:
    QPointF lastCursorPos;

public slots:
    void updatePosition();
};

#endif
