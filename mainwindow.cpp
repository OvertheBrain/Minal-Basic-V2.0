#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QList>
#include <QString>
#include "program.h"
#include "expression.h"
#include <QTextBlock>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    pros = new program();
    Parser = new parser;
    Tokenizer = new tokenizer;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_lineEdit_returnPressed()  //指令输入，并进行初步分析
{
    QString cmd=ui->lineEdit->text();
    QList<QString> tmplist = cmd.split(' ',QString::SkipEmptyParts);

    ui->lineEdit->clear();
    bool ok=0;

    if(cmd == nullptr||cmd == " ")return;

    if(inputcondition>0)  //上一条语句是INPUT或INPUTS的情况
    {
        int num=cmd.toInt(&ok);
        if(inputcondition==2){
            if(ok){
                ui->textBrowser_2->append("Not a string!");
                return;
            }
            else{
                Vars.setSTRValue(inputvar,cmd);
                inputcondition = 0;
                ui->textBrowser_2->append(cmd);
                return;
            }

         }
        else{
            if(ok)
               {
                    Vars.setValue(inputvar,num);
                    inputcondition = 0;
                    ui->textBrowser_2->append(cmd);
                    return;
               }
            else{
                    ui->textBrowser_2->append("Not a number!");
                    return;
                }
        }
    }

    if(cmd == "HELP")
    {
        ui->textBrowser_3->append("帮助：Basic-doc.pdf（请不要使用中文注释和变量名）;");
        ui->textBrowser_3->append("命令格式：行号 操作 表达式（需要空格）;");
        ui->textBrowser_3->append("输入完每句命令后回车上传，写完后以END结尾，点击RUN按钮执行;");
        ui->textBrowser_3->append("INPUT操作1：行号 INPUT 变量名，回车后右上显示框出现？，随后在输入栏中写入变量值;");
        ui->textBrowser_3->append("INPUT操作2：INPUT需要加行号，但不计入实际指令，所以指令顺序以代码显示框为准;");
        ui->textBrowser_3->append("删改操作：只输入一个行号为删除该行，若行号不存在视为无事发生，重新输入行号命令为替换当前行号命令;");
        ui->textBrowser_3->append("调试模式：点击调试模式开始调试，点击一次执行一条命令，期间无法使用载入和清空按钮，当程序出错或结束时跳出提示框，结束调试模式。");
    }

    else if(tmplist.length()==1){   //只有行号或者无空格的命令
        if(tmplist[0].toInt()){
            int linenum=tmplist[0].toInt();
            pros->del(linenum);               //删除指定行号命令
            ui->textBrowser_2->append(tmplist[0]+" has been deleted.");
            return;
        }
        else{   //无空格命令
            ui->textBrowser_2->append("wrong command!");
            return;
        }
    }


    if((tmplist[1] == "INPUT")||(tmplist[1]=="INPUTS")){
        inputcondition = 1;
        if(tmplist[1]=="INPUTS")inputcondition=2;
        ui->textBrowser_2->append(cmd);
        ui->textBrowser_2->append("?");
        if (tmplist.length()<=2){
            ui->textBrowser_2->append("no variable");  //input后没有变量，报错后继续写变量
            inputcondition = 0;
            return;
        }
        inputvar=tmplist[2];
        return;
    }

    if(tmplist[0]!=nullptr)   //插入新指令
    {
         tmplist[0].toInt(&ok);
         if(ok)
            {
               int tmplinenum = tmplist[0].toInt();
               pros->insert(tmplinenum, cmd);
            }
         else{ ui->textBrowser_2->append("wrong command!"); return;}
    }

    ui->textBrowser->append(cmd);

}

void MainWindow::on_pushButton_3_clicked() //清空按钮
{
    ui->textBrowser_4->clear();
    ui->textBrowser_3->clear();
    ui->textBrowser_2->clear();
    ui->textBrowser->clear();
    pros->clear();    //清空命令
    delete pros;
}


