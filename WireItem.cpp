#include "WireItem.h"
#include "GateItem.h"
#include "mainwindow.h"
#include "QTimer"
#include <iostream>
#include <QPainterPathStroker>

WireItem::WireItem(GateItem* source, int sourceOutputIndex, GateItem* target, int targetInputIndex)
    : source(source), sourceOutputIndex(sourceOutputIndex), target(target), targetInputIndex(targetInputIndex) {

    if (targetInputIndex >= target->getGate()->inputNum || targetInputIndex < 0 || target->getGate()->inputConnected[targetInputIndex]){
        badTarget = true;
        return;
    }

    setAcceptHoverEvents(true);
    hoverTimer = new QTimer(this);
    hoverTimer->setSingleShot(true);
    hoverTimer->setInterval(800);

    infoPopup = new InfoPopup();

    connect(hoverTimer, &QTimer::timeout, this, [this]() {

        QString info = QString("Source: %1, Pin: %2\nTarget: %3, Pin: %4")
        .arg(this->source->getGate()->getType())
            .arg(this->sourceOutputIndex)
                .arg(this->target->getGate()->getType())
                    .arg(this->targetInputIndex);
        infoPopup->setInfo(info);


        QPoint offset(15, 15);  // shift popup a bit right/down from cursor
        infoPopup->move((lastCursorPos + offset).toPoint());

        infoPopup->show();
    });

    setZValue(0); // Below gates


    target->getGate()->setInput(targetInputIndex, source->getGate()->outputs[sourceOutputIndex]);

    source->getGate()->setOutputConnections(sourceOutputIndex, target->getGate());

    source->getGate()->setOutputConnectionsIndexes(sourceOutputIndex, targetInputIndex);

    target->getGate()->inputConnected[targetInputIndex] = true;

    updatePosition();


    // Connect positionChanged signal
    if (GateItem* s = dynamic_cast<GateItem*>(source)) {
        QObject::connect(s, &GateItem::positionChanged, this, &WireItem::updatePosition);
    }
    if (GateItem* t = dynamic_cast<GateItem*>(target)) {
        QObject::connect(t, &GateItem::positionChanged, this, &WireItem::updatePosition);
    }

    QObject::connect(source->getGate(), &Gate::needChangeUpdated,
                     this, [this](int changedIndex) {
                         if (changedIndex == this->sourceOutputIndex) {
                             this->update();  // repaint only if relevant output changed
                         }
                     });

}

QRectF WireItem::boundingRect() const {
    // pen width/pick tolerance — ensure it is at least as large as the stroke + extra
    const qreal penHalf = 12.0; // half of the clickable thickness
    QRectF r = QRectF(sourcePos, targetPos).normalized();
    return r.adjusted(-penHalf, -penHalf, penHalf, penHalf);
}

void WireItem::updatePosition() {
    if (!source || !target) return;

    // Get scene coordinates for gate pin positions
    QPointF sceneSource = source->sceneBoundingRect().bottomRight();
    QPointF sceneTarget = target->sceneBoundingRect().bottomLeft();

    // Convert to item-local coordinates (CRITICAL)
    QPointF newSourcePos = mapFromScene(sceneSource);
    QPointF newTargetPos = mapFromScene(sceneTarget);

    // Inform Qt that geometry is changing before modifying endpoints
    prepareGeometryChange();

    sourcePos = newSourcePos;
    targetPos = newTargetPos;

    // Trigger repaint
    update();
}

QPainterPath WireItem::shape() const {
    QPainterPath p;
    p.moveTo(sourcePos);
    p.lineTo(targetPos);

    QPainterPathStroker stroker;
    stroker.setWidth(12.0);               // clickable tolerance in pixels (tweak this)
    stroker.setCapStyle(Qt::RoundCap);    // rounded ends feel nicer
    stroker.setJoinStyle(Qt::RoundJoin);

    return stroker.createStroke(p);
}

void WireItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*){
    if (!source || !target) return;

    // Update state from model
    source->getGate()->needChange[sourceOutputIndex] = false;
    bool isTrue = source->getGate()->outputs[sourceOutputIndex];

    // Use round caps so the line visually matches the stroker hit-area
    QPen wirePen(isTrue ? Qt::blue : Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter->setPen(wirePen);
    painter->drawLine(sourcePos, targetPos);

    // Draw end dots (note: coordinates are local now)
    painter->setBrush(Qt::green);
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(sourcePos, 4, 4);

    painter->setBrush(Qt::red);
    painter->drawEllipse(targetPos, 4, 4);


    if (hovering){
        QPen highlightPen(Qt::yellow, 6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        highlightPen.setColor(QColor(255, 200, 0, 150));
        painter->setPen(highlightPen);
        painter->setBrush(Qt::NoBrush);
        painter->drawLine(sourcePos, targetPos);
    }
}

void WireItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    hovering = true;
    lastCursorPos = event->screenPos().toPointF();
    hoverTimer->start();
    update();
    QGraphicsItem::hoverEnterEvent(event);
}

void WireItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    hovering = false;
    hoverTimer->stop();
    if (infoPopup->isVisible()) infoPopup->hide();
    QGraphicsItem::hoverLeaveEvent(event);
}

void WireItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    auto* view = scene()->views().isEmpty() ? nullptr : scene()->views().first();
    if (view) {
        auto* mainWin = dynamic_cast<MainWindow*>(view->window());
        if (mainWin && mainWin->deleteMode) {

            if (source != nullptr){
                std::vector<Gate*>& OCV = source->getGate()->outputConnections[sourceOutputIndex];
                OCV.erase(std::remove(OCV.begin(), OCV.end(), target->getGate()), OCV.end());

                std::vector<int>& OCVI = source->getGate()->outputConnectionsIndexes[sourceOutputIndex];
                OCVI.erase(std::remove(OCVI.begin(), OCVI.end(), targetInputIndex), OCVI.end());
            }

            if (target != nullptr){
                target->getGate()->inputs[targetInputIndex] = false;
                target->getGate()->inputConnected[targetInputIndex] = false;

                mainWin->enqueueGate(target->getGate());
                mainWin->processSimulation();
            }

            hoverTimer->stop();
            if (infoPopup->isVisible()) infoPopup->hide();

            scene()->removeItem(this);
            delete this;
            return;
        }
    }
    QGraphicsItem::mousePressEvent(event);
}


