#include "mainwindow.h"
#include "GateItem.h"
#include "WireItem.h"
#include "NandGate.h"
#include "InputGate.h"
#include "OutputGate.h"

#include <QToolBar>
#include <QAction>
#include <QMouseEvent>
#include <QStatusBar>
#include <QInputDialog>
#include <QTimer>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), scene(new QGraphicsScene(this)), view(new QGraphicsView(scene, this)) {
    setupUI();
}



MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    setWindowTitle("Logic Gate Simulator");
    resize(800, 600);
    setCentralWidget(view);
    view->setScene(scene);

    QToolBar *toolbar = addToolBar("Tools");


    QAction *addNandGateAction = toolbar->addAction("Add NAND Gate");

    connect(addNandGateAction, &QAction::triggered, this, [this]() {
        wireMode = false;
        inputMode = false;
        deleteMode = false;
        scene->clearSelection();
        auto *gate = new NandGate();
        auto *item = new GateItem(gate);
        item->setPos(100, 100);
        scene->addItem(item);
    });

    QAction *addInputGateAction = toolbar->addAction("Add INPUT Gate");

    connect(addInputGateAction, &QAction::triggered, this, [this]() {
        wireMode = false;
        inputMode = false;
        deleteMode = false;
        scene->clearSelection();
        auto *gate = new InputGate();
        auto *item = new GateItem(gate);
        item->setPos(100, 100);
        scene->addItem(item);
    });

    QAction *addOutputGateAction = toolbar->addAction("Add OUTPUT Gate");

    connect(addOutputGateAction, &QAction::triggered, this, [this]() {
        wireMode = false;
        inputMode = false;
        deleteMode = false;
        scene->clearSelection();
        auto *gate = new OutputGate();
        auto *item = new GateItem(gate);
        item->setPos(100, 100);
        scene->addItem(item);
    });

    QAction *addWireAction = toolbar->addAction("Add Wire");

    connect(addWireAction, &QAction::triggered, this, [this]() {
        inputMode = false;
        deleteMode = false;
        scene->clearSelection();
        pendingInputGate = nullptr;
        wireMode = !wireMode;

        if (wireMode){
            statusBar()->showMessage("Click on the input gate.");
        }
        else{
            statusBar()->showMessage("Wire mode off.", 2000);
        }
    });

    QAction *deleteAction = toolbar->addAction("Delete Wire/Gate");

    connect(deleteAction, &QAction::triggered, this, [this]() {
        wireMode = false;
        inputMode = false;
        deleteMode = !deleteMode;
        if (deleteMode) {
            statusBar()->showMessage("Delete mode: Click a gate or wire to remove it.");
        } else {
            statusBar()->showMessage("Delete mode off.", 2000);
        }
    });

    QAction *changeInputAction = toolbar->addAction("Change Input");

    connect(changeInputAction, &QAction::triggered, this, [this]() {
        wireMode = false;
        deleteMode = false;
        inputMode = !inputMode;
        if (inputMode) {
            statusBar()->showMessage("Input mode: Click an input to change.", 2000);
        } else {
            statusBar()->showMessage("Input mode off.", 2000);
        }
    });

    QAction *saveSuperGate = toolbar->addAction("Save Super Gate");

    connect(saveSuperGate, &QAction::triggered, this, [this]() {
        wireMode = false;
        deleteMode = false;
        inputMode = false;

        // Ask for a name of the super gate
        bool ok2;
        QString name = QInputDialog::getText(
            this,
            "Save Super Gate",
            "Name of Super Gate:",
            QLineEdit::Normal,
            "",
            &ok2
            );

        if (!ok2 || name.isEmpty()) return; // user canceled

        // Ask where to save the file
        QString filename = QFileDialog::getSaveFileName(
            this,
            "Choose Save Location",
            QDir::homePath() + "/" + name + ".bin", // default location and extension
            "Super Gate Files (*.bin);;All Files (*)"
            );

        if (filename.isEmpty()) return; // user canceled

        try {
            auto truthTable = genTruthTable(); // <-- your existing function
            genSuperGate(truthTable, getInputGates().size(), getOutputGates().size(), filename.toStdString());
        } catch (const std::exception& e) {
            QMessageBox::warning(this, "Save Failed", e.what());
        }
    });


}

