#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /*
     * Работа с файлами
    */

    OpenFileToRead(rules);

    if(docErr.errorString().toInt() == QJsonParseError::NoError) {
        qDebug() << doc.object()["оранжевый и темно-зелёный лист"].toString();
    }

    OpenFileToRead(attr);

    LoadFirstAttr();

    model = new QStandardItemModel(nullptr);

    model->setHorizontalHeaderLabels(QStringList() << "Если" << "То");

    ui->table->setModel(model);

    ui->table->resizeColumnsToContents();
    ui->table->resizeRowsToContents();

    ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

}

MainWindow::~MainWindow()
{
    delete ui;
}

/*
 * Вывод списка атрибутов на момент загрузки
*/
void MainWindow::LoadFirstAttr()
{
    if(docErr.errorString().toInt() == QJsonParseError::NoError)
    {
        docArr = doc.array();
        qDebug() << docArr[0];

        foreach (QJsonValue item, docArr) {
            qDebug() << item.toString();
            ui->attr->addItem(item.toString());
        }
    }
}

void MainWindow::OpenFileToRead(QString str)
{
    file.setFileName(str);
    if(!file.open(QFile::Text | QIODevice::ReadOnly)) {
        QMessageBox::information(nullptr,"error","Файл " + str + " не открылся для чтения!!");
        return;
    }

    doc = QJsonDocument::fromJson(QByteArray(file.readAll()), &docErr);
    file.close();
}

void MainWindow::OpenFileToWriteAtrr(QString str)
{
    qDebug() << "OK";
    file.setFileName(str);
    if(!file.open(QIODevice::WriteOnly)) {
        QMessageBox::information(nullptr,"error","Файл " + str + " не открылся для записи!!");
        return;
    }
    qDebug() << "--------------------SAVE TO FILE!!!-------------------";
    QStringList list;
    for(int i = 0; i < ui->attr->count(); ++i)
    {
        list.push_back(ui->attr->item(i)->text());
    }
    QJsonArray arr = QJsonArray::fromStringList(list);

    QJsonDocument docWr;
    docWr.setArray(arr);

    file.write(docWr.toJson());
    file.close();
}

void MainWindow::OpenFileToWriteRulls(QString str)
{
    file.setFileName(str);
    if(!file.open(QIODevice::WriteOnly)) {
        QMessageBox::information(nullptr,"error","Файл " + str + " не открылся для записи!!");
        return;
    }
//    qDebug() << "--------------------SAVE TO FILE!!!-------------------";
//    QStringList list;
//    for(int i = 0; i < ui->attr->count(); ++i)
//    {
//        list.push_back(ui->attr->item(i)->text());
//    }
//    QJsonArray arr = QJsonArray::fromStringList(list);

//    QJsonDocument docWr;
//    docWr.setArray(arr);

//    file.write(docWr.toJson());
    //    file.close();
}

QModelIndex MainWindow::findTable(QString str)
{
    QModelIndexList match;
    QModelIndex firstMatch;
    match = ui->table->model()->match(ui->table->model()->index(0,0),Qt::DisplayRole, str);
    firstMatch=match.value(0);
    return firstMatch;
}

/*добавление нового атрибута*/
void MainWindow::on_pushButton_4_clicked()
{
    qDebug() << ui->lineAttr->text();
    /*проверка на повторный атрибут*/
    if( ui->attr->findItems(ui->lineAttr->text(),Qt::MatchFixedString).empty()) {
        ui->attr->addItem(ui->lineAttr->text());
    }
}

/*удаление атрибута*/
void MainWindow::on_pushButton_5_clicked()
{
    delete ui->attr->takeItem(ui->attr->currentRow());
}

/*сохранение артибутов*/
void MainWindow::on_pushButton_6_clicked()
{
    OpenFileToWriteAtrr(attr);
}


/*добавление нового правила*/
void MainWindow::on_pushButton_7_clicked()
{
    if (ui->ifLine->text().size() == 0 || ui->elseLine->text().size() == 0) {
        QMessageBox::information(nullptr,"error","Введите обе части правила!");
        return;
    }
    /*разбиваем строки на подстроки, как только находим ИЛИ*/
    QRegExp rx("( ИЛИ )|( или )");
    QStringList listOr = ui->ifLine->text().split(rx);

    /*уже разбитые по ИЛИ строки разбиваем по И*/
    QVector<QStringList> vectorAnd;
    QRegExp rx2("( И )|( и )");
    foreach (QString str, listOr) {
        vectorAnd.push_back(str.split(rx2));
    }

    /*Проверка на существование слов в АТРИБУТАХ*/
    foreach (QStringList StrList, vectorAnd) {
        foreach (QString str, StrList) {
            if (ui->attr->findItems(str,Qt::MatchFixedString).empty()) {
                QMessageBox::information(nullptr,"error","Атрибут \"" + str + "\" не существует!");
                return;
            }
        }
    }

    /*проверка на повторный атрибут*/
    if (findTable(ui->ifLine->text()).isValid()) {
        QMessageBox::information(nullptr,"error","Условие \"ЕСЛИ\" уже существует в списке правил");
        return;
    }

    /*Добавление атрибута в таблицу*/
    QStandardItem* item1 = new QStandardItem(ui->ifLine->text());
    QStandardItem* item2 = new QStandardItem(ui->elseLine->text());
    model->appendRow(QList<QStandardItem*>()<<item1<<item2);
}
