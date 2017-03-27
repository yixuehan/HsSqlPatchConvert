#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QApplication>
#include <QClipboard>

MainWindow::MainWindow(QWidget *parent) :
   QMainWindow(parent),
   ui(new Ui::MainWindow)
{
   ui->setupUi(this);
   uiField.setupUi(&dialogField);
   
   QObject::connect(uiField.listAll, SIGNAL(doubleClicked(QModelIndex)),
                    this, SLOT(on_listAll_doubleClicked(const QModelIndex&))) ;

   QObject::connect(uiField.listNew, SIGNAL(on_listNew_doubleClicked(QModelIndex)),
                    this, SLOT(on_listNew_doubleClicked(const QModelIndex &))) ;
//   SqlPatch<SqlTable> t ;
//   t.setSelectedFields(vector<string>());
}

MainWindow::~MainWindow()
{
   delete ui;
}

void MainWindow::on_addField_clicked()
{
    QFileDialog fileDialog ;
    fileDialog.setDirectory(toQStr(getSetting()->get<string>("basePath")));
    fileDialog.setFileMode(QFileDialog::ExistingFiles);
    if ( fileDialog.exec() ) {
       for ( auto & fileName : fileDialog.selectedFiles() ) {
          // ���֮ǰûѡ�����ļ�
          auto baseName = boost::filesystem::basename(toStr(fileName)) ;
          auto qBaseName = toQStr(baseName) ;
          if ( ui->listMod->findItems(qBaseName, Qt::MatchExactly).count() == 0 ) {
             ui->listMod->addItem(qBaseName);
             selectedFiles[baseName] = getSqlPatch(toStr(fileName)) ;
             qDebug() << qBaseName ;
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
   string fileName = toStr(ui->listMod->item(index.row())->text()) ;
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
         fields.push_back(toStr(uiField.listNew->item(i)->text())) ;
      }
      qDebug() << "fileName:" << toQStr(fileName) ;
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
   QMessageBox msg ;
   if ( ui->modifyNo->text().length() == 0 ) {
      
      msg.setText(toQStr("�޸ĵ���Ų���Ϊ��"));
      msg.exec(); 
      return ;
   }
   getSetting()->put<string>("modifyNo", toStr(ui->modifyNo->text())) ;
   for ( auto pair : selectedFiles ) {
      pair.second->genSql() ;
   }
   SqlTable::writeSql();
   SqlConfig::writeSql();
   SqlError::writeSql() ;
   SqlBusiness::writeSql() ;
   msg.setText(toQStr("ת�����"));
   msg.exec();
   for ( const auto &copyValue : *getCopy() ) {
      ui->listCopy->addItem(toQStr(copyValue.second.name));
   }
}

void MainWindow::on_listNew_doubleClicked(const QModelIndex &index)
{
   auto pItem = uiField.listNew->takeItem(index.row());
   uiField.listNew->removeItemWidget(pItem);
   delete pItem ;
}

void MainWindow::on_listCopy_doubleClicked(const QModelIndex &index)
{
   string copyName = toStr(ui->listCopy->item(index.row())->text()) ;
   string strCopy = (*getCopy())[copyName].strCopy ;
   // ������а�
   QClipboard *board = QApplication::clipboard();  
   board->setText(toQStr(strCopy));
}

void MainWindow::on_genSource_clicked()
{
    
}
