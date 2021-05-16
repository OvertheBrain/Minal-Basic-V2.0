#include "parser.h"

parser::parser(){}

Expression * parser::ToTree(QList<QString>tokens)
{
    for(int i = 0;i<tokens.length();++i)
    {
        if(tokens[i] == "")
        {
            tokens.removeAt(i);
        }
    }

    if(tokens[0] == "-")
    {
        tokens.insert(0,"0");
    }

    for(int i = 0;i<tokens.length();++i)
    {
        int j = i+1;
        if(tokens[i] == "(" && tokens[j] == "-")
            tokens.insert(j,"0");
    }



    QString tokensstr = "";
    for(int i = 0;i<tokens.length();++i)
    {
        if(tokens[i] == "+"||tokens[i] == "-"||tokens[i] == "*"||tokens[i] == "/"||tokens[i] == "=")
        {
            tokensstr += "+";
        }
        else {
            tokensstr += tokens[i];
        }
    }

    if(tokensstr.contains("++"))
    {
        QString T = "Illegal operator in expression , an operator must have left value and right value!";
        throw (T);
    }

    Expression *root = nullptr;
    QString tmp;
    for(int i = 0;i<tokens.length();++i)
    {
        tmp = tokens[i];
        if(IsConstantexp(tmp))
        {
            ConstantExp *cons;
            if(tmp.contains("'")||tmp.contains("\"")){
                QString exp = "";
                string strexp = tmp.toStdString();
                string tmpexp;
                tmpexp = strexp.substr(1,strexp.length()-2);
                exp=QString::fromStdString(tmpexp);
                if(exp.contains("'")||exp.contains("\""))
                {
                    QString T = "There are Quotation marks in the string";
                    throw (T);
                }
                cons = new ConstantExp(exp);
            }
            else cons=new ConstantExp(tmp.toInt());
            Operands.push(cons);
        }
        if(IsIdentifierexp(tmp))
        {
            IdentifierExp*iden = new IdentifierExp(tmp);
            Operands.push(iden);
        }
        if(IsCompoundexp(tmp))
        {
            if(IsLeftpar(tmp))                 //如果是左括号直接进栈
            {
                Operators.push(tmp);
            }
            else if (IsRightpar(tmp)) {        //右括号的话，括号里面的符号都要处理
                while(Operators.top()!="(")
                {
                    QString top = Operators.top();
                    Operators.pop();

                    Expression *lef;
                    Expression *rig;

                    if(!Operands.empty())
                    {
                        rig = Operands.top();
                        Operands.pop();
                    }
                    else
                    {
                        rig = nullptr;
                    }

                    if(!Operands.empty())
                    {
                        lef = Operands.top();
                        Operands.pop();
                    }
                    else
                    {
                        lef = nullptr;
                    }
                    CompoundExp*comp = new CompoundExp(top,lef,rig);
                    Operands.push(comp);
                }
                Operators.pop();
            }
            else     //如果不是括号
            {
                if(Operators.empty())            //如果符号栈是空的，就直接进
                {
                    Operators.push(tmp);
                }
                else
                {
                    int toppri = GetPriority(Operators.top());      //获得栈顶的优先级
                    int curpri = GetPriority(tmp);            //获得当前符号的优先级
                    if(curpri>toppri)
                    {
                        Operators.push(tmp);          //如果当前符号优先级大于栈顶的优先级，那么进栈
                    }
                    else
                    {
                        QString top = Operators.top();
                        Operators.pop();                   //如果栈顶元素的优先级大于当前符号，就退栈再让当前元素进栈
                        Operators.push(tmp);

                        Expression*lef;
                        Expression*rig;

                        if(!Operands.empty())               //取出Operands栈顶两个元素，如果是空的话就返回空指针
                        {
                            rig = Operands.top();
                            Operands.pop();
                        }
                        else
                        {
                            rig = nullptr;
                        }

                        if(!Operands.empty())
                        {
                            lef = Operands.top();
                            Operands.pop();
                        }
                        else
                        {
                            lef = nullptr;
                        }
                        CompoundExp*comp = new CompoundExp(top,lef,rig);       //生成compound expression 然后再进Operands栈里
                        Operands.push(comp);
                    }
                }
            }
        }
    }

    //都进完栈了，但是栈里面可能有东西没出来
    while(!Operators.empty())
    {
        QString top = Operators.top();
        Operators.pop();

        Expression *rig;
        Expression *lef;
        if(!Operands.empty())               //取出Operands栈顶两个元素，如果是空的话就返回空指针
        {
            rig = Operands.top();
            Operands.pop();
        }
        else
        {
            rig = nullptr;
        }

        if(!Operands.empty())
        {
            lef = Operands.top();
            Operands.pop();
        }
        else
        {
            lef = nullptr;
        }
        CompoundExp *comp = new CompoundExp(top,lef,rig);       //生成compound expression 然后再进Operands栈
        Operands.push(comp);
    }

    root = Operands.top(); //处理完后，Operands栈中剩下一棵完整的表达式树
    return root;
}

bool parser::IsCompoundexp(QString exp)
{
    if(exp == "+" || exp == "-" || exp == "*" || exp == "/" || exp == "(" || exp == ")" || exp == "=")
    {
        return true;
    }
    return false;
}

bool parser::IsConstantexp(QString exp)
{
    bool ok=0;
    exp.toInt(&ok);
    if(ok)return ok;
    else if(exp.contains("'")||exp.contains("\""))return 1;
    else return false;
}

bool parser::IsEqu(QString exp)
{
    if (exp =="=")
        return true;
    return false;
}

bool parser::IsAddSub(QString exp)
{
    if(exp == "+" || exp == "-")
        return true;
    return false;
}

bool parser::IsMulDiv(QString exp)
{
    if(exp == "*" || exp == "/")
        return true;
    return false;
}

bool parser::IsLeftpar(QString exp)
{
    if(exp == "(")
        return true;
    return false;
}

bool parser::IsRightpar(QString exp)
{
    if(exp == ")")
        return true;
    return false;
}

bool parser::IsIdentifierexp(QString exp)
{
    if(exp[0]>="A" && exp[0]<="Z")          //变量的只能以字母开头
        return true;
    if(exp[0]>="a" && exp[0]<="z")
        return true;
    return false;
}

int parser::GetPriority(QString ope)
{
    if (ope =="=")
        return 1;
    else if (ope == "+"||ope =="-") {
        return 2;
    }
    else if (ope == "*" || ope =="/") {
        return 3;
    }
    return 0;              //返回0为异常
}
