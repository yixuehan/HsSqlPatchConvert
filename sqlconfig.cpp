#include "sqlpatch.h"
#include <QFileInfo>

//  系统配置的特化
template<>
const vector<string> SqlPatch<SqlConfig>::getTableFields() const
{
   
 //  string strPrev ;
   if ( fields.empty() ) {
      for ( auto &node : pt.get_child("sysconfig.items") ) {
         // if ( "tablefield" != node.first ) continue ;
   //      switch ( node.second.get<int>("dataType")) {
   //      case 0 :
   //         strPrev = "int_config_" ;
   //         break ;
   //      case 1 :
   //         strPrev = "char_config_" ;
   //         break ;
   //      case 2 :
   //         strPrev = "str_config_" ;
   //         break ;      
   //      }
         // sqlConfig ;
         string id = node.second.get<string>("id") ;
         if ( id >= "46000" && id < "47000" ) {
            mapTableInfo[id].id = id ;
            mapTableInfo[id].name = node.second.get<string>("name") ;
            mapTableInfo[id].configType = node.second.get<string>("configType") ;
            mapTableInfo[id].dataType = node.second.get<string>("dataType") ;
            mapTableInfo[id].intValue = node.second.get<string>("intValue") ;
            mapTableInfo[id].charValue = node.second.get<string>("charValue") ;
            mapTableInfo[id].strValue = node.second.get<string>("stringValue") ;
            mapTableInfo[id].remark = node.second.get<string>("desc") ;
            mapTableInfo[id].manager_level = node.second.get<string>("mgrLevel") ;
            mapTableInfo[id].access_level = node.second.get<string>("storageLevel") ;
            fields.push_back( id );
         }
      }
   }
   return fields ;
}

template<>
void SqlPatch<SqlConfig>::genSql()
{
   genSql( "sqlConfig" ) ;
}

template<>
void SqlPatch<SqlConfig>::genAdd( const string &strBase )
{
   // 获取sql模板
   boost::format fmtSql(getSetting()->get<string>(strBase + ".sqlAdd")) ;
   // id    配置名称  配置类型  数据类型  char   int str  remark  manager access
   boost::format fmtNote(getSetting()->get<string>(strBase + ".noteFmt")) ;
   for ( const auto &stdfield : /*mapSqlInfo[strBase].*/selectedFields ) {
      fmtSql.clear() ;
      fmtNote.clear() ;
      auto & info = mapTableInfo[stdfield] ;
      fmtSql % info.id % info.name % info.configType % info.dataType 
             % (("" == info.charValue || info.charValue == " ") ? "0" : info.charValue)
             % (("" == info.intValue || info.intValue == " ") ? "0" : info.intValue)
             % (("" == info.strValue) ? " " : info.strValue)
             % info.remark % info.manager_level % info.access_level ;
      mapSqlInfo[strBase].sqlText += fmtSql.str() + "\r\n" ;
      
      fmtNote % info.id % info.name ;
      mapSqlInfo[strBase].changeNote += fmtNote.str() + " " ;
   }
//   QMessageBox msg ;
//   msg.setText(toQStr(strTableName + ":" + note)) ;
//   msg.exec() ;
}

template<>
void SqlPatch<SqlConfig>::specialInit()
{
   if ( !bInit ) {
      mapSqlInfo["sqlConfig"].sqlFileName = getSetting()->get<string>("sqlConfig.fileName") ;

      ifstream ifs(mapSqlInfo["sqlConfig"].sqlFileName, ios::binary ) ;
      std::copy(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>(), 
                std::back_inserter(mapSqlInfo["sqlConfig"].sqlOldText)) ;
      ifs.close();
   }
}

template<>
void SqlPatch<SqlConfig>::writeSql()
{
   
   if ( !mapSqlInfo["sqlConfig"].sqlText.empty() ) {
      genNote( "sqlConfig") ;
      writeSql( "sqlConfig" ) ;      
   }
}
