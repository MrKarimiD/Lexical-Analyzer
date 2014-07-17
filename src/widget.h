#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "qstring.h"
#include "qdebug.h"


#define LE_state 2
#define NE_state 3
#define LT_state 4
#define EQ_state 5
#define GE_state 7
#define GT_state 8

struct Token
{
    QString name;
    int attribute;
};

struct symbol_table_rows
{
    int address;
    QString string;
    QString value;
};

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    int findAddress(QString input);
    
private slots:
    void on_open_toolButton_clicked();

private:
    Ui::Widget *ui;
    QString input_string;
    struct symbol_table_rows symbol_table[2000];
    int numberOfRows;
    bool syntaxWrong;
    int line;
    int word;
};

#endif // WIDGET_H
