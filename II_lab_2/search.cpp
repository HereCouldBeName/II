#include "search.h"
#include "ui_search.h"
#include "mainwindow.h"

Search::Search(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Search)
{
    ui->setupUi(this);
    ui->textEdit->setReadOnly(true);
}

Search::~Search()
{
    delete ui;
}

void Search::SetTextExpert(QString str)
{
    ui->textEdit->append(str);
}

void Search::on_pushButton_2_clicked()
{
    MainWindow *window = (MainWindow*)parent();

    window->ClearSearchInTable();

    this->close();
}

void Search::on_pushButton_clicked()
{
    QString strSearch = ui->searchLine->text();
    if (strSearch.size() == 0) {
        QMessageBox::information(nullptr,"error","Не введено выражение для поиска!");
        return;
    }

    MainWindow *window = (MainWindow*)parent();

    QStringList list;

    if(strSearch.toLower() == "да") {
        window->CheckUserSearch(list,1);
        return;
    }
    if(strSearch.toLower() == "нет") {
        window->CheckUserSearch(list,0);
        return;
    }
    if(strSearch.toLower() == "почему?") {
        window->CheckUserSearch(list,2);
        return;
    }

    /*Раздеяем строку на подстроки по ';'*/
    QRegExp rx(";");
    list = strSearch.split(rx);

    /*Проверяем каждый атрибут на наличие в списке*/
    foreach (QString str, list) {
        if (window->IsAttrEmpty(str)) {
            QMessageBox::information(nullptr,"error","Атрибута \"" + str +"\" не существует в списке атрибутов");
            return;
        }
    }
    /*Проверка поиска*/
    window->CheckUserSearch(list,0);
}