void MainWindow::handleGateClick(GateItem* clickedGate) {
    if (deleteMode){
        scene->removeItem(clickedGate);
        clickedGate->hoverTimer->stop();
        if (clickedGate->infoPopup->isVisible()) clickedGate->infoPopup->hide();
        delete clickedGate;
        return;
    }

    if (inputMode){
        if (clickedGate->getGate()->getType() == "INPUT"){
            clickedGate->getGate()->outputs[0] = !clickedGate->getGate()->outputs[0];
            gateQueue.push(clickedGate->getGate());
            processSimulation();
        }
        return;
    }

    if (wireMode){
        if (!pendingInputGate) {
            pendingInputGate = clickedGate;
            statusBar()->showMessage("Click on the output gate.", 2000);
        } else {

            bool ok2;
            int sourceOutputIndex = QInputDialog::getInt(this, "Select Output Index", "Output Index (0-indexed) of source gate:", 0, -1, pendingInputGate -> outputNum()-1, 1, &ok2);

            if (!ok2){
                pendingInputGate->setSelected(false);
                pendingInputGate = nullptr;

                clickedGate->setSelected(false);
                clickedGate=nullptr;

                wireMode = false;
                statusBar()->clearMessage();
                return; // User cancelled
            }

            bool ok3;
            int targetInputIndex = QInputDialog::getInt(this, "Select Input Index", "Input Index (0-indexed) of target gate:", 0, -1, clickedGate -> inputNum()-1, 1, &ok3);

            if (!ok3){
                pendingInputGate->setSelected(false);
                pendingInputGate = nullptr;

                clickedGate->setSelected(false);
                clickedGate=nullptr;

                wireMode = false;
                statusBar()->clearMessage();
                return; // User cancelled
            }


            auto* wire = new WireItem(pendingInputGate, sourceOutputIndex, clickedGate, targetInputIndex);

            if (wire->badTarget){
                statusBar()->showMessage("ERROR: There is already an input at that pin.", 2000);
                delete wire;
                pendingInputGate->setSelected(false);
                // Reset for next wire
                pendingInputGate=nullptr;
                wireMode = false;
                return;
            }

            scene->addItem(wire);
            enqueueGate(clickedGate->getGate());
            processSimulation();

            pendingInputGate->setSelected(false);

            clickedGate->setSelected(false);

            statusBar()->showMessage("Wire added.", 2000);

            // Reset for next wire
            pendingInputGate=nullptr;
            wireMode = false;
        }
    }
}

void MainWindow::enqueueGate(Gate* gate) {
    gateQueue.push(gate);
}

void MainWindow::processSimulation() {
    while (!gateQueue.empty()) {
        Gate* source = gateQueue.front();
        gateQueue.pop();

        std::vector<bool> newOutput = source->evaluate();

        for (int i = 0; i < source->outputNum; i++){

            for (int j = 0; j < source->outputConnections[i].size(); j++){
                Gate* outputConnection = source->outputConnections[i][j];
                int outputConnectionIndex = source->outputConnectionsIndexes[i][j];

                if (outputConnection == nullptr) continue;

                for (bool each:source->outputs){
                    if (each) std::cout << "1 ";
                    else std::cout<< "0 ";
                }
                std::cout << std::endl;

                for (bool each:outputConnection->inputs){
                    if (each) std::cout << "1 ";
                    else std::cout<< "0 ";
                }
                std::cout << std::endl;

                if (newOutput[i] != outputConnection->inputs[outputConnectionIndex]){
                    outputConnection->inputs[outputConnectionIndex] = newOutput[i];
                    source->setNeedChange(i, true);
                    enqueueGate(outputConnection);
                }
            }
        }
    }
}

