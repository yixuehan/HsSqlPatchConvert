#include "sqlpatch.h"
#include <QFileInfo>

shared_ptr<SqlPatchInterface> getSqlPatch(const string &strFileName)
{
   // �����׺��table_design
   auto posDot = strFileName.find_last_of('.') ;
   if ( string::npos != strFileName.find("table_design", posDot) ) {
      return shared_ptr<SqlPatchInterface>(new SqlPatch<SqlTable>(strFileName)) ;
   }
   // �����sysconfig.xml
   QFileInfo fileInfo ;
   fileInfo.setFile(toQStr(strFileName));
   if ( fileInfo.baseName() == "sysconfig" ) {
      return shared_ptr<SqlPatchInterface>(new SqlPatch<SqlConfig>(strFileName)) ;
   }
   return nullptr ;
}