void MainWindow::on_pushButton_clicked() //加载文本
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("搜索命令文本"),"F:",tr("文本文件(*txt)"));
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                QMessageBox::warning(this,"error","open file error!");
                return;
            }
            else
            {
                if(!file.isReadable())
                    QMessageBox::warning(this,"error","this file is not readable!");
                else
                {
                    QTextStream textStream(&file);


                    while(!textStream.atEnd())
                     {
                        QString cmd=textStream.readLine();
                        QList<QString> tmplist = cmd.split(' ',QString::SkipEmptyParts);

                        bool ok;
                        if(tmplist[0]!=nullptr)
                        {
                            tmplist[0].toInt(&ok);
                            if(ok)
                            {
                                int tmplinenum = tmplist[0].toInt();
                                pros->insert(tmplinenum, cmd);
                            }
                        }
                        ui->textBrowser->append(cmd);
                     }
                    file.close();
                }
            }
}


void MainWindow::on_pushButton_2_clicked() //运行按钮
{
    ui->textBrowser_4->clear();
    ui->textBrowser_3->clear();
    ui->textBrowser_2->clear();
    ui->pushButton->setEnabled(1);
    ui->pushButton_3->setEnabled(1);
    Run(Vars, pros->head->next);
}

void MainWindow::highLightErrorCode(){      //错误行高亮
    if(!wrongLine.empty())
       {
           QTextEdit *code = ui->textBrowser;
           QTextCursor cursor(code->textCursor());
           QList<QTextEdit::ExtraSelection> extras;
           QVector<QPair<int, QColor>> highlights;
           for(auto iter=wrongLine.begin();iter!=wrongLine.end();iter++)
               highlights.push_back(QPair<int,QColor>(*iter,QColor(255,100,100)));
           for(auto &line:highlights)
           {
               QTextEdit::ExtraSelection h;
               h.cursor=cursor;
               int pos=ui->textBrowser->document()->findBlockByNumber(line.first-1).position();
               h.cursor.setPosition(pos);
               h.cursor.movePosition(QTextCursor::StartOfLine);
               h.cursor.movePosition(QTextCursor::EndOfLine,QTextCursor::KeepAnchor);
               h.format.setBackground(line.second);
               extras.append(h);

           }
           code->setExtraSelections(extras);
       }
}

void MainWindow::SyntaxtreeDisplay(statement *cmd){
       QList<QString> tmpstatements = cmd->text.split(' ',QString::SkipEmptyParts);
       if(tmpstatements.length()<=2){
           QString t = "";
           ui->textBrowser_3->append(tmpstatements[0]+" "+tmpstatements[1]);
           if(tmpstatements[1]!="REM"&&tmpstatements[1]!="END"){
               t += "The statement has no expression!";
               ui->textBrowser_3->append(t);
           }
           return;
       }
       if(tmpstatements[1] == "REM"){
           ui->textBrowser_3->append(tmpstatements[0] +" REM");
           QString exp = "";
           for(int i = 2;i < tmpstatements.length();++i){
               exp += tmpstatements[i];
           }
           ui->textBrowser_3->append("   "+exp);
       }

       else if(tmpstatements[1] == "GOTO"){
           ui->textBrowser_3->append(tmpstatements[0] +" GOTO");
           ui->textBrowser_3->append("        "+tmpstatements[2]);
       }

       else if(tmpstatements[1] == "LET"){
           ui->textBrowser_3->append(tmpstatements[0] +" LET =");
           QString exp = "";
           for(int i = 2;i < tmpstatements.length();++i){  //获得"LET"后的expression
               exp += tmpstatements[i];
           }

           ui->textBrowser_3->append("   "+exp);
       }

       else if(tmpstatements[1] == "PRINT"){
           ui->textBrowser_3->append(tmpstatements[0] +" PRINT");
           QString exp = "";
           for(int i = 2;i < tmpstatements.length();++i){  //获得"LET"后的expression
               exp += tmpstatements[i];
           }

           ui->textBrowser_3->append("   "+exp);
       }
       else if(tmpstatements[1] == "PRINTF"){
           ui->textBrowser_3->append(tmpstatements[0] +" PRINTF");
           QString exp = "";
           string strexp = cmd->text.toStdString();
           string tmp;
           tmp = strexp.substr(tmpstatements[0].length()+5,strexp.length()-1);
           exp=QString::fromStdString(tmp);

           ui->textBrowser_3->append("   "+exp);
       }

       else if(tmpstatements[1] == "IF"){
           ui->textBrowser_3->append(tmpstatements[0] +" IF THEN");
           QString exp = "";
           for(int i = 2;i < tmpstatements.length();++i)
           {
               if(tmpstatements[i] == "THEN") //获取条件表达式
                   break;
               else
                   exp += tmpstatements[i];
           }
           int j = 0;
           for(j = 0;j<tmpstatements.length();++j)  //获取行号
           {
               if(tmpstatements[j] == "THEN")
                   break;
           }
           ui->textBrowser_3->append("   "+exp);
           ui->textBrowser_3->append("   "+tmpstatements[j+1]);

       }
}



