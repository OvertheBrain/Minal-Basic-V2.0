#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "program.h"
#include "evalstate.h"
#include "tokenizer.h"
#include "parser.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void Run(evalstate &Vars,statement *cur);
    void SyntaxtreeDisplay(statement *cmd);
    bool IfCondition(QString str);
    void highLightErrorCode();

private slots:
    void on_lineEdit_returnPressed();

    void on_pushButton_3_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::MainWindow *ui;
    program *pros;  //程序命令集
    evalstate Vars;
    tokenizer *Tokenizer;
    parser *Parser;
    QString inputvar;
    int inputcondition=0;  //记录是否INPUT
    statement *curstatement;  //记录debug时正在运行的指令
    bool debugging=0;    //记录是否在debug模式
    QVector<int> wrongLine;
};
#endif // MAINWINDOW_H
