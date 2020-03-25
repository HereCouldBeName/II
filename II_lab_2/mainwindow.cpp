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

    QStandardItemModel* model = new QStandardItemModel(nullptr);

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

void MainWindow::OpenFileToWrite(QString str)
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
    OpenFileToWrite(attr);
}
