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
    void LoadFirstRulls();
    void LoadFirstAttr();
    void OpenFileToRead(QString str);
    void OpenFileToWriteAtrr(QString str);
    void OpenFileToWriteRulls(QString str);
    QVector<QStringList> SeparateRulls(QString str);
public:
    bool findTable(QString str);
    bool IsAttrEmpty(QString str);
    void CheckUserSearch(QStringList list);
};
#endif // MAINWINDOW_H
