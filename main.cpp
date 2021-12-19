#include <QApplication>
#include "SelectionForm.h"
#include "MainWindow.h"
#include "MainCalculation.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    SelectionForm form;
    MainWindow mainWindow;
    MainWindow::connect(&form, &SelectionForm::startMainWindow, &mainWindow, &MainWindow::startMainWindow);
    MainWindow::connect(&form, &SelectionForm::startMainWindow, mainWindow.worker, &MainCalculation::recordData);
    // Show form first
    form.show();

    return QApplication::exec();
}
