#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <map>
#include <pubfunc.h>
#include "ui_fieldchioce.h"
#include "sqlpatch.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
   Q_OBJECT
   
public:
   explicit MainWindow(QWidget *parent = 0);
   ~MainWindow();
   
private slots:
   void on_addField_clicked();
   void on_listMod_doubleClicked(const QModelIndex &index);
   
   void on_listAll_doubleClicked(const QModelIndex &index);
   
   void on_genSqlPatch_clicked();
   
   void on_listNew_doubleClicked(const QModelIndex &index);
   
private:

private:
   Ui::MainWindow *ui;
   QDialog dialogField ;
   Ui::fieldChoice uiField ;
   // 记录选中的文件
   map<std::string, shared_ptr<SqlPatchInterface>> selectedFiles ;
   
};

#endif // MAINWINDOW_H
