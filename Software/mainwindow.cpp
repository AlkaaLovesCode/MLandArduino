#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    // ===== Top Bar =====
    statusLabel = new QLabel("Disconnected");
    portSelector = new QComboBox();
    portSelector->addItems({"COM3", "COM4", "COM5"});
    connectBtn = new QPushButton("Connect");

    QHBoxLayout *topBar = new QHBoxLayout();
    topBar->addWidget(new QLabel("Port:"));
    topBar->addWidget(portSelector);
    topBar->addWidget(connectBtn);
    topBar->addStretch();
    topBar->addWidget(statusLabel);

    // ===== Control Panel =====
    startBtn = new QPushButton("Start Tracking");
    stopBtn  = new QPushButton("Stop Tracking");

    sensitivitySlider = new QSlider(Qt::Horizontal);
    sensitivitySlider->setRange(1, 100);

    smoothingSlider = new QSlider(Qt::Horizontal);
    smoothingSlider->setRange(1, 100);

    servoValueLabel = new QLabel("Servo: 0°");

    logBox = new QTextEdit();
    logBox->setReadOnly(true);

    QVBoxLayout *controlLayout = new QVBoxLayout();
    controlLayout->addWidget(startBtn);
    controlLayout->addWidget(stopBtn);
    controlLayout->addWidget(new QLabel("Sensitivity"));
    controlLayout->addWidget(sensitivitySlider);
    controlLayout->addWidget(new QLabel("Smoothing / Bias"));
    controlLayout->addWidget(smoothingSlider);
    controlLayout->addWidget(servoValueLabel);
    controlLayout->addWidget(new QLabel("Logs"));
    controlLayout->addWidget(logBox);

    QGroupBox *controlBox = new QGroupBox("Control Panel");
    controlBox->setLayout(controlLayout);

    // ===== Layout Combine =====
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addWidget(controlBox, 1);

    QVBoxLayout *root = new QVBoxLayout();
    root->addLayout(topBar);
    root->addLayout(mainLayout);

    central->setLayout(root);

    // ===== Signals =====
    connect(connectBtn, &QPushButton::clicked, this, &MainWindow::onConnect);
    connect(startBtn, &QPushButton::clicked, this, &MainWindow::onStartTracking);
    connect(stopBtn, &QPushButton::clicked, this, &MainWindow::onStopTracking);
    connect(sensitivitySlider, &QSlider::valueChanged, this, &MainWindow::onSensitivityChanged);
    connect(smoothingSlider, &QSlider::valueChanged, this, &MainWindow::onSmoothingChanged);
}

MainWindow::~MainWindow() {}

void MainWindow::onConnect() {
    statusLabel->setText("Connected");
    logBox->append("Connected to device");
}

void MainWindow::onStartTracking() {
    logBox->append("Tracking started");
}

void MainWindow::onStopTracking() {
    logBox->append("Tracking stopped");
}

void MainWindow::onSensitivityChanged(int value) {
    logBox->append("Sensitivity: " + QString::number(value));
}

void MainWindow::onSmoothingChanged(int value) {
    logBox->append("Smoothing: " + QString::number(value));
}
