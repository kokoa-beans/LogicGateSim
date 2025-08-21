#ifndef OUTPUTGATE_H
#define OUTPUTGATE_H

#include "Gate.h"
#include <iostream>

class OutputGate : public Gate {
public:


    OutputGate(){
        inputNum = 1;
        outputNum = 0;


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
        return "OUTPUT";
    }
};

#endif
