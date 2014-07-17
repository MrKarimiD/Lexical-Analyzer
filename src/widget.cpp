#include "widget.h"
#include "ui_widget.h"
#include "QString"
#include "QFileDialog"
#include "QIODevice"
#include "QMessageBox"
#include "exception"


struct symbol_table_rows symbol_table[2000];

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    syntaxWrong=false;
    numberOfRows=0;
    line=0;
    word=0;

    //--------Key Words----------------------
    symbol_table[0].address=0;
    symbol_table[0].string="if";
    symbol_table[0].value="---";

    symbol_table[1].address=1;
    symbol_table[1].string="then";
    symbol_table[1].value="---";

    symbol_table[2].address=2;
    symbol_table[2].string="else";
    symbol_table[2].value="---";

    symbol_table[3].address=3;
    symbol_table[3].string="main";
    symbol_table[3].value="---";

    symbol_table[4].address=4;
    symbol_table[4].string="int";
    symbol_table[4].value="---";

    symbol_table[5].address=5;
    symbol_table[5].string="float";
    symbol_table[5].value="---";

    symbol_table[6].address=6;
    symbol_table[6].string="switch";
    symbol_table[6].value="---";

    symbol_table[7].address=7;
    symbol_table[7].string="case";
    symbol_table[7].value="---";

    symbol_table[8].address=8;
    symbol_table[8].string="void";
    symbol_table[8].value="---";

    symbol_table[9].address=9;
    symbol_table[9].string="char";
    symbol_table[9].value="---";

    symbol_table[10].address=10;
    symbol_table[10].string="for";
    symbol_table[10].value="---";

    symbol_table[11].address=11;
    symbol_table[11].string="break";
    symbol_table[11].value="---";

    symbol_table[12].address=12;
    symbol_table[12].string="return";
    symbol_table[12].value="---";

    symbol_table[13].address=13;
    symbol_table[13].string="continue";
    symbol_table[13].value="---";

    numberOfRows=14;

    //---------------------------------------
}

Widget::~Widget()
{
    delete ui;
}

int Widget::findAddress(QString input)
{
    int temp=-1;
    for(int i=0;i<numberOfRows;i++)
    {
        if(symbol_table[i].string==input)
        {
            temp=symbol_table[i].address;
            break;
        }
    }
    if(temp==-1)
    {
        symbol_table[numberOfRows].address=numberOfRows;
        symbol_table[numberOfRows].string=input;
        temp=numberOfRows;
        numberOfRows++;
    }
    return temp;
}


