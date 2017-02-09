#include "sqlpatch.h"
#include <QFileInfo>

// 表结构的特化
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

template<>
void SqlPatch<SqlTable>::writeSql()
{
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
      genNote( "sqlFil" ) ;
      writeSql( "sqlFil" ) ;
   }

   if (  mapSqlInfo["sqlData"].bNeed ) {
      genNote( "sqlData" ) ;
      writeSql( "sqlData" ) ;
   }
}
