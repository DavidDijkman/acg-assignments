#include "mainwindow.h"

#include "ui_mainwindow.h"

/**
 * @brief MainWindow::MainWindow Creates a new Main Window UI.
 * @param parent Qt parent widget.
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  qDebug() << "✓✓ MainWindow constructor";
}

/**
 * @brief MainWindow::~MainWindow Deconstructs the main window.
 */
MainWindow::~MainWindow() {
  delete ui;
  qDebug() << "✗✗ MainWindow destructor";
}

/**
 * @brief MainWindow::on_ngonSlider_valueChanged Called when the n-gon slider's
 * value changes.
 * @param value New value of the slider.
 */
void MainWindow::on_ngonSlider_valueChanged(int value) {
  ui->ngonValue->setNum(value);
  ui->mainView->updateBuffers(value);
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_pushButton_clicked Called when the button with the name
 * "pushButton" is clicked.
 */
void MainWindow::on_pushButton_clicked() { qDebug() << "hello"; }