void Widget::on_open_toolButton_clicked()
{
    syntaxWrong=false;
    QString fileAddress = QFileDialog::getOpenFileName(this,tr("Select Your Source File"), "/home", tr("Text Files (*.txt)"));
    ui->path_lineEdit->setText(fileAddress);
    QFile source_file(fileAddress);
    if(!source_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox msgBox;
        msgBox.setText("Error!  You Should Select a File!");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
    }
    else
    {
        input_string=source_file.readAll();
        ui->source_textEdit->setText(input_string);
    }

    struct Token tokens[20000];
    int numberOfTokens=0;

    int state=0;
    QString temp;
    QChar c;
    int counter=0;
    while(true)
    {
        if(counter==input_string.length())
        {
          break;
        }
        else
        {
            switch (state)
            {
            case 0:
                c=input_string[counter];
                if( c == '<')
                {
                    state=1;
                }
                else if( c == '=')
                {
                    state=5;
                }
                else if( c == '>')
                {
                    state=6;
                }
                else if( ( c>='a' && c<='z') || ( c>='A' && c<='Z'))
                {
                    state=10;
                    temp.append(c);
                }
                else if( c>='0' && c<='9')
                {
                    state=13;
                    temp.append(c);
                }
                else if( (c == ' ') || (c == '\n') || (c == '\t') )
                {
                    state=23;
                }
                else if(c=='{')  state=25;
                else if(c=='}')  state=26;
                else if(c=='(')  state=27;
                else if(c==')')  state=28;
                else if(c==';')  state=29;
                counter++;
                break;
            case 1:
                c=input_string[counter];
                if(c=='=')  state=2;
                else if(c=='>')  state=3;
                else {state=4;}
                counter++;
                break;
            case 2:
                tokens[numberOfTokens].name="relop";
                tokens[numberOfTokens].attribute=LE_state;
                numberOfTokens++;
                state=0;
                break;
            case 3:
                tokens[numberOfTokens].name="relop";
                tokens[numberOfTokens].attribute=NE_state;
                numberOfTokens++;
                state=0;
                break;
            case 4:
                tokens[numberOfTokens].name="relop";
                tokens[numberOfTokens].attribute=LT_state;
                counter--;
                numberOfTokens++;
                state=0;
                break;
            case 5:
                tokens[numberOfTokens].name="relop";
                tokens[numberOfTokens].attribute=EQ_state;
                numberOfTokens++;
                state=0;
                break;
            case 6:
                c=input_string[counter];
                if( c == '=')  state=7;
                else
                {
                    state=8;
                }
                counter++;
                break;
            case 7:
                tokens[numberOfTokens].name="relop";
                tokens[numberOfTokens].attribute=GE_state;
                numberOfTokens++;
                state=0;
                break;
            case 8:
                tokens[numberOfTokens].name="relop";
                tokens[numberOfTokens].attribute=GT_state;
                numberOfTokens++;
                counter--;
                state=0;
                break;
            case 10:
                c=input_string[counter];
                if(( c>='a' && c<='z') || ( c>='A' && c<='Z') || ( c>='0' && c<='9') || (c=='_'))
                {
                    state=10;
                    temp.append(c);
                }
                else
                {
                    state=11;
                }
                counter++;
                break;
            case 11:
                counter--;
                tokens[numberOfTokens].name="id";
                tokens[numberOfTokens].attribute=findAddress(temp);
                temp.clear();
                numberOfTokens++;
                state=0;
                break;
            case 13:
                c=input_string[counter];
                if( c>='0' && c<='9')
                {
                    state=13;
                    temp.append(c);
                }
                else if(c=='.')
                {
                    state=14;
                    temp.append(c);
                }
                else if(c=='E')
                {
                    state=16;
                    temp.append(c);
                }
                else
                {
                    state=20;
                }
                counter++;
                break;
            case 14:
                c=input_string[counter];
                if( c>='0' && c<='9' )
                {
                    state=15;
                    temp.append(c);
                }
                else
                {
                    syntaxWrong=true;
                    QMessageBox msgBox;
                    QString error_str("Syntax Error at line ");
                    error_str.append(QString::number(line));
                    error_str.append(" word ");
                    error_str.append(QString::number(word));
                    error_str.append("!");
                    msgBox.setText(error_str);
                    msgBox.setIcon(QMessageBox::Critical);
                    msgBox.exec();
                }
                counter++;
                break;
            case 15:
                c=input_string[counter];
                if( c>='0' && c<='9' )
                {
                    state=15;
                    temp.append(c);
                }
                else if(c=='E')
                {
                    state=16;
                    temp.append(c);
                }
                else
                {
                    state=21;
                }
                counter++;
                break;
            case 16:
                c=input_string[counter];
                if( c>='0' && c<='9' )
                {
                    state=18;
                    temp.append(c);
                }
                else if( c=='+' || c=='-')
                {
                    state=17;
                    temp.append(c);
                }
                counter++;
                break;
            case 17:
                c=input_string[counter];
                if( c>='0' && c<='9' )
                {
                    state=18;
                    temp.append(c);
                }
                else
                {
                    syntaxWrong=true;
                    QMessageBox msgBox;
                    QString error_str("Syntax Error at line ");
                    error_str.append(QString::number(line));
                    error_str.append(" word ");
                    error_str.append(QString::number(word));
                    error_str.append("!");
                    msgBox.setText(error_str);
                    msgBox.setIcon(QMessageBox::Critical);
                    msgBox.exec();
                }
                break;
            case 18:
                c=input_string[counter];
                if( c>='0' && c<='9' )
                {
                    state=18;
                    temp.append(c);
                }
                else
                {
                    state=19;
                    temp.append(c);
                }
                counter++;
                break;
            case 19:
                counter--;
                tokens[numberOfTokens].name="number";
    //                tokens[numberOfTokens].attribute=;
                temp.clear();
                numberOfTokens++;
                state=0;
                break;
            case 20:
                counter--;
                tokens[numberOfTokens].name="number";
    //                tokens[numberOfTokens].attribute=;
                temp.clear();
                numberOfTokens++;
                state=0;
                break;
            case 21:
                counter--;
                tokens[numberOfTokens].name="number";
    //                tokens[numberOfTokens].attribute=;
                temp.clear();
                numberOfTokens++;
                state=0;
                break;
            case 23:
                c=input_string[counter];
                if( (c == ' ') || (c == '\n') || (c == '\t') )
                {
                    state=23;

                    if(c == '\n')
                    {
                        line++;
                        word=0;
                    }
                    else if( (c == ' ') || (c == '\t') )
                    {
                        word++;
                    }
                }
                else
                {
                    state=24;
                }
                counter++;
                break;
            case 24:
                counter--;
                state=0;
                break;
            case 25:
                tokens[numberOfTokens].name="right_block_op";
                tokens[numberOfTokens].attribute=-1;
                temp.clear();
                numberOfTokens++;
                state=0;
                break;
            case 26:
                tokens[numberOfTokens].name="left_block_op";
                tokens[numberOfTokens].attribute=-1;
                temp.clear();
                numberOfTokens++;
                state=0;
                break;
            case 27:
                tokens[numberOfTokens].name="right_par_op";
                tokens[numberOfTokens].attribute=-1;
                temp.clear();
                numberOfTokens++;
                state=0;
                break;
            case 28:
                tokens[numberOfTokens].name="left_par_op";
                tokens[numberOfTokens].attribute=-1;
                temp.clear();
                numberOfTokens++;
                state=0;
                break;
            case 29:
                tokens[numberOfTokens].name="end_cmd_op";
                tokens[numberOfTokens].attribute=-1;
                temp.clear();
                numberOfTokens++;
                state=0;
                break;
            }
        }

        if(syntaxWrong)
            break;
    }
    QString test;
    if(!syntaxWrong)
    {
        for(int i=0;i<numberOfTokens;i++)
        {
            test.append("<"+tokens[i].name+">\n");
        }
        ui->token_textEdit->setText(test);

        test.clear();
    }

//    test.append("Address\tValue\n");

//    for(int i=0;i<numberOfRows;i++)
//    {
//        test.append(symbol_table[i].address+"\t"+symbol_table[i].string+"\n");
//    }

//    ui->symbol_textEdit->setText(test);
}























