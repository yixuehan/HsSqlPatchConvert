#include "sqlpatch.h"
#include <boost/filesystem.hpp>

// ��ȡ��Ӧ�ļ��Ĵ������
shared_ptr<SqlPatchInterface> getSqlPatch(const string &strFileName)
{
   // �����׺��table_design
   auto posDot = strFileName.find_last_of('.') ;
   if ( string::npos != strFileName.find("table_design", posDot) ) {
      return shared_ptr<SqlPatchInterface>(new SqlPatch<SqlTable>(strFileName)) ;
   }
   // �����sysconfig.xml
   auto baseName = boost::filesystem::basename(strFileName) ;// p( strFileName ) ;
   //qDebug() << toQStr(baseName) ;
   if ( baseName == "sysconfig" ) {
      return shared_ptr<SqlPatchInterface>(new SqlPatch<SqlConfig>(strFileName)) ;
   }
   return nullptr ;
}