std::vector<GateItem*> MainWindow::getInputGates() {
    std::vector<GateItem*> inputs;
    for (auto* item : scene->items()) {
        if (auto gateItem = dynamic_cast<GateItem*>(item)) {
            if (gateItem->getGate()->getType() == "INPUT") {
                inputs.push_back(gateItem);
            }
        }
    }
    return inputs;
}

std::vector<GateItem*> MainWindow::getOutputGates() {
    std::vector<GateItem*> outputs;
    for (auto* item : scene->items()) {
        if (auto gateItem = dynamic_cast<GateItem*>(item)) {
            if (gateItem->getGate()->getType() == "OUTPUT") {
                outputs.push_back(gateItem);
            }
        }
    }
    return outputs;
}

std::vector<std::vector<bool>> MainWindow::genTruthTable() {
    auto inputs = getInputGates();
    auto outputs = getOutputGates();

    int inputCount = inputs.size();
    int outputCount = outputs.size();

    std::vector<std::vector<bool>> truthTable(1 << inputCount, std::vector<bool>(outputCount));

    for (int mask = 0; mask < (1 << inputCount); ++mask) {
        // set all inputs according to mask
        for (int i = 0; i < inputCount; ++i) {
            bool bit = (mask & (1 << i)) != 0;
            inputs[i]->getGate()->setOutput(0, bit); //turn that input bit on/off
            gateQueue.push(inputs[i]->getGate());
        }

        processSimulation(); // propagate signals

        // record outputs
        for (int j = 0; j < outputCount; ++j) {
            bool val = outputs[j]->getGate()->inputs[0];
            truthTable[mask][j] = val;
        }
    }

    return truthTable;
}

void MainWindow::genSuperGate(const std::vector<std::vector<bool>>& truthTable,
                              uint32_t inputCount, uint32_t outputCount,
                              const std::string& filename) {
        std::ofstream out(filename, std::ios::binary);
    if (!out) throw std::runtime_error("Failed to open file");


    // Write header
    out.write(reinterpret_cast<const char*>(&inputCount), sizeof(inputCount));
    out.write(reinterpret_cast<const char*>(&outputCount), sizeof(outputCount));

    // Write rows
    for (const auto& row : truthTable) {
        uint8_t byte = 0;
        int bitIndex = 0;
        for (bool val : row) {
            if (val) byte |= (1 << bitIndex);
            bitIndex++;
            if (bitIndex == 8) {
                out.write(reinterpret_cast<const char*>(&byte), 1);
                byte = 0;
                bitIndex = 0;
            }
        }
        if (bitIndex > 0) { // flush last byte
            out.write(reinterpret_cast<const char*>(&byte), 1);
        }
    }
}

std::vector<std::vector<bool>> MainWindow::loadSuperGate(const std::string& filename,
                                                    uint32_t& inputCount,
                                                    uint32_t& outputCount) {
    std::ifstream in(filename, std::ios::binary);
    if (!in) throw std::runtime_error("Failed to open file");

    // Read header
    in.read(reinterpret_cast<char*>(&inputCount), sizeof(inputCount));
    in.read(reinterpret_cast<char*>(&outputCount), sizeof(outputCount));

    size_t rowCount = 1u << inputCount;
    std::vector<std::vector<bool>> truthTable(rowCount, std::vector<bool>(outputCount));

    for (size_t r = 0; r < rowCount; ++r) {
        size_t bitsRead = 0;
        while (bitsRead < outputCount) {
            uint8_t byte;
            in.read(reinterpret_cast<char*>(&byte), 1);
            for (int b = 0; b < 8 && bitsRead < outputCount; ++b, ++bitsRead) {
                truthTable[r][bitsRead] = (byte >> b) & 1;
            }
        }
    }
    return truthTable;
}
