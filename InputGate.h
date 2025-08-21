#ifndef INPUTGATE_H
#define INPUTGATE_H

#include "Gate.h"
#include <iostream>

class InputGate : public Gate {
public:


    InputGate(){
        inputNum = 0;
        outputNum = 1;


        inputs.resize(inputNum, false);
        inputConnected.resize(inputNum, false);

        outputs.resize(outputNum, false);

        outputConnections.resize(outputNum, std::vector<Gate*>{});
        outputConnectionsIndexes.resize(outputNum, std::vector<int>{});

        needChange.resize(outputNum, false);
    }

    std::vector<bool> evaluate() override {
        return outputs;
    }

    QString getType() const override {
        return "INPUT";
    }
};

#endif
