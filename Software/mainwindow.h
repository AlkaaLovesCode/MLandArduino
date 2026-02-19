#pragma once
#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QComboBox>
#include <QTextEdit>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onConnect();
    void onStartTracking();
    void onStopTracking();
    void onSensitivityChanged(int value);
    void onSmoothingChanged(int value);

private:
    QLabel *statusLabel;
    QComboBox *portSelector;
    QPushButton *connectBtn;

    QPushButton *startBtn;
    QPushButton *stopBtn;

    QSlider *sensitivitySlider;
    QSlider *smoothingSlider;

    QLabel *servoValueLabel;
    QTextEdit *logBox;
};
