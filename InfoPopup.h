#ifndef INFOPOPUP_H
#define INFOPOPUP_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

class InfoPopup : public QWidget {
    Q_OBJECT
public:
    explicit InfoPopup(QWidget* parent = nullptr) : QWidget(parent, Qt::ToolTip) {
        // Qt::ToolTip already includes Frameless and StayOnTop behavior, no need to add them manually
        setAttribute(Qt::WA_ShowWithoutActivating);
        QVBoxLayout* layout = new QVBoxLayout(this);
        infoLabel = new QLabel(this);
        layout->addWidget(infoLabel);
        setLayout(layout);
        setStyleSheet("background-color: #ffffdd; border: 1px solid black; padding: 4px; color: black;");
    }


    void setInfo(const QString& info) {
        infoLabel->setText(info);
        adjustSize();
    }

private:
    QLabel* infoLabel;
};

#endif // INFOPOPUP_H
