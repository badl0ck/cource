#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QtConcurrent/QtConcurrent>
#include <QCoreApplication>
#include <QFuture>
#include <QtWidgets>
#include <QThread>
#include <string>
#include <time.h>
#include <QFile>
#include <QIODevice>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pushButton_2->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

int perElementFunc(const Task tsk)
{
    int semiResult = 0;
    /*СОРТИРОВКА ДВУМЕРНОГО МАССИВА*/
    int temp = 0;
       for (int m = 0; m < (tsk.endIndex - tsk.beginIndex + 1) * tsk.nElem - 1; m++)   //сдвиги очередных элементов в правильную позицию
           /*сдвиг элемента массива в правильную позицию*/
           for (int i = tsk.beginIndex; i <= tsk.endIndex; i++)
           {
               QList<int> currentLineSorting = tsk.arr->at(i);
               for (int j = 0; j<tsk.nElem - 1; j++){
                   /*АНАЛИЗ НА ПОСЛЕДНИЙ ЭЛЕМЕНТ МАССИВА*/
                       if (i == tsk.endIndex && j == tsk.nElem - 1){  //Если строка последняя и справа тупик, то ничего не делаем
                               continue;
                       }
                   /*КОНЕЦ АНАЛИЗА НА ПОСЛЕДНЮЮ СТРОКУ*/

                       if (currentLineSorting.at(j) > currentLineSorting.at(j+1)){ //Если элемент не на своей позиции
                          temp = currentLineSorting.at(j);        //Обмен местами
                          currentLineSorting.replace(j, currentLineSorting.at(j+1));
                          currentLineSorting.replace(j+1, temp);

                          QThread::msleep(1);
                   }

                   tsk.arr->replace(i, currentLineSorting);
              }
           }
      /*КОНЕЦ СОРТИРОВКИ ДВУМЕРНОГО МАССИВА*/

    return semiResult;
}

void reduce(int & sum, const int semiSum)
{
    sum += semiSum;
}

void MainWindow::on_pushButton_clicked()
{
    // создать массив с заданным размером (с проверкой)
    // заполнить его рандомными числами
    // распараллелить программу, выделить каждому потоку собственный кусок массива, начать вычисление
    // сложить полученные с разных отрезков данные
    QString outputLine = "";

    QString arrayLengthString = ui->lineEdit_2->text(); // достать из формы значения
    QString threadsNumberString = ui->lineEdit_3->text();

    nElem = arrayLengthString.toInt(); // перевести их в int
    int numThread = threadsNumberString.toInt();

    if (nElem > 0 && numThread > 0) // налагаемые ограничения на вводимые данные
    {
        ui->pushButton->setEnabled(false);
        ui->pushButton_4->setEnabled(false);
        ui->pushButton_2->setEnabled(true);

        int elemPerThread = nElem / numThread; // количество строк на поток

        ui->listWidget->addItem(QString::number(numThread) + " threads");
        ui->listWidget->addItem("Hello!");

        QList<int> current_line; // используется для заполнения, дальше не фигурирует

        for (int i = 0; i < nElem; i++)
        {
            current_line.clear();

            for (int j = 0; j < nElem; j++)
            {
                current_line.append(rand() % 100000 + 100);
            }

            arr.append(current_line);
        }

        // вывести матрицу в окне

        ui->listBeforeSorting->clear();

        for (int i = 0; i < nElem; i++)
        {
            outputLine.clear();
            for (int j = 0; j < nElem; j++)
            {
                outputLine.append(QString::number(arr.at(i).at(j)) + " ");
            }
            ui->listBeforeSorting->addItem(outputLine);
        }

        time.start();

        ui->listWidget->addItem("Preparing tasks");

        for (int i = 0; i <= numThread - 1; i++)
        {
            Task tsk;
            tsk.beginIndex = i * elemPerThread; // номер первой строки куска

            if (i == numThread - 1) // последней строки куска
            {
                tsk.endIndex = nElem - 1;
            }
            else
            {
                tsk.endIndex = (i + 1) * elemPerThread - 1;
            }

            tsk.arr = &arr;
            tsk.nElem = nElem;
            tasks.append(tsk);
        }

        watcher = new QFutureWatcher<int>();
        connect(watcher, SIGNAL(progressValueChanged(int)), this, SLOT(progressValueChanged(int)));
        connect(watcher, SIGNAL(finished()), this, SLOT(finished()));
        future = QtConcurrent::mappedReduced(tasks, perElementFunc, reduce);
        watcher->setFuture(future);
        ui->listWidget->addItem(QString::number(future.progressMinimum()) + " " +
                                QString::number(future.progressValue()) + " " + QString::number(future.progressMaximum()));
    }
    else
    {
        QMessageBox box(this);
        box.setText("threads > 0, arrayLength > 0");
        box.exec();
    }
}

void MainWindow::progressValueChanged(int v)
{
    ui->listWidget->addItem("Progress: " + QString::number(v));
}

