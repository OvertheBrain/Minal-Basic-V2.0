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
            if(IsLeftpar(tmp))                 //??????????????????????????????
            {
                Operators.push(tmp);
            }
            else if (IsRightpar(tmp)) {        //???????????????????????????????????????????????????
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
            else     //??????????????????
            {
                if(Operators.empty())            //???????????????????????????????????????
                {
                    Operators.push(tmp);
                }
                else
                {
                    int toppri = GetPriority(Operators.top());      //????????????????????????
                    int curpri = GetPriority(tmp);            //??????????????????????????????
                    if(curpri>toppri)
                    {
                        Operators.push(tmp);          //??????????????????????????????????????????????????????????????????
                    }
                    else
                    {
                        QString top = Operators.top();
                        Operators.pop();                   //????????????????????????????????????????????????????????????????????????????????????
                        Operators.push(tmp);

                        Expression*lef;
                        Expression*rig;

                        if(!Operands.empty())               //??????Operands?????????????????????????????????????????????????????????
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
                        CompoundExp*comp = new CompoundExp(top,lef,rig);       //??????compound expression ????????????Operands??????
                        Operands.push(comp);
                    }
                }
            }
        }
    }

    //?????????????????????????????????????????????????????????
    while(!Operators.empty())
    {
        QString top = Operators.top();
        Operators.pop();

        Expression *rig;
        Expression *lef;
        if(!Operands.empty())               //??????Operands?????????????????????????????????????????????????????????
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
        CompoundExp *comp = new CompoundExp(top,lef,rig);       //??????compound expression ????????????Operands???
        Operands.push(comp);
    }

    root = Operands.top(); //???????????????Operands???????????????????????????????????????
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
    if(exp[0]>="A" && exp[0]<="Z")          //??????????????????????????????
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
    return 0;              //??????0?????????
}
