#include "sqlbusiness.h"

bool SqlBusiness::bInit = false ;
#include <boost/algorithm/string.hpp>

SqlBusiness::SqlBusiness(const string &strFile)
   :SqlPatchInterface(strFile)
{
   init( ) ;
}


const vector<string> SqlBusiness::getTableFields() const
{
   if ( fields.empty() ) {
      auto iter = pt.get_child("hsdoc").end();
      while ( pt.get_child("hsdoc").begin() != iter ) {        
         --iter ;
         if ( iter->first == "busflag" ) {
            string id = iter->second.get<string>("<xmlattr>.bussniessFlag") ;
            if ( id >= getSetting()->get<string>("sqlBusiness.idBegin") 
                 && id <= getSetting()->get<string>("sqlBusiness.idEnd") ) {
               mapTableInfo[id].id = id ;
               mapTableInfo[id].name = iter->second.get<string>("<xmlattr>.bussniessName") ;
               mapTableInfo[id].business_kind = "0" ;
               mapTableInfo[id].business_subject = iter->second.get<string>("<xmlattr>.bussniessSubject") ;
               mapTableInfo[id].business_group = 
                     boost::algorithm::erase_all_copy( iter->second.get<string>("<xmlattr>.bussniessSort"), "-")
                     +iter->second.get<string>("<xmlattr>.reverse");
               mapTableInfo[id].borrow_lend = iter->second.get<string>("<xmlattr>.loan") ;
               mapTableInfo[id].join_business_flag = iter->second.get<string>("<xmlattr>.reverseBussniess") ;
               mapTableInfo[id].opp_business_flag = iter->second.get<string>("<xmlattr>.reverseBussniessFlag") ;
   
               fields.push_back( id );
            }
         } 
      }
   }
   return fields ;
}


void SqlBusiness::genSql()
{
   SqlPatchInterface::genSql( "sqlBusiness" ) ;
}


void SqlBusiness::genAdd( const string &strBase )
{
   // 获取sql模板
   boost::format fmtSql(getSetting()->get<string>(strBase + ".sqlAdd")) ;
   // id    配置名称  配置类型  数据类型  char   int str  remark  manager access
   boost::format fmtNote(getSetting()->get<string>(strBase + ".noteFmt")) ;
   for ( const auto &stdfield : selectedFields ) {
      fmtSql.clear() ;
      fmtNote.clear() ;
      auto & info = mapTableInfo[stdfield] ;
      //string id; 
      //string name; 
      //string business_kind; 
      //string business_subject; 
      //string business_group; 
      //string borrow_lend; 
      //string join_business_flag; 
      //string opp_business_flag ;
      fmtSql % info.id % info.name % info.business_kind 
             % ("" == info.business_subject ? " " : info.business_subject)
             % (("" == info.business_group || info.business_group == " ") ? "0" : info.business_group)
             % (("" == info.borrow_lend || info.borrow_lend == " ") ? "0" : info.borrow_lend)
             % info.join_business_flag % info.opp_business_flag ;
      mapSqlInfo[strBase].sqlText += fmtSql.str() + "\r\n" ;
      
      if ( "" == mapSqlInfo[strBase].changeNote)
         mapSqlInfo[strBase].changeNote += fmtNote.str() + info.id + " " ;
      else
         mapSqlInfo[strBase].changeNote += info.id + " " ;
   }
}


void SqlBusiness::init()
{
   if ( !bInit ) {
      mapSqlInfo["sqlBusiness"].sqlFileName = getSetting()->get<string>("sqlBusiness.fileName") ;

      ifstream ifs(mapSqlInfo["sqlBusiness"].sqlFileName, ios::binary ) ;
      std::copy(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>(), 
                std::back_inserter(mapSqlInfo["sqlBusiness"].sqlOldText)) ;
      ifs.close();
      bInit = true ;
   }
}


void SqlBusiness::writeSql()
{
   if ( !mapSqlInfo["sqlBusiness"].sqlText.empty() ) {
      genNote( "sqlBusiness") ;
      SqlPatchInterface::writeSql( "sqlBusiness" ) ;      
   }
}