void MainWindow::finished()
{
    ui->listWidget->addItem("Time elapsed: " + QString::number(time.elapsed()) + ", elements found: " + QString::number(future.result()));
    ui->listWidget->addItem("-------------------------");
    ui->pushButton->setEnabled(true);
    ui->pushButton_4->setEnabled(true);
    ui->pushButton_2->setEnabled(false);

    QString outputLine = "";
    ui->listAfterSorting->clear();

    for (int i = 0; i < nElem; i++)
    {
        outputLine.clear();
        for (int j = 0; j < nElem; j++)
        {
            outputLine.append(QString::number(arr.at(i).at(j)) + " ");
        }
        ui->listAfterSorting->addItem(outputLine);
    }

    tasks.clear();
    arr.clear();
}

void MainWindow::on_lineEdit_2_customContextMenuRequested(const QPoint &pos)
{

}

void MainWindow::on_pushButton_2_clicked() // пауза/возвращение
{
    if (watcher->isFinished())
    {
        return;
    }

    if (watcher->isPaused())
    {
        watcher->resume();
        // change greetingLabel
        ui->listWidget->addItem("Resumed");
        ui->pushButton_2->setText("Пауза");
    }
    else
    {
        watcher->pause();
        // change greetingLabel - paused
        ui->listWidget->addItem("Paused");
        ui->pushButton_2->setText("Resume");
    }
}

void MainWindow::on_pushButton_3_clicked() // закрыть
{
    if (watcher == NULL)
    {
        return;
    }

    watcher->cancel();
    watcher->waitForFinished();
    close();
}

void MainWindow::on_pushButton_4_clicked() // последовательное вычисление
{
    QString arrayLengthString = ui->lineEdit_2->text(); // достать из формы значения

    int nElem = arrayLengthString.toInt(); // перевести их в int

    if (nElem > 0) // налагаемые ограничения на вводимые данные
    {
        ui->pushButton->setEnabled(false);
        ui->pushButton_4->setEnabled(false);

        ui->listWidget->addItem("Последовательное вычисление");
        ui->listWidget->addItem("Hello!");

        QList<int> current_line; // используется для заполнения, дальше не фигурирует

        for (int i = 0; i < nElem; i++)
        {
            current_line.clear();

            for (int j = 0; j < nElem; j++)
            {
                current_line.append(rand() % 100000 + 100);
            }

            arr.append(current_line);
        }

        QString outputLine = "";
        ui->listBeforeSorting->clear();

        for (int i = 0; i < nElem; i++)
        {
            outputLine.clear();
            for (int j = 0; j < nElem; j++)
            {
                outputLine.append(QString::number(arr.at(i).at(j)) + " ");
            }
            ui->listBeforeSorting->addItem(outputLine);
        }

        time.start();

        ui->listWidget->addItem("Sorting");

        /*СОРТИРОВКА ДВУМЕРНОГО МАССИВА*/
        int temp = 0;
           for (int m = 0; m < nElem * nElem - 1; m++)   //сдвиги очередных элементов в правильную позицию
               /*сдвиг элемента массива в правильную позицию*/
               for (int i = 0; i<nElem ; i++){
                   QList<int> currentLineSorting = arr.at(i);

                   for (int j = 0; j<nElem - 1; j++){
                       /*АНАЛИЗ НА ПОСЛЕДНИЙ ЭЛЕМЕНТ МАССИВА*/
                           if (i==nElem-1 && j==nElem-1){  //Если строка последняя и справа тупик, то ничего не делаем
                                   continue;
                           }
                       /*КОНЕЦ АНАЛИЗА НА ПОСЛЕДНЮЮ СТРОКУ*/

                       if (currentLineSorting.at(j) > currentLineSorting.at(j+1)){ //Если элемент не на своей позиции
                          temp = currentLineSorting.at(j);        //Обмен местами
                          currentLineSorting.replace(j, currentLineSorting.at(j+1));
                          currentLineSorting.replace(j+1, temp);
                       }
                  }

                  arr.replace(i, currentLineSorting);
               }
          /*КОНЕЦ СОРТИРОВКИ ДВУМЕРНОГО МАССИВА*/

        ui->listWidget->addItem("Time elapsed: " + QString::number(time.elapsed()));
        ui->listWidget->addItem("-------------------------");

        ui->listAfterSorting->clear();

        for (int i = 0; i < nElem; i++)
        {
            outputLine.clear();
            for (int j = 0; j < nElem; j++)
            {
                outputLine.append(QString::number(arr.at(i).at(j)) + " ");
            }
            ui->listAfterSorting->addItem(outputLine);
        }

        ui->pushButton->setEnabled(true);
        ui->pushButton_4->setEnabled(true);
    }
    else
    {
        QMessageBox box(this);
        box.setText("arrayLength > 0");
        box.exec();
    }
}
