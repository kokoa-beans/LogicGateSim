#ifndef GATE_H
#define GATE_H

#include <QObject>
#include <vector>
#include <QString>
#include <iostream>

class Gate : public QObject{
    Q_OBJECT
public:
    virtual ~Gate() = default;

    void setInput(int pinNum, bool val) {
        if (pinNum >= inputs.size()) return;
        inputs[pinNum] = val;
    }

    void setOutput(int pinNum, bool val) {
        if (pinNum >= outputs.size()) return;
        outputs[pinNum] = val;
    }

    void setOutputConnections(int pinNum, Gate* val) {
        if (pinNum >= outputConnections.size()) return;
        outputConnections[pinNum].push_back(val);
    }


    void setOutputConnectionsIndexes(int pinNum, int val) {
        if (pinNum >= outputConnectionsIndexes.size()) return;
        outputConnectionsIndexes[pinNum].push_back(val);
    }

    void setNeedChange(int pinNum, bool value) {
        if (pinNum >= needChange.size()) return;
        needChange[pinNum] = value;

        std::cout<<this->getType().toStdString()<<std::endl;

        if (needChange[pinNum]) emit needChangeUpdated(pinNum);
    }


    virtual std::vector<bool> evaluate() = 0;
    virtual QString getType() const = 0;

    int inputNum;
    int outputNum;

    std::vector<bool> inputs;
    std::vector<bool> inputConnected;

    std::vector<bool> outputs;

    std::vector<std::vector<Gate*>> outputConnections;
    std::vector<std::vector<int>> outputConnectionsIndexes;

    std::vector<bool> needChange;

signals:
    void needChangeUpdated(int outputIndex); // emitted whenever needChange changes

};

#endif
