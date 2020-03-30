#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>

#include <QFile>
#include <QFileDialog>

#include <QStandardItem>

#include <QMessageBox>
#include <QDebug>
#include <QtGui>

#include "search.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString rules = "C:/Users/Comp026/Documents/II_lab_2/db/rules.json";
    QString attr  = "C:/Users/Comp026/Documents/II_lab_2/db/attr.json";


    QJsonDocument docRul;
    QJsonArray docArRul;
    QJsonParseError docErrRul;

    QJsonDocument docAttr;
    QJsonArray docArAttr;
    QJsonParseError docErrAttr;

    QJsonDocument doc;
    QJsonArray docArr;
    QJsonParseError docErr;


    QString path;
    QFile file;

private slots:

    void on_pushButton_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    QStandardItemModel* model;
    Search* searchWindow;
    QStringList errorRulls;
    QStringList lastRequest;
    QString answerRull;
    void LoadFirstRulls();
    void LoadFirstAttr();
    void OpenFileToRead(QString str);
    void OpenFileToWriteAtrr(QString str);
    void OpenFileToWriteRulls(QString str);
    QVector<QStringList> SeparateRulls(QString str);

    struct ind {
        int indexMin1 = 0;
        int indexMin2 = 0;
        int countFalse = 0;
    };

    ind min;
    void MarkRulls(int row, Qt::GlobalColor color);

public:
    bool findTable(QString str);
    bool IsAttrEmpty(QString str);
    /*
     * option = 1 - пользователь подтвердил
     * option = 0 - пользователь опроверг
     * option = 2 - вопрос от пользователя
    */
    void CheckUserSearch(QStringList list, int option);
    void ClearSearchInTable();
};
#endif // MAINWINDOW_H
