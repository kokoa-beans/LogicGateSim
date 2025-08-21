#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <queue>
#include "WireItem.h"

class MainWindow : public QMainWindow {
    Q_OBJECT



public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void handleGateClick(GateItem* clickedGate);

    void enqueueGate(Gate* gate);
    void processSimulation();

    std::vector<GateItem*> getInputGates();
    std::vector<GateItem*> getOutputGates();

    std::vector<std::vector<bool>> genTruthTable();


    void genSuperGate(const std::vector<std::vector<bool>>& truthTable,
                                  uint32_t inputCount, uint32_t outputCount,
                                  const std::string& filename);

    std::vector<std::vector<bool>> loadSuperGate(const std::string& filename,
                                                uint32_t& inputCount,
                                                 uint32_t& outputCount);


    //Different modes
    bool deleteMode = false;
    bool wireMode = false;
    bool inputMode = false;


private:
    QGraphicsScene *scene;
    QGraphicsView *view;
    void setupUI();

    GateItem* pendingInputGate = nullptr;



    std::queue<Gate*> gateQueue;
};
#endif
