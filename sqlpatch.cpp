#include "sqlpatch.h"
#include <QFileInfo>

template <class T>
std::map<string, SqlInfo> SqlPatch<T>::mapSqlInfo ;


template <class T>
bool SqlPatch<T>::bInit = false ;

// 获取字段名获取对应的oracle类型和默认值
void getTypeValue( const string &fieldName, string &strType, string &defaultValue )
{
   static bool bInit = false ;
   static map<string, string> mapfield2type ;
   if ( !bInit ) {
      map<string, string> mapStdfields ;
      map<string, string> mapDataType ;
      boost::property_tree::ptree pt ;
      readPtreeFromUtf8(getSetting()->get<string>("stdfieldsFile"), pt);
      for ( const auto &child : pt.get_child("hsdoc")) {
         if ( "stdfield" == child.first )
            mapStdfields[child.second.get<string>("<xmlattr>.name")] =
               child.second.get<string>("<xmlattr>.type") ;
      }
      readPtreeFromUtf8(getSetting()->get<string>("dataTypeFile"), pt);
      for ( const auto &child : pt.get_child("hsdoc")) {
         if ( "userType" == child.first )
            mapDataType[child.second.get<string>("<xmlattr>.name")] =
                  child.second.get<string>("database.map.<xmlattr>.value") ;
      }
      for ( const auto &valuePair : mapStdfields ) {
         mapfield2type[valuePair.first] = mapDataType[valuePair.second] ;
      }
   }
   strType = mapfield2type[fieldName] ;
   defaultValue = (string::npos == strType.find("number") ? "'' ''" : "0.0") ;
}

shared_ptr<SqlPatchInterface> getSqlPatch(const string &strFileName)
{
   // 如果后缀是table_design
   auto posDot = strFileName.find_last_of('.') ;
   if ( string::npos != strFileName.find("table_design", posDot) ) {
      return shared_ptr<SqlPatchInterface>(new SqlPatch<SqlTable>(strFileName)) ;
   }
   // 如果是sysconfig.xml
   QFileInfo fileInfo ;
   fileInfo.setFile(toQStr(strFileName));
//   QMessageBox msg ;
//   msg.setText(fileInfo.baseName());
//   msg.exec() ;
   if ( fileInfo.baseName() == "sysconfig" ) {
      return shared_ptr<SqlPatchInterface>(new SqlPatch<SqlConfig>(strFileName)) ;
   }
   return nullptr ;
}

template<>
const vector<string> SqlPatch<SqlTable>::getTableFields() const
{
   if ( fields.empty() ) {
      for ( auto &node : pt.get_child("hsdoc") ) {
         if ( "tablefield" != node.first ) continue ;
         fields.push_back(node.second.get<string>("<xmlattr>.name"));
      }
   }
   return fields ;
}

template<>
void SqlPatch<SqlTable>::setSelectedFields( const vector<string> &fields )
{
   mapTableInfo["sqlHis"].bNeed = pt.get<bool>("hsdoc.tablePrimaryInfo.<xmlattr>.history") ;
   mapTableInfo["sqlData"].bNeed = pt.get<bool>("hsdoc.tablePrimaryInfo.<xmlattr>.rtable") ;
   selectedFields = fields ;
   
   if ( mapTableInfo["sqlHis"].bNeed ) {    
      mapTableInfo["sqlLast"].bNeed = true ;
      mapTableInfo["sqlFil"].bNeed = true ; 
   }

   if ( !mapSqlInfo["sqlHis"].bNeed ) mapSqlInfo["sqlHis"].bNeed = mapTableInfo["sqlHis"].bNeed ;
   if ( !mapSqlInfo["sqlData"].bNeed ) mapSqlInfo["sqlData"].bNeed = mapTableInfo["sqlData"].bNeed ;
   if ( !mapSqlInfo["sqlLast"].bNeed ) mapSqlInfo["sqlLast"].bNeed = mapTableInfo["sqlLast"].bNeed ;
   if ( !mapSqlInfo["sqlFil"].bNeed ) mapSqlInfo["sqlFil"].bNeed = mapTableInfo["sqlFil"].bNeed ;
}

