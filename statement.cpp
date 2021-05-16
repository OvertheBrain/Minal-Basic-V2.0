#include "statement.h"

statement::statement()
{
    next = nullptr;
}

statement::statement(QString state,statement *n, int textnum)
{
    text = state;
    next = n;
    textline = textnum;
    lst = text.split(' ',QString::SkipEmptyParts);  //获取分段命令元素
    bool ok;
    lst[0].toInt(&ok);

    if(ok)
    {
        linenum = lst[0].toInt();   //如果是有行号的命令，行号存在linenum里
    }
    else
    {
        linenum = 0;    //如果是直接执行的命令，行号置为0
    }
}

void statement::cover(QString ntext)
{
    text = ntext;
    lst = text.split(' ',QString::SkipEmptyParts);
    linenum = lst[0].toInt();
}

int statement::getlinenum()
{
    return linenum;
}

QString statement::gettext()
{
    return text;
}
