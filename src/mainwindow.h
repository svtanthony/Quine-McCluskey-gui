#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QListWidget;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void addItem();
    void setBits();
    void removeItems();
    void solve();
    void createTables();
    void createfill();
    void stuffTable(int bits);
    void stuffTable(int bits, int index, QString &num, int value);


private:
    int mapTerm(const QString & term);
    int mapTerm(const QString & term, int index);
    void exclusiveAdd(QString val, QListWidget* target, QListWidget* other);
    void invert(char tTable[],int tTlength);
    void createSolving(int tTlength,int bitNum,char tTable[],QSet<QString> &sTable, QSet<QString> &primes,QList<QString> &primary);
    void simplify(QSet<QString> sTable, QSet<QString> &primes, int tTwidth, bool *match = 0);
    void minimize(QList<QString> primes, QList<QString> primary, int bitNum);
    void decode(QList<QString> &solutions, int bitNum);
    void nonEssential(QList<QList<int> > &mTable, bool match[], QList<int> list,QString temp,QList<QString> &solutions,QList<QString> &primes,int position);
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
