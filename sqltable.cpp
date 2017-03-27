#include "sqltable.h"

bool SqlTable::bInit = false ;

SqlTable::SqlTable(const string &strFile)
   :SqlPatchInterface(strFile)
{
   init() ;
}

const vector<string> SqlTable::getTableFields() const
{
   if ( fields.empty() ) {
      for ( auto &node : pt.get_child("hsdoc") ) {
         if ( "tablefield" != node.first ) continue ;
         fields.push_back(node.second.get<string>("<xmlattr>.name"));
      }
   }
   return fields ;
}


void SqlTable::setSelectedFields( const vector<string> &fields )
{
   QMessageBox msg ;
   msg.setText("SqlTable::setSelectedFields") ;
   msg.exec( ) ;
   mapTableInfo["sqlHis"].bNeed = pt.get<bool>("hsdoc.tablePrimaryInfo.<xmlattr>.history") ;
   mapTableInfo["sqlData"].bNeed = pt.get<bool>("hsdoc.tablePrimaryInfo.<xmlattr>.rtable") ;
   selectedFields = fields ;
   
   if ( mapTableInfo["sqlHis"].bNeed ) {    
      mapTableInfo["sqlLast"].bNeed = true ;
      mapTableInfo["sqlFil"].bNeed = true ;
   }
   
   mapSqlInfo["sqlCom"].bNeed = mapTableInfo["sqlCom"].bNeed = true ;
   if ( !mapSqlInfo["sqlHis"].bNeed ) mapSqlInfo["sqlHis"].bNeed = mapTableInfo["sqlHis"].bNeed ;
   if ( !mapSqlInfo["sqlData"].bNeed ) mapSqlInfo["sqlData"].bNeed = mapTableInfo["sqlData"].bNeed ;
   if ( !mapSqlInfo["sqlLast"].bNeed ) mapSqlInfo["sqlLast"].bNeed = mapTableInfo["sqlLast"].bNeed ;
   if ( !mapSqlInfo["sqlFil"].bNeed ) mapSqlInfo["sqlFil"].bNeed = mapTableInfo["sqlFil"].bNeed ;
}


void SqlTable::init()
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
      bInit = true ;
   }
   tableName = pt.get<string>("hsdoc.tablePrimaryInfo.<xmlattr>.tableName") ;
}



void SqlTable::genAdd( const string &strBase )
{
   // 获取sql模板
   boost::format fmtSql(getSetting()->get<string>(strBase + ".sqlAdd")) ;
   // 表名 字段名 类型  默认值
   string note ;
   string strTableName ;
   strTableName = getSetting()->get<string>(strBase + ".tablePrefix") + tableName ;

   for ( const auto &stdfield : selectedFields ) {
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




void SqlTable::genSql()
{
   if ( mapTableInfo["sqlHis"].bNeed )
      SqlPatchInterface::genSql( "sqlHis" ) ;
   
   if ( mapTableInfo["sqlFil"].bNeed )
      SqlPatchInterface::genSql( "sqlFil" ) ;
   
   if ( mapTableInfo["sqlData"].bNeed )
      SqlPatchInterface::genSql( "sqlData" ) ;
   
   if ( mapTableInfo["sqlLast"].bNeed )
      SqlPatchInterface::genSql( "sqlLast" ) ;
   
   SqlPatchInterface::genSql( "sqlCom" ) ;
}


void SqlTable::writeSql()
{
   //将com和last的生成合并
   QMessageBox msg ;
   msg.setText("sqlTable::writeSql") ;
   msg.exec();
   if ( mapSqlInfo["sqlCom"].bNeed ) {
      mapSqlInfo["sqlCom"].changeNote +=  mapSqlInfo["sqlLast"].changeNote ;
      genNote( "sqlCom") ;
      mapSqlInfo["sqlCom"].sqlText +=  mapSqlInfo["sqlLast"].sqlText ;
      SqlPatchInterface::writeSql( "sqlCom" ) ;
      msg.setText("sqlCom") ;
      msg.exec() ;
   }
   
   if ( mapSqlInfo["sqlHis"].bNeed ) {
      genNote( "sqlHis") ;
      SqlPatchInterface::writeSql( "sqlHis" ) ;   
      msg.setText("sqlHis") ;
      msg.exec() ;
   }
   
   if (  mapSqlInfo["sqlFil"].bNeed ) {
      genNote( "sqlFil" ) ;
      SqlPatchInterface::writeSql( "sqlFil" ) ;
      msg.setText("sqlFil") ;
      msg.exec() ;
   }

   if (  mapSqlInfo["sqlData"].bNeed ) {
      genNote( "sqlData" ) ;
      SqlPatchInterface::writeSql( "sqlData" ) ;
      msg.setText("sqlData") ;
      msg.exec() ;
   }
}
