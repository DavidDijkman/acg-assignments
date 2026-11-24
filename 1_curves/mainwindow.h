#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

/**
 * @brief The MainWindow class represents the main window.
 */
class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

 private slots:
  void on_controlNetCheckBox_toggled(bool checked);
  void on_subdivCurveCheckBox_toggled(bool checked);
  void on_netPresets_currentIndexChanged(int index);
  void on_subdivMask_returnPressed();
  void on_subdivSteps_valueChanged(int numSteps);

 private:
  Ui::MainWindow *ui;
};

#endif  // MAINWINDOW_H
