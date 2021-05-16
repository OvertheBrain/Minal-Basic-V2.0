#ifndef PARSER_H
#define PARSER_H

# include"expression.h"
# include<QList>
# include<stack>


class parser
{
public:
    parser();
    Expression * ToTree(QList<QString> tokens);

private:
    stack<QString> Operators;
    stack<Expression*> Operands;

    bool IsCompoundexp(QString);
    bool IsConstantexp(QString);
    bool IsIdentifierexp(QString);

    int GetPriority(QString ope);  //优先级


    bool IsMulDiv(QString); //是不是*/
    bool IsAddSub(QString); //是不是+-
    bool IsEqu(QString);
    bool IsLeftpar(QString);
    bool IsRightpar(QString);
};

#endif // PARSER_H
