#ifndef NANDGATE_H
#define NANDGATE_H

#include "Gate.h"
#include <iostream>

class NandGate : public Gate {
public:
    NandGate(){
        inputNum = 2;
        outputNum = 1;

        inputs.resize(inputNum, false);
        inputConnected.resize(inputNum, false);

        outputs.resize(outputNum, false);

        outputConnections.resize(outputNum, std::vector<Gate*>{});
        outputConnectionsIndexes.resize(outputNum, std::vector<int>{});

        needChange.resize(outputNum, false);
    }


    std::vector<bool> evaluate() override {
        bool val = !(inputs[0] && inputs[1]);
        setOutput(0, val);
        return outputs;
    }

    QString getType() const override {
        return "NAND";
    }
};

#endif
