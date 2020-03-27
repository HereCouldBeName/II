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
    this->close();
}

void Search::on_pushButton_clicked()
{
    if (ui->searchLine->text().size() == 0) {
        QMessageBox::information(nullptr,"error","Не введено выражение для поиска!");
        return;
    }

    /*Раздеяем строку на подстроки по ';'*/
    QRegExp rx(";");
    QStringList list = ui->searchLine->text().split(rx);

    /*Проверяем каждый атрибут на наличие в списке*/
    MainWindow *window = (MainWindow*)parent();
    foreach (QString str, list) {
        if (window->IsAttrEmpty(str)) {
            QMessageBox::information(nullptr,"error","Атрибута \"" + str +"\" не существует в списке атрибутов");
            return;
        }
    }
    /*получаем список правил*/
    window->CheckUserSearch(list);
}
