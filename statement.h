#ifndef STATEMENT_H
#define STATEMENT_H
#include<QString>
#include<string.h>
#include<QList>

using namespace std;

class statement
{
    friend class MainWindow;
    friend class program;
public:
    statement();
    statement(QString state,statement*n = nullptr,int textnum=0);
    void cover(QString ntext);
    int getlinenum();
    QString gettext();
    int textline;   //记录代码显示框中所处行号

private:
    int linenum = 0;
    QString text;
    statement *next;
    QList<QString> lst;
};

#endif // STATEMENT_H
