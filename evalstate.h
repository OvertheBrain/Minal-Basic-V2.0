#ifndef EVALSTATE_H
#define EVALSTATE_H

#include<map>
#include<QString>

using namespace std;

class evalstate{

public:
    evalstate();
    void setValue(QString var,int value);
    void setSTRValue(QString var, QString value);  //保存字符变量
    int getValue (QString var);
    QString getstring(QString var);
    bool isDefined (QString var);
    bool isDefinedstr (QString var);
    void clear();
    QString Tranvrese();      //遍历

private:
     map<QString,int> VarMap;
     map<QString,QString> SVarMap;
     QString contaning;     
};

#endif // EVALSTATE_H
