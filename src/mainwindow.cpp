#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QRegExp>
#include <math.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("R Q M");

    setBits();
    createTables();

    connect(ui->termBox,SIGNAL(editingFinished()),this,SLOT(addItem()));
    connect(ui->dcBox,SIGNAL(editingFinished()),this,SLOT(addItem()));
    connect(ui->bitsBox,SIGNAL(editingFinished()),this,SLOT(setBits()));
    connect(ui->bitsBox2,SIGNAL(valueChanged(int)),this,SLOT(createTables()));
    connect(ui->functionBox,SIGNAL(valueChanged(int)),this,SLOT(createTables()));

    //buttons
    connect(ui->termButton,SIGNAL(clicked()),this,SLOT(removeItems()));
    connect(ui->dcButton,SIGNAL(clicked()),this,SLOT(removeItems()));
    connect(ui->solveButton,SIGNAL(clicked()),this,SLOT(solve()));
    connect(ui->solveButton2,SIGNAL(clicked()),this,SLOT(solve()));
    connect(ui->fillButton,SIGNAL(clicked()),this,SLOT(createfill()));
}
MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::setBits()
{
    static int bits = 0;
    if(!bits)
        bits = ui->bitsBox->value();

    if(bits > ui->bitsBox->value())
    {
        ui->termsList->clear();
        ui->dcList->clear();
    }

    bits = ui->bitsBox->value();

    int maxValue = (1 << bits) - 1;
    ui->termBox->setMaximum(maxValue);
    ui->dcBox->setMaximum(maxValue);

}
void MainWindow::addItem()
{
    QObject* sndr = sender();
    if(sndr == ui->termBox)
    {
        QString itemValue = ui->termBox->text();

        exclusiveAdd(itemValue,ui->termsList,ui->dcList);
    }
    else if(sndr == ui->dcBox)
    {
        QString itemValue = ui->dcBox->text();

        exclusiveAdd(itemValue,ui->dcList,ui->termsList);
    }

}
void MainWindow::exclusiveAdd(QString val, QListWidget* target, QListWidget* other)
{
    QList<QListWidgetItem*> found = target->findItems(val,Qt::MatchExactly);

    if(found.isEmpty())
        target->addItem(val);

    found = other->findItems(val,Qt::MatchExactly);

    if(!found.isEmpty())
        delete found[0];
}
void MainWindow::removeItems()
{
    if(sender() == ui->termButton)
        foreach(QListWidgetItem* item, ui->termsList->selectedItems())
            delete item;
    else if(sender() == ui->dcButton)
        foreach(QListWidgetItem* item, ui->dcList->selectedItems())
            delete item;
}
void MainWindow::solve()
{
    ui->solutions->clear();

    QObject * sndr = sender();
    int tTlength, bitNum,functions;
    if(sndr == ui->solveButton)
        tTlength = (1<<(ui->bitsBox->value())), bitNum = (ui->bitsBox->value()),functions = 1;
    else
        tTlength = (1<<(ui->bitsBox2->value())), bitNum = (ui->bitsBox2->value()),functions = ui->functionBox->value();

    char tTable[tTlength];
    QList<QString> primary;
    QSet<QString> sTable, primes;

    if(sndr == ui->solveButton)
    {
        for(int i(0);i<(tTlength);i++)
            tTable[i] = '0';

        for(int i(0);i<ui->termsList->count();i++)
            tTable[ui->termsList->item(i)->text().toInt()]='1';

        for(int i(0);i<ui->dcList->count();i++)
            tTable[ui->dcList->item(i)->text().toInt()]='x';

        if(ui->maxButton->isChecked() && ui->sopButton->isChecked())
            invert(tTable,tTlength);
        else if(ui->minButton->isChecked() && ui->posButton->isChecked())
            invert(tTable,tTlength);

        createSolving(tTlength,bitNum,tTable,sTable,primes,primary);
    }
    else
    {
        for(int i(1);i<=functions;++i)
        {
            primary.clear();
            sTable.clear();
            primes.clear();
            for(int j(0);j<tTlength;++j)
            {
                if(!ui->entryTable->item(j,i)->text().isEmpty())
                {
                    QString s = ui->entryTable->item(j,i)->text();
                    tTable[j] = s[0].toAscii();
                }
                else
                    tTable[j] = 'x';
            }

            if(ui->complimentBox->isChecked() != ui->posButton2->isChecked())
                invert(tTable,tTlength);

            ui->solutions->addItem(QString("Function %1").arg(i-1));
            createSolving(tTlength,bitNum,tTable,sTable,primes,primary);
            ui->solutions->addItem(" ");
        }
    }


}
void MainWindow::createSolving(int tTlength,int bitNum,char tTable[],QSet<QString> &sTable, QSet<QString> &primes,QList<QString> &primary)
{
    for(int i(0);i < tTlength;i++)
    {
        if(tTable[i] != '0')
            {
                QString convert,binary = QString::number(i,2);
                int count = binary.count();
                convert.fill('0',bitNum);
                convert.replace(bitNum-count,count,binary);
                sTable << convert;
                if(tTable[i] == '1')
                    primary.append(convert);
            }
    }
    simplify(sTable,primes,bitNum);
    minimize(primes.toList(),primary,bitNum);
}
void MainWindow::createTables()
{
    int rows = 1<<ui->bitsBox2->value(),var = ui->bitsBox2->value(),col(ui->functionBox->value()+1);
    ui->entryTable->setRowCount(rows);
    ui->entryTable->setColumnCount(col);

    QList<QString> rowname,columnname;
    columnname.append("Variables");

    for(int i(0);i<rows;++i)
        rowname.append(QString::number(i,10));

    for(int i(0);i<col;++i)
    {
        QString s;
        s.append("F").append(QString::number(i,10));
        columnname.append(s);
    }
    for(int i(0);i<rows;++i)
        for(int j(0);j<col;++j)
            ui->entryTable->setItem(i,j,new QTableWidgetItem());

    ui->entryTable->setHorizontalHeaderLabels(columnname);
    ui->entryTable->setVerticalHeaderLabels(rowname);
    ui->entryTable->resizeColumnsToContents();
    ui->entryTable->resizeRowsToContents();

    stuffTable(var);
}
void MainWindow::stuffTable(int bits)
{
    QString s;
    s.fill('-',bits);
    stuffTable(bits,0,s,0);
}
void MainWindow::stuffTable(int bits, int index, QString &num, int value)
{
    if(index == bits)
    {
        QTableWidgetItem *item = new QTableWidgetItem(num);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        ui->entryTable->setItem(value,0,item);
        return;
    }

    num[index] = '0';
    stuffTable(bits,index+1,num,value << 1);
    num[index] = '1';
    stuffTable(bits,index+1,num,1 + (value << 1));
}
void MainWindow::createfill()
{
    int col = ui->entryTable->currentColumn(), row = 1<<ui->bitsBox2->value();
    if(col < 1)
        return;

    for(int i(0);i<row;++i)
        if(ui->entryTable->item(i,col)->text().isEmpty())
        {
            QTableWidgetItem *item = new QTableWidgetItem(ui->defaultBox->currentText());
            ui->entryTable->setItem(i,col,item);
        }
}
void MainWindow::invert(char tTable[],int tTlength)
{
    for(int k(0);k<tTlength;k++)
        {
            if(tTable[k] == '0')
                tTable[k] = '1';
            else if(tTable[k] == '1')
                tTable[k] = '0';
        }
}
void MainWindow::simplify(QSet<QString> sTable, QSet<QString> &primes, int bitNum, bool * match)
{
    int msize = pow(3,bitNum);
    if( match == 0 )
    {
        match = new bool[msize];
        memset(match,0,msize);
    }
    //bool match[sTable.count()];
//    memset(match,0,sTable.count()*sizeof(bool));
    QSet<QString> temp;
    QList<QString> sTableL = sTable.values();
    for(int i(0), _i(sTable.count()); i < _i; ++i)
    {
        for(int j(0);j<(bitNum);++j)
        {
            if(sTableL[i][j] != '0')
                continue;
/*//            for(int k(0);k<(sTable.count());++k)
//            {
//                if(sTable[i] == sTable[k])
//                {
//                    QString implicant = sTable[i];
//                    implicant[j] = '-';
//                    temp.append(implicant);
//                    match[i] = true;
//                    match[k] = true;
//                   break;
//                }
//            }*/
            sTableL[i][j] = '1';
            bool matched = false;
            if(sTable.contains(sTableL[i]))
            {
                match[mapTerm(sTableL[i])] = true;
                matched = true;
                //QString implicant = sTableL[i];
                //implicant[j] = '-';
                sTableL[i][j] = '-';
//                temp << implicant;
                temp << sTableL[i];
            }
            sTableL[i][j]='0';
            if(matched)
                match[mapTerm(sTableL[i])] = true;
        }

//        if(!match[i] && !primes.contains(sTable[i]))
//            primes.append(sTable[i]);
    }
    for(int i(0), _i(sTable.count()); i < _i; ++i)
        if(!match[mapTerm(sTableL[i])])
            primes << sTableL[i];
    if(!temp.isEmpty())
        simplify(temp,primes,bitNum);
}
int MainWindow::mapTerm(const QString & term)
{
    return mapTerm(term, term.size() - 1);
}
int MainWindow::mapTerm(const QString & term, int index)
{
    if(index == -1)
        return 0;

    int value = 0;
    switch(term[index].toAscii())
    {
    case '0':
        value = 0;
        break;
    case '1':
        value = 1;
        break;
    default:
        value = 2;
    }

    return value + 3 * mapTerm(term, index - 1);
}
void MainWindow::minimize(QList<QString> primes, QList<QString> primary, int bitNum)
{
    QList<QList<int> > mTable;
    QVector<int> defV;
    defV.fill(0,primary.size());
    QList<int> list = defV.toList();
    QList<QString> solutions;
    QString temp;
    int pCol = primary.count(),pRow = primes.count();
    bool match[primes.count()];
    memset(match,1,primes.count()*sizeof(bool));

    while(mTable.size() != primes.size())
        mTable.append(list);

    for(int i(0);i<pRow;++i)
    {
        temp.clear();
        for(int j(0);j<bitNum;++j)
        {
            if(primes[i][j] == '1')
            {
                temp.append('1');
                continue;
            }
            if(primes[i][j] == '0')
            {
                temp.append('0');
                continue;
            }
            if(primes[i][j] == '-')
                temp.append("[01]");
        }
        for(int k(0);k<pCol;++k)
            if(primary[k].contains(QRegExp(temp)))
            {
                mTable[i][k]= 1;
                list[k] += 1;
            }
    }
    temp.clear();
    for(int i(0);i<pCol;++i)//pick essential primes
        if(list[i] == 1)
            for(int j(0);j<pRow;++j)
                if(mTable[j][i] == 1 && (!temp.contains(primes[j])))
                {
                    temp.append(primes[j]);
                    match[j]=0;
                }
    for(int k(0);k<pCol;++k)
        list[k] = 0;
    nonEssential(mTable,match,list,temp,solutions,primes,0);
    decode(solutions,bitNum);
}
void MainWindow::decode(QList<QString> &solutions,int bitNum)
{
    int sSize = solutions.count(),initial(solutions[0].length());
    QObject * sndr = sender();

    if(sSize>1)
        for(int i(0);i<sSize;++i)
            if(solutions[i].length()<initial)
                initial = solutions[i].length();

    foreach(QString s, solutions)
        if(s.length() > initial)
            solutions.removeOne(s);

    sSize = solutions.count();
    QString decode;

    if((sndr == ui->solveButton && ui->sopButton->isChecked()) || (sndr == ui->solveButton2 && ui->sopButton2->isChecked()))
    {
        for(int i(0);i<sSize;++i)
        {
            decode.clear();

            bool first = 1;
            for(int j(0);j<initial;++j)
            {
                int variable = j%bitNum;
                if(!first && variable==0)
                    decode.append("+");
                first = 0;

                if(solutions[i][j] == '-')
                    continue;
                decode.append(char(variable+65));
                if(solutions[i][j] == '0')
                    decode.append("'");
            }
            solutions[i] = decode;
        }
    }
    else
    {
        for(int i(0);i<sSize;++i)
        {
            bool first = true;
            decode.clear();
            for(int j(0);j<initial;++j)
            {
                int variable = j%bitNum;
                if(!variable)
                {
                    decode.append("(");
                    first = true;
                }
                if(solutions[i][j] == '-')
                {
                    if(variable == bitNum-1)
                        decode.append(")");
                    continue;
                }
                if(!first)
                    decode.append("+");
                first = false;

                decode.append(char(variable+65));
                if(solutions[i][j] == '1')
                    decode.append("'");
                if(variable == bitNum-1)
                    decode.append(")");
            }
            solutions[i] = decode;
        }
    }
    ui->solutions->addItems(solutions);
}
void MainWindow::nonEssential(QList<QList<int> > &mTable, bool match[], QList<int> list,QString temp,QList<QString> &solutions,QList<QString> &primes,int position)
{
    int col = list.size(),row = mTable.size();
    bool summation = 1;

    for(int i(0);i<row;++i)
        if(!match[i])
            for(int j(0);j<col;++j)
                list[j]+= mTable[i][j];
    for(int i(0);i<col;++i)
        if(list[i]==0)
        {
            summation = 0;
            break;
        }

    // add short circuit criteria for number of terms
    //use (primes.size)*bitSize for initial
    //pass by reference and use solution[i].count%bitSize = num of false....

    if(summation)//have solution clean tableat insert
    {
        //use minimum to clear or use for each to clean up
        solutions.append(temp);
        return;
    }

    if(position == row)//currently at end
        return;

    if(!match[position])//continue to next row
    {
        nonEssential(mTable,match,list,temp,solutions,primes,position+1);
        return;
    }

    //use & don't use row then continue to next row
    nonEssential(mTable,match,list,temp,solutions,primes,position+1);
    match[position]=0;
    temp.append(primes[position]);
    nonEssential(mTable,match,list,temp,solutions,primes,position+1);
    match[position]=1;
}