void MainWindow::Run(evalstate &Vars, statement *cur){
    while(cur!=nullptr){

        SyntaxtreeDisplay(cur); //打印语法树

        QList<QString> tmpstatements = cur->text.split(' ',QString::SkipEmptyParts);

        if(tmpstatements[1] == "END")
        {
            break;
        }

        else if(tmpstatements[1] == "REM"|| tmpstatements[1] == "INPUT")
        {
            cur = cur->next;
            continue;
        }

        else if(tmpstatements.length()<=2){   //无表达式情况
            QString t = "";
            t += QString::number(cur->linenum);
            t += ":";
            t += "The statement has no expression!";
            ui->textBrowser_2->append(t);
            wrongLine.push_back(cur->textline);
            cur = cur->next;
            continue;
        }

        else if(tmpstatements[1] == "LET")
        {
            QString exp = "";
            string strexp = cur->text.toStdString();
            string tmp;
            tmp = strexp.substr(tmpstatements[0].length()+5,strexp.length()-1);
            exp=QString::fromStdString(tmp);   //取出LET后的表达式

            QList<QString> tokens;
            tokens.clear();
            tokens = Tokenizer->ToTokens(exp);

            Expression *val;                //化成表达树
            try{
                val=Parser->ToTree(tokens);
            }catch(QString errorTips){
                QString Tips = "";
                Tips += QString::number(cur->linenum);
                Tips += ":";
                Tips += errorTips;
                ui->textBrowser_2->append(Tips);
                wrongLine.push_back(cur->textline);
                highLightErrorCode();
                return;
            }

            bool isSTR=exp.contains("'",Qt::CaseInsensitive)||exp.contains("\"",Qt::CaseInsensitive);
            if(isSTR){
                try {
                    val->evalstr(Vars);                               //进行求值
                } catch (QString errorTips) {
                    QString Tips = "";
                    Tips += QString::number(cur->linenum);
                    Tips += ":";
                    Tips += errorTips;
                    ui->textBrowser_2->append(Tips);
                    wrongLine.push_back(cur->textline);
                }
            }
            else{
                try {
                val->eval(Vars);                               //进行求值
                } catch (QString errorTips) {
                    QString Tips = "";
                    Tips += QString::number(cur->linenum);
                    Tips += ":";
                    Tips += errorTips;
                    ui->textBrowser_2->append(Tips);
                    wrongLine.push_back(cur->textline);
                }
            }
        }

        else if(tmpstatements[1] == "PRINTF")
        {
            QString exp = "";
            string strexp = cur->text.toStdString();
            string tmp;
            tmp = strexp.substr(tmpstatements[0].length()+8,strexp.length()-1);
            exp=QString::fromStdString(tmp);        //取出PRINT后的表达式

            QList<QString> tmpexp = exp.split(',');
            
            QString print="";
            for(int j=0;j<tmpexp.length();j++){
                QList<QString>tokens;
                tokens.clear();
                tokens = Tokenizer->ToTokens(tmpexp[j]);
                if(tmpexp[j].contains("'")||tmpexp[j].contains("\""))
                {
                    QString exp = "";
                    string strexp = tmpexp[j].toStdString();
                    string tmp;
                    tmp = strexp.substr(1,strexp.length()-2);
                    exp=QString::fromStdString(tmp);
                    if(exp.contains("'")||exp.contains("\""))
                    {
                          ui->textBrowser_2->append ("There are Quotation marks in the string");
                          wrongLine.push_back(cur->textline);
                    }
                    print+=exp;
                }
                else if(Vars.isDefined(tokens[0])){

                    Expression *val;
                    val=Parser->ToTree(tokens);
                    try {
                        val->eval(Vars);                              //进行求值
                    } catch (QString errorTips) {
                        QString Tips = "";
                        Tips += QString::number(cur->linenum);
                        Tips += ":";
                        Tips += errorTips;
                        ui->textBrowser_2->append(Tips);
                        wrongLine.push_back(cur->textline);
                        highLightErrorCode();
                        return;
                    }
                    int value=val->eval(Vars);
                    QString tmp = QString::number(value,10);
                    print+=" ";
                    print+=tmp;
                }

                else{
                    QList<QString>tokens;
                    tokens.clear();
                    tokens = Tokenizer->ToTokens(tmpexp[j]);

                    Expression *val;
                    val=Parser->ToTree(tokens);
                    try {
                        val->evalstr(Vars);                              //进行求值
                    } catch (QString errorTips) {
                        QString Tips = "";
                        Tips += QString::number(cur->linenum);
                        Tips += ":";
                        Tips += errorTips;
                        ui->textBrowser_2->append(Tips);
                        wrongLine.push_back(cur->textline);
                        highLightErrorCode();
                        return;
                    }
                    QString tmp=val->evalstr(Vars);
                    print +=" ";
                    print +=tmp;
                }
            }
            ui->textBrowser_2->append(print);
        }

        else if(tmpstatements[1]=="PRINT"){
            if(Vars.isDefined(tmpstatements[2])){
                QList<QString>tokens;
                tokens.clear();
                tokens = Tokenizer->ToTokens(tmpstatements[2]);

                Expression *val;
                val=Parser->ToTree(tokens);
                try {
                    val->eval(Vars);                              //进行求值
                } catch (QString errorTips) {
                    QString Tips = "";
                    Tips += QString::number(cur->linenum);
                    Tips += ":";
                    Tips += errorTips;
                    ui->textBrowser_2->append(Tips);
                    wrongLine.push_back(cur->textline);
                    highLightErrorCode();
                    return;
                }
                int value=val->eval(Vars);
                QString tmp = QString::number(value,10);
                ui->textBrowser_2->append(tmp);
            }
        }

        else if(tmpstatements[1] == "GOTO")
        {
            bool ok;
            tmpstatements[2].toInt(&ok);
            if(ok)
            {
                int desnum = tmpstatements[2].toInt();       //获取要跳转的行号
                statement *tmp = pros->head->next;
                while (tmp!=nullptr)
                {
                    QList<QString> tmpst = tmp->text.split(' ',QString::SkipEmptyParts);
                    if(tmpst[0].toInt() == desnum) break;
                    tmp = tmp->next;
                }
                if(tmp == nullptr) //报错，没有这一行
                {
                    QString t = "";
                    t += QString::number(cur->linenum);
                    t += ":";
                    t += "The linenumber is invalid！";
                    ui->textBrowser_2->append(t);
                    wrongLine.push_back(cur->textline);
                    cur=cur->next;
                    continue;
                }
                cur = tmp;     //切换到指定行号指令
                continue;
            }
            else{
                ui->textBrowser_2->append(tmpstatements[0]+": Not a number!");
                wrongLine.push_back(cur->textline);
            }
        }

        else if(tmpstatements[1] == "IF")
        {
            QString exp = "";
            for(int i = 2;i < tmpstatements.length();++i)
            {
                if(tmpstatements[i] == "THEN") //获取条件表达式
                    break;
                else
                    exp += tmpstatements[i];
            }
            try{
                IfCondition(exp);
            }
            catch(QString err){
                QString t = "";
                t += QString::number(cur->linenum);
                t += ":";
                t += "The expression is invalid！";
                ui->textBrowser_2->append(t);
                wrongLine.push_back(cur->textline);
                highLightErrorCode();
                return;
            }

            bool ok=IfCondition(exp);

            if(ok)           //如果if条件成立，就去找then
            {
                int j = 0;
                for(j = 0;j<tmpstatements.length();++j)
                {
                    if(tmpstatements[j] == "THEN")
                        break;
                }
                if(j == tmpstatements.length()-1)
                {
                    QString t = "";
                    t += QString::number(cur->linenum);
                    t += ":";
                    t += "No linenumber is input!";
                    ui->textBrowser_2->append(t);
                    wrongLine.push_back(cur->textline);
                    cur=cur->next;
                    continue;
                }

                int desnum = tmpstatements[j+1].toInt();  //获取行号
                statement *tmp = pros->head->next;
                while (tmp!=nullptr)
                {
                    QList<QString> tmpst = tmp->text.split(' ');
                    if(tmpst[0].toInt() == desnum)
                    {
                        break;
                    }
                    tmp = tmp->next;
                }
                if(tmp == nullptr)
                {
                    QString t = "";
                    t += QString::number(cur->linenum);
                    t += ":";
                    t += "The linenumber is invalid！";
                    ui->textBrowser_2->append(t);
                    wrongLine.push_back(cur->textline);
                    cur=cur->next;
                    continue;
                }
                cur = tmp;
                continue;
            }
        }

        else {                    //非指令集中的指令，报错
            QString t = "";
            t += QString::number(cur->linenum);
            t += ":";
            t += "The statement is invalid!";
            ui->textBrowser_2->append(t);
            wrongLine.push_back(cur->textline);
        }
        cur=cur->next;
    }

    highLightErrorCode();
    ui->textBrowser_4->clear();
    ui->textBrowser_4->append(Vars.Tranvrese());
}

