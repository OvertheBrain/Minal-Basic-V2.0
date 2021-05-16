#include "expression.h"

Expression::Expression(){}

Expression::~Expression(){}

ConstantExp::~ConstantExp(){}

CompoundExp::~CompoundExp(){}

IdentifierExp::~IdentifierExp(){}

QString ConstantExp::getIdentifierName()
{
    return " ";
}

int ConstantExp::eval(evalstate &Vars) {
   return value;
}

QString ConstantExp::evalstr(evalstate &SVar){
    return strvalue;
}

ExpressionType ConstantExp::type()
{
    return CONSTANT;
}

IdentifierExp::IdentifierExp(QString name)
{
    this->name = name;
}

ExpressionType IdentifierExp::type()
{
    return IDENTIFIER;
}

QString IdentifierExp::getIdentifierName()
{
    return this->name;
}

int IdentifierExp::eval(evalstate &Vars)
{
    if(!Vars.isDefined(name))
    {
        QString error = "";
        error += name;
        error += " is not defined!";
        throw error;
    }

    return Vars.getValue(name);

}

QString IdentifierExp::evalstr(evalstate &SVars)
{
    if(!SVars.isDefinedstr(name))
    {
        QString error = "";
        error += name;
        error += " is not defined!";
        throw error;
    }
    else {
        return SVars.getstring(name);
    }
}


CompoundExp::CompoundExp(QString op, Expression *lhs, Expression *rhs)
{
    this->op = op;
    this->lhs = lhs;
    this->rhs = rhs;
}

ExpressionType CompoundExp::type()
{
    return COMPOUND;
}

int CompoundExp::eval(evalstate&Vars)
{
    if(rhs == nullptr)
    {
        QString error = "Illegal operator in expression , an operator must have right value!";
        throw error;
    }
    int right = rhs->eval(Vars);
    if (op == "=") {
        Vars.setValue(lhs->getIdentifierName(), right);
        return right;
    }
    if(lhs == nullptr)
    {
        QString t = "Illegal operator in expression , an operator must have left value!";
        throw t;
    }
    int left = lhs->eval(Vars);
    if (op == "+") return left + right;
    if (op == "-") return left - right;
    if (op == "*") return left * right;
    if (op == "/") {
          if (right == 0)
          {
              QString error = "Division by 0";
              throw(error);
          }
          return left / right;
       }
       QString error = "Illegal operator in expression";
       throw error;
}

QString CompoundExp::evalstr(evalstate &SVar){
    QString right = rhs->evalstr(SVar);
    if(rhs == nullptr)
    {
        QString error = "Illegal operator in expression , an operator must have right value!";
        throw error;
    }
    if(lhs == nullptr)
    {
        QString t = "Illegal operator in expression , an operator must have left value!";
        throw t;
    }

    SVar.setSTRValue(lhs->getIdentifierName(), right);
    return right;
}

Expression* CompoundExp::getLHS()
{
    return lhs;
}

Expression* CompoundExp::getRHS()
{
    return rhs;
}

QString CompoundExp::getOperator()
{
    return op;
}

QString CompoundExp::getIdentifierName()
{
    return " ";
}
