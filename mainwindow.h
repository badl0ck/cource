#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFutureWatcher>
#include <QMainWindow>
#include <QTime>
#include <QList>

struct Task
{
    int beginIndex; // первая строка выделенного куска массива
    int endIndex; // последняя строка
    QList<QList<int>> *arr; // указатель на матрицу
    int nElem; // количество элементов в куске
};

// extern int bValue;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_pushButton_clicked();

    void on_lineEdit_2_customContextMenuRequested(const QPoint &pos); // лишнее

    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();

    void progressValueChanged(int v);
    void finished();

    void on_pushButton_4_clicked();

private:
    Ui::MainWindow *ui;
    QFutureWatcher<int> *watcher;
    QTime time;
    QList<QList<int>> arr;
    QList<Task> tasks;
    QFuture<int> future;
    int nElem;
};
#endif // MAINWINDOW_H