bool MainWindow::IfCondition(QString str)
{
    if(str.contains("="))  //=><三种情况
    {
        QList<QString> ifstrs = str.split('=',QString::SkipEmptyParts);

        QList<QString>tokensleft;
        tokensleft.clear();
        tokensleft = Tokenizer->ToTokens(ifstrs[0]);
        Expression *valleft;
        valleft = Parser->ToTree(tokensleft);   //化成表达树
        int numleft ;
        QString err;
        try {
            numleft = valleft->eval(Vars);         //左表达式求值                               //进行求值
        } catch (QString errorTips) {
            err = errorTips;
            throw err;
        }

        QList<QString>tokensright;
        tokensright.clear();
        tokensright = Tokenizer->ToTokens(ifstrs[1]);
        Expression*valright;
        valright = Parser->ToTree(tokensright);
        int numright ;
        QString err2;
        try {
            numright = valright->eval(Vars);        //右表达式求值                                //进行求值
        } catch (QString errorTips) {
            err = errorTips;
            throw err;
        }

        if(numleft == numright)
        {
            return true;
        }
        else {
            return false;
        }
    }
    else if (str.contains(">")) {
        QList<QString> ifstrs = str.split('>',QString::SkipEmptyParts);
        QList<QString>tokensleft;
        tokensleft.clear();
        tokensleft = Tokenizer->ToTokens(ifstrs[0]);

        Expression*valleft;
        valleft = Parser->ToTree(tokensleft);
        int numleft ;
        QString err;
        try {
            numleft = valleft->eval(Vars);                                        //进行求值
        } catch (QString errorTips) {
            err = errorTips;
            throw err;
        }

        QList<QString>tokensright;
        tokensright.clear();
        tokensright = Tokenizer->ToTokens(ifstrs[1]);

        Expression*valright;
        valright = Parser->ToTree(tokensright);
        int numright ;
        QString err2;
        try {
            numright = valright->eval(Vars);                                        //进行求值
        } catch (QString errorTips) {
            err = errorTips;
            throw err;
        }

        if(numleft > numright)
        {
            return true;
        }
        else {
            return false;
        }
    }
    else if (str.contains("<")) {
        QList<QString> ifstrs = str.split('<',QString::SkipEmptyParts);
        QList<QString>tokensleft;
        tokensleft.clear();
        tokensleft = Tokenizer->ToTokens(ifstrs[0]);

        Expression*valleft;
        valleft = Parser->ToTree(tokensleft);
        int numleft ;
        QString err;
        try {
            numleft = valleft->eval(Vars);        //进行求值
        } catch (QString errorTips) {
            err = errorTips;
            throw err;
        }

        QList<QString>tokensright;
        tokensright.clear();
        tokensright = Tokenizer->ToTokens(ifstrs[1]);
        Expression*valright;
        valright = Parser->ToTree(tokensright);
        int numright ;
        QString err2;
        try {
            numright = valright->eval(Vars);                                        //进行求值
        } catch (QString errorTips) {
            err = errorTips;
            throw err;
        }

        if(numleft < numright)
        {
            return true;
        }
        else {
            return false;
        }
    }
    return false;
}


