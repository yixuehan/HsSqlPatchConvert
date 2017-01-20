#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent) :
   QMainWindow(parent),
   ui(new Ui::MainWindow)
{
   ui->setupUi(this);
   uiField.setupUi(&dialogField);
   
   QObject::connect(uiField.listAll, SIGNAL(doubleClicked(QModelIndex)),
                    this, SLOT(on_listAll_doubleClicked(const QModelIndex&))) ;
   // test
   QObject::connect(uiField.listNew, SIGNAL(on_listNew_doubleClicked(QModelIndex)),
                    this, SLOT(on_listNew_doubleClicked(const QModelIndex &))) ;
}

MainWindow::~MainWindow()
{
   delete ui;
}

void MainWindow::on_addField_clicked()
{
    QFileDialog fileDialog ;
    QFileInfo fileInfo ;
    fileDialog.setDirectory(toQStr(getSetting()->get<string>("basePath")));
    fileDialog.setFileMode(QFileDialog::ExistingFiles);
    if ( fileDialog.exec() ) {
       for ( auto & fileName : fileDialog.selectedFiles() ) {
          // ���֮ǰûѡ�����ļ�
          fileInfo.setFile(fileName);
          if ( ui->listMod->findItems(fileInfo.baseName(), Qt::MatchExactly).count() == 0 ) {
             ui->listMod->addItem(fileInfo.baseName());
             selectedFiles[toCStr(fileInfo.baseName())] = getSqlPatch(toCStr(fileName)) ;
          }
       }
    }
}

void MainWindow::on_listMod_doubleClicked(const QModelIndex &index)
{
   // ���ԭ����
   uiField.listAll->clear();
   uiField.listNew->clear();
   
   // ������ӵ������ֶ����ӵ�������
   string fileName = toCStr(ui->listMod->item(index.row())->text()) ;
   for ( auto field : selectedFiles[fileName]->getSelectedFields() ) {
      uiField.listNew->addItem(toQStr(field));
   }
   for ( auto field : selectedFiles[fileName]->getTableFields() ) {
      uiField.listAll->addItem(toQStr(field));
   }
   // �������е������ֶθ���ԭ�е��ֶ�
   if ( dialogField.exec() ) {
      vector<string> fields ;
      for ( int i = 0 ; i < uiField.listNew->count(); ++i ) {
         fields.push_back(toCStr(uiField.listNew->item(i)->text())) ;
      }
      selectedFiles[fileName]->setSelectedFields(std::move(fields)) ;
   }
}

void MainWindow::on_listAll_doubleClicked(const QModelIndex &index)
{
   auto qText = uiField.listAll->item(index.row())->text();
   if ( uiField.listNew->findItems(qText, Qt::MatchExactly).count() == 0 ) {
      uiField.listNew->addItem(qText); 
   }
}

void MainWindow::on_genSqlPatch_clicked()
{
   for ( auto pair : selectedFiles ) {
      pair.second->genSql() ;
   }
   SqlPatch<SqlTable>::writeSql();
}

void MainWindow::on_listNew_doubleClicked(const QModelIndex &index)
{
   auto pItem = uiField.listNew->takeItem(index.row());
   uiField.listNew->removeItemWidget(pItem);
   delete pItem ;
}
