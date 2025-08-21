#ifndef GATEITEM_H
#define GATEITEM_H

#include <QGraphicsItem>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QObject> // Required for signals/slots
#include "Gate.h"
#include "InfoPopup.h"

class GateItem : public QGraphicsObject {
    Q_OBJECT

public:

    InfoPopup* infoPopup = nullptr;
    QTimer* hoverTimer = nullptr;


    GateItem(Gate* gate);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

    int inputNum();
    int outputNum();

    Gate* getGate() const;

signals:
    void positionChanged();

private:
    Gate* gate;
    QPointF dragStart;

    QPointF lastCursorPos;
};

#endif
