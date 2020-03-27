#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /*модуль для правил*/

    OpenFileToRead(rules);

    LoadFirstRulls();

    /*модуль для атрибутов*/

    OpenFileToRead(attr);

    LoadFirstAttr();

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
    QVariantMap map;
    int row = model->rowCount();
    for(int i=0; i<row; i++) {
        qDebug() << model->item(i,0)->text() <<" " << model->item(i,1)->text();
        map.insert(model->item(i,0)->text(), model->item(i,1)->text());
    }
    file.write(QJsonDocument(QJsonObject::fromVariantMap(map)).toJson());
    file.close();
}

QVector<QStringList> MainWindow::SeparateRulls(QString str)
{
    /*разбиваем строки на подстроки, как только находим ИЛИ*/
    QRegExp rx("( ИЛИ )|( или )");
    QStringList listOr = str.split(rx);

    /*уже разбитые по ИЛИ строки разбиваем по И*/
    QVector<QStringList> vectorAnd;
    QRegExp rx2("( И )|( и )");
    foreach (QString str, listOr) {
        vectorAnd.push_back(str.split(rx2));
    }
    return vectorAnd;
}

/*Функция поиска по таблице*/
bool MainWindow::findTable(QString str)
{
    /*   QModelIndexList match;
        QModelIndex firstMatch;
        match = ui->table->model()->match(ui->table->model()->index(0,0),Qt::DisplayRole, str);
        firstMatch=match.value(0);*/

    int row = model->rowCount();
    for(int i=0; i<row; i++) {
        if(model->item(i,0)->text().toLower() == str.toLower())
            return true;
    }

    return false;
}

bool MainWindow::IsAttrEmpty(QString str)
{
    return ui->attr->findItems(str,Qt::MatchFixedString).empty();
}

void MainWindow::CheckUserSearch(QStringList list)
{
    /*Перебор всех правил*/
    int row = model->rowCount();
    QVector<QVector<QList<bool>>> fullres;
    for(int i=0; i<row; i++) {
        qDebug() << model->item(i,0)->text() <<" " << model->item(i,1)->text();
        QVector<QStringList> vectorAndRulls = SeparateRulls(model->item(i,0)->text());
        /*Сравниваем каждое подправило с поиском*/
        qDebug() << list;
        QVector<QList<bool>> res;
        res.resize(vectorAndRulls.size());
        int index = 0;
        foreach (QStringList listRull, vectorAndRulls) {
            foreach (QString rull, listRull) {
                bool isFind = false;
                foreach (QString str, list) {
                    if(str.toLower() == rull.toLower()) {
                        res[index].push_back(true);
                        isFind = true;
                        break;
                    }
                }
                if(!isFind) {
                    res[index].push_back(false);
                }
            }
            index++;
        }
        fullres.push_back(res);
    }

    struct ind {
        int indexMin1 = 0;
        int indexMin2 = 0;
        int indexMin3 = 0;
        int countFalse = 0;
    };

    ind curr;

    ind min;

    min.indexMin1 = 99999;
    min.indexMin2 = 99999;
    min.indexMin3 = 99999;
    min.countFalse = 99999;

    foreach (QVector<QList<bool>> rullfull, fullres) {
        curr.indexMin2 = 0;
        foreach (QList<bool> rullOr, rullfull) {
            curr.countFalse = 0;
            curr.indexMin3 = 0;
            foreach (bool el, rullOr) {
                if(!el) {
                    curr.countFalse++;
                    curr.indexMin3++;
                }
            }
            if(curr.countFalse < min.countFalse) {
                min = curr;
            }
            curr.indexMin2++;
        }
        curr.indexMin1++;
    }
    qDebug() << fullres;
    qDebug() << min.indexMin1 << " "<< min.indexMin2 << " " << min.indexMin3 << " "<<min.countFalse;
    if(min.countFalse == 0) {
        QString s = "---------------------------Заключение экспертной системы:---------------------------"
                    " \nПо вашему запросу найдено:\nПравило: "
                + model->item(min.indexMin1,0)->text() + "\nИз которого следует: "
                + model->item(min.indexMin1,1)->text() + "\n\n";
        searchWindow->SetTextExpert(s);
    }
}

/*добавление нового атрибута*/
void MainWindow::on_pushButton_4_clicked()
{
    qDebug() << ui->lineAttr->text();
    /*проверка на повторный атрибут*/
    if (IsAttrEmpty(ui->lineAttr->text())) {
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

    /*Разбиение на элементы по ИЛИ и И*/
    QVector<QStringList> vectorAnd = SeparateRulls(ui->ifLine->text());

    qDebug() << vectorAnd;

    /*Проверка на существование слов в АТРИБУТАХ*/
    foreach (QStringList StrList, vectorAnd) {
        foreach (QString str, StrList) {
            if (IsAttrEmpty(str)) {
                QMessageBox::information(nullptr,"error","Атрибут \"" + str + "\" не существует!");
                return;
            }
        }
    }

    /*проверка на повторное правило*/
    if (findTable(ui->ifLine->text())) {
        QMessageBox::information(nullptr,"error","Условие \"ЕСЛИ\" уже существует в списке правил");
        return;
    }

    /*Добавление атрибута в таблицу*/
    QStandardItem* item1 = new QStandardItem(ui->ifLine->text());
    QStandardItem* item2 = new QStandardItem(ui->elseLine->text());
    model->appendRow(QList<QStandardItem*>()<<item1<<item2);
}

/*Удалене правила*/
void MainWindow::on_pushButton_clicked()
{
    int count = ui->table->selectionModel()->selectedRows().count();
    for( int i = 0; i < count; i++) {
        ui->table->model()->removeRow( ui->table->selectionModel()->selectedRows().at( i).row(), QModelIndex());
    }
}

/*запись в файл правил*/
void MainWindow::on_pushButton_3_clicked()
{
    OpenFileToWriteRulls(rules);
}

/*
     * Вывод списка правил на момент загрузки
    */

void MainWindow::LoadFirstRulls()
{
    if(docErr.errorString().toInt() == QJsonParseError::NoError) {

        model = new QStandardItemModel(nullptr);
        model->setHorizontalHeaderLabels(QStringList() << "Если" << "То");
        ui->table->setModel(model);

        ui->table->resizeColumnsToContents();
        ui->table->resizeRowsToContents();

        ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);


        foreach (QString str, doc.object().keys()) {
            QStandardItem* item1 = new QStandardItem(str);
            QStandardItem* item2 = new QStandardItem(doc.object()[str].toString());
            model->appendRow(QList<QStandardItem*>()<<item1<<item2);

        }
    }
}

/*Вызов окна поиска*/
void MainWindow::on_pushButton_2_clicked()
{
    searchWindow = new Search(this);
    searchWindow->setModal(true);
    searchWindow->exec();
}