template<>
void SqlPatch<SqlTable>::specialInit()
{
   if ( !bInit ) {
      mapSqlInfo["sqlHis"].sqlFileName = getSetting()->get<string>("sqlHis.fileName") ;
      mapSqlInfo["sqlFil"].sqlFileName = getSetting()->get<string>("sqlFil.fileName") ;
      mapSqlInfo["sqlData"].sqlFileName = getSetting()->get<string>("sqlData.fileName") ;
      mapSqlInfo["sqlCom"].sqlFileName = getSetting()->get<string>("sqlCom.fileName") ;
      ifstream ifs(mapSqlInfo["sqlHis"].sqlFileName, ios::binary ) ;
      std::copy(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>(), 
                std::back_inserter(mapSqlInfo["sqlHis"].sqlOldText)) ;
      ifs.close();
      
      ifs.open(mapSqlInfo["sqlFil"].sqlFileName, ios::binary) ;
      std::copy(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>(), 
                std::back_inserter(mapSqlInfo["sqlFil"].sqlOldText)) ;
      ifs.close();
      
      ifs.open(mapSqlInfo["sqlData"].sqlFileName, ios::binary) ;
      std::copy(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>(), 
                std::back_inserter(mapSqlInfo["sqlData"].sqlOldText)) ;
      ifs.close() ;
      
      ifs.open(mapSqlInfo["sqlCom"].sqlFileName, ios::binary) ;
      std::copy(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>(), 
                std::back_inserter(mapSqlInfo["sqlCom"].sqlOldText)) ;
      ifs.close() ;
   }
   tableName = pt.get<string>("hsdoc.tablePrimaryInfo.<xmlattr>.tableName") ;
}


template<>
void SqlPatch<SqlTable>::genAdd( const string &strBase )
{
   // 获取sql模板
   boost::format fmtSql(getSetting()->get<string>(strBase + ".sqlAdd")) ;
   // 表名 字段名 类型  默认值
   string note ;
   string strTableName ;
   strTableName = getSetting()->get<string>(strBase + ".tablePrefix") + tableName ;

   for ( const auto &stdfield : /*mapSqlInfo[strBase].*/selectedFields ) {
      fmtSql.clear() ;
      string strType ;
      string strDefaultValue ;
      getTypeValue( stdfield, strType, strDefaultValue );
      fmtSql % strTableName % stdfield % strType % strDefaultValue ;

      mapSqlInfo[strBase].sqlText += fmtSql.str() + "\r\n" ;
      if ( note.empty() ) {
         note = "表[" + strTableName + "]增加字段" + stdfield ;
      }
      else  {
         note += "," + stdfield ;
      }
   }
//   QMessageBox msg ;
//   msg.setText(toQStr(strTableName + ":" + note)) ;
//   msg.exec() ;
   mapSqlInfo[strBase].changeNote += note + " " ;
}



template<>
void SqlPatch<SqlTable>::genSql()
{
   if ( mapTableInfo["sqlHis"].bNeed )
      genSql( "sqlHis" ) ;
   
   if ( mapTableInfo["sqlFil"].bNeed )
      genSql( "sqlFil" ) ;
   
   if ( mapTableInfo["sqlData"].bNeed )
      genSql( "sqlData" ) ;
   
   if ( mapTableInfo["sqlLast"].bNeed )
      genSql( "sqlLast" ) ;
   
   genSql( "sqlCom" ) ;
}

//template<>
//void SqlPatch<SqlTable>::genSql(const string &strBase)
//{
//   genAdd( strBase ) ;
//}

template<>
void SqlPatch<SqlTable>::writeSql()
{
   // QMessageBox msg;
   //将com和last的生成合并
   if ( mapSqlInfo["sqlCom"].bNeed ) {
      mapSqlInfo["sqlCom"].changeNote +=  mapSqlInfo["sqlLast"].changeNote ;
      genNote( "sqlCom") ;
      mapSqlInfo["sqlCom"].sqlText +=  mapSqlInfo["sqlLast"].sqlText ;
      writeSql( "sqlCom" ) ;
   }
   
   if ( mapSqlInfo["sqlHis"].bNeed ) {
      genNote( "sqlHis") ;
      writeSql( "sqlHis" ) ;      
   }
   
   if (  mapSqlInfo["sqlFil"].bNeed ) {
//      msg.setText(toQStr(SqlTable::sqlOldFilText)) ;
//      msg.exec();
      genNote( "sqlFil" ) ;
      writeSql( "sqlFil" ) ;
   }

   if (  mapSqlInfo["sqlData"].bNeed ) {
      genNote( "sqlData" ) ;
      writeSql( "sqlData" ) ;
   }
}

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


