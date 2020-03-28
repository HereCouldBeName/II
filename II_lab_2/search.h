#ifndef SEARCH_H
#define SEARCH_H

#include <QDialog>

#include <QMessageBox>
#include <QDebug>

namespace Ui {
class Search;
}

class Search : public QDialog
{
    Q_OBJECT

public:
    explicit Search(QWidget *parent = nullptr);
    ~Search();
    void SetTextExpert(QString str);

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::Search *ui;
    QString lastSearch;
};

#endif // SEARCH_H
