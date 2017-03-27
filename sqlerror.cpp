#include "sqlerror.h"

bool SqlError::bInit = false ;

SqlError::SqlError(const string &strFile)
   :SqlPatchInterface(strFile)
{
   init() ;
}

const vector<string> SqlError::getTableFields() const
{
 //  string strPrev ;
   if ( fields.empty() ) {
      string id ;
      for ( auto &node : pt.get_child("hsdoc") ) {
         if ( node.first != "type" ) continue ;
         qDebug() << toQStr(node.second.get<string>("<xmlattr>.name") ) ;
         if ( node.second.get<string>("<xmlattr>.name") 
              == getSetting()->get<string>("sqlError.itemName")) {
             // 显示最后N条
            auto iter = node.second.end() ;
            if ( node.second.begin() != iter ) {
               for ( int i = 1; i <= getSetting()->get<int>("sqlError.listCount"); ++i ) {
                  auto iterSecond = (--iter)->second ;
                  id = iterSecond.get<string>("<xmlattr>.code") ;
                  mapTableInfo[id].id = iterSecond.get<string>("<xmlattr>.code") ;
                  mapTableInfo[id].name = iterSecond.get<string>("<xmlattr>.info") ;
                  mapTableInfo[id].constant = iterSecond.get<string>("<xmlattr>.constant") ;
                  fields.push_back(id) ;
                  if ( node.second.begin() == iter ) break ;
               }
            }
            break ;
         }
      }
   }
   return fields ;
}


void SqlError::genSql()
{
   SqlPatchInterface::genSql( "sqlError" ) ;
}


void SqlError::genAdd( const string &strBase )
{
   // 获取sql模板
   boost::format fmtSql(getSetting()->get<string>(strBase + ".sqlAdd")) ;
   // id    errorName
   mapSqlInfo[strBase].changeNote = getSetting()->get<string>(strBase + ".noteFmt") ;
   for ( const auto &stdfield : selectedFields ) {
      fmtSql.clear() ;
      //fmtNote.clear() ;
      auto & info = mapTableInfo[stdfield] ;
      fmtSql % info.id % info.name ;
      mapSqlInfo[strBase].sqlText += fmtSql.str() + "\r\n" ;
      mapSqlInfo[strBase].changeNote += info.id + " " ;
   }
}


void SqlError::init()
{
   if ( !bInit ) {
      mapSqlInfo["sqlError"].sqlFileName = getSetting()->get<string>("sqlError.fileName") ;

      ifstream ifs(mapSqlInfo["sqlError"].sqlFileName, ios::binary ) ;
      std::copy(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>(), 
                std::back_inserter(mapSqlInfo["sqlError"].sqlOldText)) ;
      ifs.close();
      bInit = true ;
   }
}


void SqlError::writeSql()
{
   
   if ( !mapSqlInfo["sqlError"].sqlText.empty() ) {
      genNote( "sqlError") ;
      SqlPatchInterface::writeSql( "sqlError" ) ;      
   }
}

//
//void SqlError::setSelectedFields( const vector<string> &fields )
//{
//   QMessageBox msg ;
//   msg.setText("SqlError::setSelectedFields") ;
//   msg.exec( ) ;
//   selectedFields = fields ;
//}
