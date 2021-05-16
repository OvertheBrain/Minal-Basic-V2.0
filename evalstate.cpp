#include "evalstate.h"

evalstate::evalstate(){}

void evalstate::setValue(QString var, int value)
{
    VarMap[var] = value;
}

void evalstate::setSTRValue(QString var, QString value)
{
    SVarMap[var] = value;
}

int evalstate::getValue(QString var)
{
    return VarMap[var];
}

QString evalstate::getstring(QString var)
{
    return SVarMap[var];
}

void evalstate::clear()
{
    VarMap.clear();
}

bool evalstate::isDefined(QString var)
{    
    return VarMap.count(var);
}

bool evalstate::isDefinedstr(QString var)
{
    return SVarMap.count(var);
}

QString evalstate::Tranvrese()
{
    contaning = "";
    map<QString,int>::iterator iter;
        iter = VarMap.begin();
        while(iter != VarMap.end()){
            contaning += iter->first;
            contaning += ": INT=";
            contaning += QString::number(iter->second);
            contaning += "\n";
            iter++;
        }
    map<QString,QString>::iterator iter2;   //字符变量遍历
        iter2 = SVarMap.begin();
        while(iter2 != SVarMap.end()){
            contaning += iter2->first;
            contaning += ": STR= ";
            contaning += iter2->second;
            contaning += "\n";
            iter2++;
        }

    return contaning;
}

