#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QProcess>
#include <QFileDialog>
#include <QDebug>
#include <QTextStream>
#include <QMessageBox>
#include<cmath>
#include<cstring>
#include<string>
#include <iostream>
#include<fstream>
#include <codecvt>
#include <locale>
using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_selectButton_clicked();

    void on_runButton_clicked();

    void appendToTextEdit(const QString &text);

    void getTokenMain();

    void GetToken();

private:
    Ui::Widget *ui;
    QStringList fileContent;  // 存储文件内容
    int pos = 0;
};
#endif // WIDGET_H
