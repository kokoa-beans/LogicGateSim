#include "GateItem.h"
#include "QStyleOptionGraphicsItem"
#include "QTimer"
#include "mainwindow.h"

GateItem::GateItem(Gate* gate) : gate(gate) {
    setAcceptHoverEvents(true);
    hoverTimer = new QTimer(this);
    hoverTimer->setSingleShot(true);
    hoverTimer->setInterval(800);

    infoPopup = new InfoPopup();

    connect(hoverTimer, &QTimer::timeout, this, [this]() {

        QString info = QString("Inputs: %1\nOutputs: %2")
        .arg(this->gate->inputNum)
            .arg(this->gate->outputNum);
        infoPopup->setInfo(info);

        QPoint offset(15, 15);  // shift popup a bit right/down from cursor
        infoPopup->move((lastCursorPos + offset).toPoint());

        infoPopup->show();
    });

    setFlags(ItemIsMovable | ItemIsSelectable );
}

QRectF GateItem::boundingRect() const {
    return QRectF(0, 0, 60, 40);
}

void GateItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *) {
    painter->setBrush(Qt::lightGray);
    painter->drawRect(boundingRect());

    // Highlight border if selected
    if (option->state & QStyle::State_Selected) {
        QPen pen(Qt::blue, 2, Qt::DashLine);
        painter->setPen(pen);
        painter->drawRect(boundingRect());
    }

    QPoint offset(15, 15);  // shift popup a bit right/down from cursor
    infoPopup->move((lastCursorPos+offset).toPoint());

    painter->setPen(Qt::black);
    painter->drawText(boundingRect(), Qt::AlignCenter, gate->getType());
}

void GateItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mousePressEvent(event);

    auto* view = scene()->views().isEmpty() ? nullptr : scene()->views().first();
    if (view) {
        auto* mainWin = dynamic_cast<MainWindow*>(view->window());
        if (mainWin) {
            mainWin->handleGateClick(this);
        }
    }
    emit positionChanged();
}


void GateItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    lastCursorPos = event->screenPos().toPointF();
    QGraphicsItem::mouseMoveEvent(event);
    emit positionChanged();
}

void GateItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    lastCursorPos = event->screenPos().toPointF();
    hoverTimer->start();
    QGraphicsItem::hoverEnterEvent(event);
}

void GateItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    hoverTimer->stop();
    if (infoPopup->isVisible()) infoPopup->hide();
    QGraphicsItem::hoverLeaveEvent(event);
}


Gate* GateItem::getGate() const {
    return gate;
}

int GateItem::inputNum(){
    return gate->inputNum;
}

int GateItem::outputNum(){
    return gate->outputNum;
}
