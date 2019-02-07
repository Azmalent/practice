#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QIntValidator>
#include <QMainWindow>

#include <GL/gl.h>

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        static MainWindow* instance();

    private:
        static MainWindow* instanceHolder;
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

        Ui::MainWindow* ui;
        QIntValidator* validator;

    private slots:
        void on_updateButton_clicked();

    signals:
        void updateColorSignal(GLint r, GLint g, GLint b);
};

#endif
