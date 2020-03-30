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

void MainWindow::MarkRulls(int row, Qt::GlobalColor color)
{
    for(int i=0; i<model->rowCount(); i++) {
        model->setData(model->index(i,0),QBrush(Qt::white),Qt::BackgroundRole);
        model->setData(model->index(i,1),QBrush(Qt::white),Qt::BackgroundRole);
    }
    if(color != Qt::white) {
        model->setData(model->index(row,0),QBrush(color), Qt::BackgroundRole);
        model->setData(model->index(row,1),QBrush(color), Qt::BackgroundRole);
    }
}

/*Функция поиска по таблице*/
bool MainWindow::findTable(QString str)
{
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

void MainWindow::CheckUserSearch(QStringList list, int option)
{
    if(list.isEmpty()) {
        list = lastRequest;

        if(option == 0) {
            errorRulls.push_back(answerRull);
        } else if(option == 1){
            list.push_back(answerRull);
        } else {
            MarkRulls(min.indexMin1, Qt::yellow);
            QString s = "---------------------------Ответ экспертной системы:---------------------------"
                        " \nПроверка по правилу, где:\nПравило: "
                    + model->item(min.indexMin1,0)->text() + "\nДолжность: "
                    + model->item(min.indexMin1,1)->text() + "\n\n";
            searchWindow->SetTextExpert(s);
        }
    }
    lastRequest = list;

    /*Перебор всех правил*/
    int row = model->rowCount();
    QVector<QVector<QList<int>>> fullres;
    for(int i=0; i<row; i++) {
        qDebug() << model->item(i,0)->text() <<" " << model->item(i,1)->text();
        QVector<QStringList> vectorAndRulls = SeparateRulls(model->item(i,0)->text());
        /*Сравниваем каждое подправило с поиском*/
        qDebug() << list;
        QVector<QList<int>> res;
        res.resize(vectorAndRulls.size());
        int index = 0;
        foreach (QStringList listRull, vectorAndRulls) {
            foreach (QString rull, listRull) {
                bool isErr = false;
                foreach (QString err, errorRulls) {
                    if(err.toLower() == rull.toLower()) {
                        res[index].push_back(2); // НИКОГДА не ВЫПОЛНИТСЯ
                        isErr = true;
                        break;
                    }
                }
                if(!isErr) {
                    bool isFind = false;
                    foreach (QString str, list) {
                        if(str.toLower() == rull.toLower()) {
                            res[index].push_back(1); //найдено
                            isFind = true;
                            break;
                        }
                    }
                    if(!isFind) {
                        res[index].push_back(0); // не найдено
                    }
                }
            }
            index++;
        }
        fullres.push_back(res);
    }

    ind curr;

    min.countFalse = 99999;

    foreach (QVector<QList<int>> rullfull, fullres) {
        curr.indexMin2 = 0;
        foreach (QList<int> rullOr, rullfull) {
            curr.countFalse = 0;
            bool notTake = false;
            foreach (int el, rullOr) {
                if(el == 0) {
                    curr.countFalse++;
                } else if(el == 2) {
                    notTake = true;
                    break;
                }
            }
            if(curr.countFalse < min.countFalse && !notTake) {
                min = curr;
            }
            curr.indexMin2++;
        }
        curr.indexMin1++;
    }
    qDebug() << fullres;
    qDebug() << min.indexMin1 << " "<< min.indexMin2 <<" "<< min.countFalse;
    if(min.countFalse == 0) {
        MarkRulls(min.indexMin1, Qt::green);
        QString s = "---------------------------Заключение экспертной системы:---------------------------"
                    " \nПо вашему запросу найдено:\nПравило: "
                + model->item(min.indexMin1,0)->text() + "\nИз которого следует что вы подходите на должность: "
                + model->item(min.indexMin1,1)->text() + "\n\n";
        searchWindow->SetTextExpert(s);
        return;
    }
    if(min.countFalse == 99999) {
        QString s = "---------------------------Заключение экспертной системы:---------------------------"
                    "\nВы не подходите ни на одну из должностей";
        searchWindow->SetTextExpert(s);
        return;
    }

    int i = 0;
    foreach (int val, fullres[min.indexMin1][min.indexMin2]) {
        if(val == 0) {
            QVector<QStringList> vectorAndRulls = SeparateRulls(model->item(min.indexMin1,0)->text());
            answerRull = vectorAndRulls[min.indexMin2][i];
            QString s = "---------------------------Вопрос от экспертной системы:---------------------------"
                       "\nВерно ли, что вы владеете \"" + answerRull + "\"?";
            qDebug() << s;
            searchWindow->SetTextExpert(s);
            return;
        }
        i++;
    }
}

void MainWindow::ClearSearchInTable()
{
    MarkRulls(0, Qt::white);
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


    int row = model->rowCount();
    for(int i=0; i<row; i++) {
        foreach (QStringList StrList, vectorAnd) {
            QVector<QStringList> vectorAndRulls = SeparateRulls(model->item(i,0)->text());
            foreach(QStringList ListRulls, vectorAndRulls) {
                int countTrue = 0;
                if(StrList.size() <= ListRulls.size()) {
                    foreach (QString strFind, StrList) {
                        bool isFind = false;
                        foreach (QString rullStr, ListRulls) {
                            if(strFind.toLower() == rullStr.toLower()) {
                                isFind = true;
                                countTrue++;
                                break;
                            }
                        }
                        if(!isFind) {
                            break;
                        }
                    }
                    if(countTrue >= StrList.size()) {
                        QString txt;
                        MarkRulls(i,Qt::red);
                        foreach (QString strFind, StrList) {
                            txt += " " + strFind;
                        }
                        QMessageBox::information(nullptr,"Предупреждение","Условие \""
                                                 +txt+
                                                 "\" уже существует в списке правил, добавление не возможно");
                        return;
                    }
                }
            }
        }
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