void MainWindow::on_pushButton_4_clicked()  //单步调试按钮
{
    ui->pushButton->setEnabled(0);
    ui->pushButton_3->setEnabled(0);


    if(debugging==0){
        curstatement=pros->head->next;
        debugging = 1;
    }
    SyntaxtreeDisplay(curstatement); //打印语法树

    QList<QString> tmpstatements = curstatement->text.split(' ',QString::SkipEmptyParts);

    if(tmpstatements[1] == "END") {     //程序结束时弹出提示信息
        debugging = 0;
        ui->textBrowser_4->append("程序运行结束");
        ui->pushButton->setEnabled(1);
        ui->pushButton_3->setEnabled(1);
        QMessageBox::information(NULL,"END","程序运行结束");
        return;
    }

    else if(tmpstatements[1] == "REM"|| tmpstatements[1] == "INPUT")
    {
        curstatement = curstatement->next;
        return;
    }

    else if(tmpstatements.length()<=2){
        QString t = "";
        t += QString::number(curstatement->linenum);
        t += ":";
        t += "The statement has no expression!";
        ui->textBrowser_2->append(t);
        curstatement=curstatement->next;
        QMessageBox::information(NULL,"ERROR","程序错误");
        ui->pushButton->setEnabled(1);
        ui->pushButton_3->setEnabled(1);
        return;
    }

    else if(tmpstatements[1] == "LET")
    {
        QString exp = "";
        string strexp = curstatement->text.toStdString();
        string tmp;
        tmp = strexp.substr(tmpstatements[0].length()+5,strexp.length()-1);
        exp=QString::fromStdString(tmp);   //取出LET后的表达式

        QList<QString> tokens;
        tokens.clear();
        tokens = Tokenizer->ToTokens(exp);

        Expression *val;                //化成表达树
        try{
            val=Parser->ToTree(tokens);
        }catch(QString errorTips){
            QString Tips = "";
            Tips += QString::number(curstatement->linenum);
            Tips += ":";
            Tips += errorTips;
            ui->textBrowser_2->append(Tips);
            QMessageBox::information(NULL,"ERROR","程序错误");
            ui->pushButton->setEnabled(1);
            ui->pushButton_3->setEnabled(1);
            curstatement=curstatement->next;
            return;
        }

        bool isSTR=exp.contains("'",Qt::CaseInsensitive)||exp.contains("\"",Qt::CaseInsensitive);
        if(isSTR){
            try {
                val->evalstr(Vars);                               //进行求值
            } catch (QString errorTips) {
                QString Tips = "";
                Tips += QString::number(curstatement->linenum);
                Tips += ":";
                Tips += errorTips;
                ui->textBrowser_2->append(Tips);
                QMessageBox::information(NULL,"ERROR","程序错误");
                ui->pushButton->setEnabled(1);
                ui->pushButton_3->setEnabled(1);
            }
        }
        if(!isSTR){
            try {
            val->eval(Vars);                               //进行求值
            } catch (QString errorTips) {
                QString Tips = "";
                Tips += QString::number(curstatement->linenum);
                Tips += ":";
                Tips += errorTips;
                ui->textBrowser_2->append(Tips);
                QMessageBox::information(NULL,"ERROR","程序错误");
                ui->pushButton->setEnabled(1);
                ui->pushButton_3->setEnabled(1);
            }
        }
    }

    else if(tmpstatements[1] == "PRINTF")
    {
        QString exp = "";
        string strexp = curstatement->text.toStdString();
        string tmp;
        tmp = strexp.substr(tmpstatements[0].length()+8,strexp.length()-1);
        exp=QString::fromStdString(tmp);        //取出PRINT后的表达式

        QList<QString> tmpexp = exp.split(',');  //用，逐个分开

        QString print="";
        for(int j=0;j<tmpexp.length();j++){
            QList<QString>tokens;
            tokens.clear();
            tokens = Tokenizer->ToTokens(tmpexp[j]);
            if(tmpexp[j].contains("'")||tmpexp[j].contains("\""))
            {
                QString exp = "";
                string strexp = tmpexp[j].toStdString();
                string tmp;
                tmp = strexp.substr(1,strexp.length()-2);
                exp=QString::fromStdString(tmp);
                if(exp.contains("'")||exp.contains("\""))
                {
                      ui->textBrowser_2->append ("There are Quotation marks in the string");
                      wrongLine.push_back(curstatement->textline);
                      QMessageBox::information(NULL,"ERROR","程序错误");
                      ui->pushButton->setEnabled(1);
                      ui->pushButton_3->setEnabled(1);
                }
                print+=exp;
            }

            else if(Vars.isDefined(tokens[0])){

                Expression *val;
                val=Parser->ToTree(tokens);
                try {
                    val->eval(Vars);                              //进行求值
                } catch (QString errorTips) {
                    QString Tips = "";
                    Tips += QString::number(curstatement->linenum);
                    Tips += ":";
                    Tips += errorTips;
                    ui->textBrowser_2->append(Tips);
                    QMessageBox::information(NULL,"ERROR","程序错误");
                    ui->pushButton->setEnabled(1);
                    ui->pushButton_3->setEnabled(1);
                    curstatement=curstatement->next;
                    return;
                }
                int value=val->eval(Vars);
                QString tmp = QString::number(value,10);
                print +=" ";
                print+=tmp;
            }
            else{
                QList<QString>tokens;
                tokens.clear();
                tokens = Tokenizer->ToTokens(tmpexp[j]);

                Expression *val;
                val=Parser->ToTree(tokens);
                try {
                    val->evalstr(Vars);                              //进行求值
                } catch (QString errorTips) {
                    QString Tips = "";
                    Tips += QString::number(curstatement->linenum);
                    Tips += ":";
                    Tips += errorTips;
                    ui->textBrowser_2->append(Tips);
                    QMessageBox::information(NULL,"ERROR","程序错误");
                    ui->pushButton->setEnabled(1);
                    ui->pushButton_3->setEnabled(1);
                    curstatement=curstatement->next;
                    return;
                }
                QString tmp=val->evalstr(Vars);
                print +=" ";
                print +=tmp;
            }
        }
        ui->textBrowser_2->append(print);

    }
    else if(tmpstatements[1]=="PRINT"){
        if(Vars.isDefined(tmpstatements[2])){
            QList<QString>tokens;
            tokens.clear();
            tokens = Tokenizer->ToTokens(tmpstatements[2]);

            Expression *val;
            val=Parser->ToTree(tokens);
            try {
                val->eval(Vars);                              //进行求值
            } catch (QString errorTips) {
                QString Tips = "";
                Tips += QString::number(curstatement->linenum);
                Tips += ":";
                Tips += errorTips;
                ui->textBrowser_2->append(Tips);
                QMessageBox::information(NULL,"ERROR","程序错误");
                ui->pushButton->setEnabled(1);
                ui->pushButton_3->setEnabled(1);
                curstatement=curstatement->next;
                return;
            }
            int value=val->eval(Vars);
            QString tmp = QString::number(value,10);
            ui->textBrowser_2->append(tmp);
        }
    }
    else if(tmpstatements[1] == "GOTO")
    {
        bool ok;
        tmpstatements[2].toInt(&ok);
        if(ok)
        {
            int desnum = tmpstatements[2].toInt();       //获取要跳转的行号
            statement *tmp = pros->head->next;
            while (tmp!=nullptr)
            {
                QList<QString> tmpst = tmp->text.split(' ',QString::SkipEmptyParts);
                if(tmpst[0].toInt() == desnum) break;
                tmp = tmp->next;
            }
            if(tmp == nullptr) //报错，没有这一行
            {
                QString t = "";
                t += QString::number(curstatement->linenum);
                t += ":";
                t += "The linenumber is invalid！";
                ui->textBrowser_2->append(t);
                QMessageBox::information(NULL,"ERROR","程序错误");
                ui->pushButton->setEnabled(1);
                ui->pushButton_3->setEnabled(1);
            }
            QTextEdit *code = ui->textBrowser;
            QTextCursor cursor(code->textCursor());
            QList<QTextEdit::ExtraSelection> extras;
            QVector<QPair<int, QColor>> highlights;
            QTextEdit::ExtraSelection h;
            h.cursor=cursor;
            int pos=ui->textBrowser->document()->findBlockByNumber(curstatement->textline-1).position();
            h.cursor.setPosition(pos);
            h.cursor.movePosition(QTextCursor::StartOfLine);
            h.cursor.movePosition(QTextCursor::EndOfLine,QTextCursor::KeepAnchor);
            h.format.setBackground(QColor(100,255,100));
            extras.append(h);
            code->setExtraSelections(extras);
            curstatement = tmp;
            return;
        }
        else{
            ui->textBrowser_2->append("Not a number!");
            QMessageBox::information(NULL,"ERROR","程序错误");
            ui->pushButton->setEnabled(1);
            ui->pushButton_3->setEnabled(1);
        }
    }

    else if(tmpstatements[1] == "IF")
    {
        QString exp = "";
        for(int i = 2;i < tmpstatements.length();++i)
        {
            if(tmpstatements[i] == "THEN") //获取条件表达式
                break;
            else
                exp += tmpstatements[i];
        }

        try{
            IfCondition(exp);
        }
        catch(QString err){
            QString t = "";
            t += QString::number(curstatement->linenum);
            t += ":";
            t += "The expression is invalid！";
            ui->textBrowser_2->append(t);
            curstatement=curstatement->next;
            QMessageBox::information(NULL,"ERROR","程序错误");
            ui->pushButton->setEnabled(1);
            ui->pushButton_3->setEnabled(1);
            return;
        }

        bool ok=IfCondition(exp);

        if(ok)           //如果if条件成立，就去找then
        {
            int j = 0;
            for(j = 0;j<tmpstatements.length();++j)
            {
                if(tmpstatements[j] == "THEN")
                    break;
            }
            if(j == tmpstatements.length()-1)
            {
                QString t = "";
                t += QString::number(curstatement->linenum);
                t += ":";
                t += "No linenumber is input!";
                ui->textBrowser_2->append(t);
                curstatement=curstatement->next;
                QMessageBox::information(NULL,"ERROR","程序错误");
                ui->pushButton->setEnabled(1);
                ui->pushButton_3->setEnabled(1);
                return;
            }

            int desnum = tmpstatements[j+1].toInt();  //获取行号
            statement *tmp = pros->head->next;
            while (tmp!=nullptr)
            {
                QList<QString> tmpst = tmp->text.split(' ',QString::SkipEmptyParts);
                if(tmpst[0].toInt() == desnum)
                {
                    break;
                }
                tmp = tmp->next;
            }
            if(tmp == nullptr)
            {
                QString t = "";
                t += QString::number(curstatement->linenum);
                t += ":";
                t += "The linenumber is invalid！";
                ui->textBrowser_2->append(t);
                QMessageBox::information(NULL,"ERROR","程序错误");
                ui->pushButton->setEnabled(1);
                ui->pushButton_3->setEnabled(1);
            }          
            QTextEdit *code = ui->textBrowser;
            QTextCursor cursor(code->textCursor());
            QList<QTextEdit::ExtraSelection> extras;
            QVector<QPair<int, QColor>> highlights;
            QTextEdit::ExtraSelection h;
            h.cursor=cursor;
            int pos=ui->textBrowser->document()->findBlockByNumber(curstatement->textline-1).position();
            h.cursor.setPosition(pos);
            h.cursor.movePosition(QTextCursor::StartOfLine);
            h.cursor.movePosition(QTextCursor::EndOfLine,QTextCursor::KeepAnchor);
            h.format.setBackground(QColor(100,255,100));
            extras.append(h);
            code->setExtraSelections(extras);
            curstatement = tmp;
            return;
        }
    }
    else {
        QString t = "";
        t += QString::number(curstatement->linenum);
        t += ":";
        t += "The statement is invalid!";
        ui->textBrowser_2->append(t);
        QMessageBox::information(NULL,"ERROR","程序错误");
        ui->pushButton->setEnabled(1);
        ui->pushButton_3->setEnabled(1);
    }

    ui->textBrowser_4->clear();
    ui->textBrowser_4->append(Vars.Tranvrese());   //显示当前变量

    QTextEdit *code = ui->textBrowser;
    QTextCursor cursor(code->textCursor());
    QList<QTextEdit::ExtraSelection> extras;
    QVector<QPair<int, QColor>> highlights;
    QTextEdit::ExtraSelection h;
    h.cursor=cursor;
    int pos=ui->textBrowser->document()->findBlockByNumber(curstatement->textline-1).position();
    h.cursor.setPosition(pos);
    h.cursor.movePosition(QTextCursor::StartOfLine);
    h.cursor.movePosition(QTextCursor::EndOfLine,QTextCursor::KeepAnchor);
    h.format.setBackground(QColor(100,255,100));
    extras.append(h);
    code->setExtraSelections(extras);   //当前执行行高亮

    curstatement=curstatement->next;
}
