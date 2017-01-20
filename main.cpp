#include "mainwindow.h"
#include <QApplication>
#include <boost/format.hpp>
#include <QMessageBox>
int main(int argc, char *argv[])
{
   QApplication a(argc, argv);
   QMessageBox msgBox ;
   try {
      MainWindow w;
      w.show();
      return a.exec();
   }
   catch ( boost::property_tree::info_parser_error &ec ) {

      boost::format str( "文件%1%在%2%行解析出错.%3%") ;
      str % ec.filename() % ec.line() % ec.message() ;
      msgBox.setText(toQStr(str.str())) ;
      msgBox.exec() ;
   }
   catch ( boost::property_tree::xml_parser_error &ec ) {

      boost::format str( "文件%1%在%2%行解析出错.%3%") ;
      str % ec.filename() % ec.line() % ec.message() ;
      msgBox.setText(toQStr(str.str())) ;
      msgBox.exec() ;
   }
   catch ( boost::property_tree::ptree_bad_path &ec ) {
      boost::format str( "%1%") ;
      str % ec.what() ;
      msgBox.setText(toQStr(str.str())) ;
      msgBox.exec() ;
   }
   catch ( boost::property_tree::ptree_bad_data &ec ) {
      boost::format str( "%1%") ;
      str % ec.what();
      msgBox.setText(toQStr(str.str())) ;
      msgBox.exec() ;
   }
   catch ( boost::bad_any_cast ec)
   {
      boost::format str("%1%") ;
      str % ec.what() ;
      msgBox.setText(toQStr(str.str())) ;
      msgBox.exec() ;
   }
   catch(std::exception &e)
   {
      boost::format str("%1%") ;
      str % e.what();
      msgBox.setText(toQStr(str.str())) ;
      msgBox.exec() ;
   }
   catch(...)
   {
      boost::format str("Unknown exception") ;
      msgBox.setText(toQStr(str.str())) ;
      msgBox.exec() ;
   }
   
   
   
}
