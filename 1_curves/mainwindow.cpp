#include "mainwindow.h"

#include "ui_mainwindow.h"

/**
 * @brief MainWindow::MainWindow Creates a new Main Window UI.
 * @param parent Qt parent widget.
 */
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  qDebug() << "✓✓ MainWindow constructor";
  ui->setupUi(this);

  // Set some default UI values
  Settings& settings = ui->mainView->settings;
  ui->controlNetCheckBox->setChecked(settings.showNet);
  ui->subdivCurveCheckBox->setChecked(settings.showSubdivisionCurve);

  // Input restrictions for the Mask
  ui->subdivMask->setValidator(new QRegularExpressionValidator(
      QRegularExpression("(-?\\d+\\s)+(-?\\d+\\s?)")));

  // Initialise mask
  ui->mainView->subCurve.setMask(ui->subdivMask->text());
}

/**
 * @brief MainWindow::~MainWindow Deconstructs the main window.
 */
MainWindow::~MainWindow() {
  qDebug() << "✗✗ MainWindow destructor";
  delete ui;
}

// Don't worry about adding documentation for the UI-related functions.

void MainWindow::on_controlNetCheckBox_toggled(bool checked) {
  ui->mainView->settings.showNet = checked;
  ui->mainView->update();
}

void MainWindow::on_subdivCurveCheckBox_toggled(bool checked) {
  ui->mainView->settings.showSubdivisionCurve = checked;
  ui->mainView->update();
}

void MainWindow::on_netPresets_currentIndexChanged(int index) {
  if (ui->mainView->isValid()) {
    ui->mainView->subCurve.presetNet(index);
  }
  ui->mainView->updateBuffers();
}

void MainWindow::on_subdivMask_returnPressed() {
  ui->mainView->subCurve.setMask(ui->subdivMask->text());
  ui->mainView->update();
}

void MainWindow::on_subdivSteps_valueChanged(int numSteps) {
  // TODO: ...
}
