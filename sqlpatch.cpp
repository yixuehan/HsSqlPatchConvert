#include "sqlpatch.h"
#include <boost/filesystem.hpp>

// 获取相应文件的处理对象
shared_ptr<SqlPatchInterface> getSqlPatch(const string &strFileName)
{
   // 如果后缀是table_design
   auto posDot = strFileName.find_last_of('.') ;
   if ( string::npos != strFileName.find("table_design", posDot) ) {
      return shared_ptr<SqlPatchInterface>(new SqlPatch<SqlTable>(strFileName)) ;
   }
   // 如果是sysconfig.xml
   auto baseName = boost::filesystem::basename(strFileName) ;// p( strFileName ) ;
   //qDebug() << toQStr(baseName) ;
   if ( baseName == "sysconfig" ) {
      return shared_ptr<SqlPatchInterface>(new SqlPatch<SqlConfig>(strFileName)) ;
   }
   return nullptr ;
}
