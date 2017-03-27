#include "sqlpatch.h"
#include <boost/filesystem.hpp>
#include "sqlbusiness.h"
#include "sqlconfig.h"
#include "sqlerror.h"
#include "sqltable.h"

std::map<string, SqlInfo> SqlPatchInterface::mapSqlInfo ;


SqlPatchInterface::SqlPatchInterface( const string& strFileName )
   :fileName(strFileName)
{
   readPtreeFromUtf8(fileName, pt);
   //init() ;
}

void SqlPatchInterface::genSql( const string &strBase )
{
   genAdd( strBase ) ;
}

void SqlPatchInterface::genNote( const string &strBase )
{
   boost::format fmtNote(getSetting()->get<string>(strBase + ".note")) ;
   boost::format fmtCopy(getSetting()->get<string>("copyFmt")) ;
   // 修改单号 版本 
   auto & mapCopy = *getCopy() ;
   // --V8.15.1.%1%   %2%-%3%-%4%      %5%   %6%   %7% 	 %8%" 
   // 版本号   年 月  日  修改单  修改人 申请人  修改说明
   string notePosStr = getSetting()->get<string>(strBase + ".notePosStr") ;
   auto &notePos = mapSqlInfo[strBase].notePos ;
   const auto &strOldSql = mapSqlInfo[strBase].sqlOldText ;
   notePos = strOldSql.find( notePosStr ) ;
   qDebug() << toQStr(notePosStr) << ":" << (size_t)notePos ;
//      QMessageBox msg ;
//      msg.setText(toQStr(strOldSql)) ;
//      msg.exec() ;
//      msg.setText(toQStr(notePosStr))  ;
//      msg.exec() ;
   // 记录修改记录的文件位置
   notePos += notePosStr.length() ;
   // 计算版本号
   auto preLength = getSetting()->get<int>(strBase + ".preLength") ;
   auto spacePos = strOldSql.find_first_of(' ', notePos + preLength ) ;
   // 取版本
   auto dotPos = strOldSql.find_last_of('.', spacePos ) ;
   string strVer = strOldSql.substr(dotPos+1, spacePos - dotPos-1) ;
   //qDebug() << toQStr("strBase:" + strBase + "|||strVer:" + strVer + "|||fmtCopy" + fmtCopy.str()) ;
   fmtNote % (std::stoi(strVer)+1) ;
   
   boost::gregorian::date d(boost::gregorian::day_clock::local_day()) ;
   
   fmtNote % (int)d.year() % (int)d.month() % (int)d.day() 
           % getSetting()->get<string>("modifyNo")
           % getSetting()->get<string>("userName")
           % getSetting()->get<string>("applyUserName")
           % mapSqlInfo[strBase].changeNote ;
   
   spacePos = fmtNote.str().find_first_of(' ', preLength ) ;
   fmtCopy % getSetting()->get<string>("modifyNo") % fmtNote.str().substr( preLength, spacePos- preLength ) ;
   
   mapSqlInfo[strBase].note = fmtNote.str() ;
   mapSqlInfo[strBase].beginNote = boost::algorithm::replace_first_copy(mapSqlInfo[strBase].note, "--", "\r\n-- begin ") ;
   mapSqlInfo[strBase].endNote = boost::algorithm::replace_first_copy(mapSqlInfo[strBase].note, "--", "-- end ") ;
   
   // 记录剪切内容
   struct CopyInfo copyInfo ;
   copyInfo.name = boost::filesystem::basename( mapSqlInfo[strBase].sqlFileName ) ;
   copyInfo.strCopy = fmtCopy.str() ;
   mapCopy[copyInfo.name] = copyInfo ;
}


void SqlPatchInterface::setSelectedFields( const vector<string> &fields )
{
//   QMessageBox msg ;
//   msg.setText("SqlPatch<T>::setSelectedFields") ;
//   msg.exec( ) ;
   selectedFields = fields ;
}

const vector<string> SqlPatchInterface::getSelectedFields() const
{
   return selectedFields ;
}

const boost::property_tree::ptree &SqlPatchInterface::getPTree() const
{
   return pt ;
}

void SqlPatchInterface::writeSql( const string &strBase )
{
   ofstream ofs(getSetting()->get<string>(strBase + ".fileName"), ios::binary) ;
   // 先写到增加修改说明的位置，写入修改说明，再写到增加sql语句的位置，写入sql语句，再写完文件
   size_t wOldByte = 0;
   auto &strOldSql = mapSqlInfo[strBase].sqlOldText ;
   auto &notePos = mapSqlInfo[strBase].notePos ;
   auto &note = mapSqlInfo[strBase].note ;
   auto &beginNote  = mapSqlInfo[strBase].beginNote ;
   auto &endNote  = mapSqlInfo[strBase].endNote ;
   auto &sqlText = mapSqlInfo[strBase].sqlText ;
   ofs.write( strOldSql.c_str(), notePos ) ;
   wOldByte = notePos ;
   ofs.write( note.c_str(), note.length() ) ;
   // 获取写入sql语句的位置
   size_t sqlPos = strOldSql.find(getSetting()->get<string>(strBase + ".sqlEnd")) - 1 ;
   ofs.write( strOldSql.c_str() + wOldByte, sqlPos - notePos) ;
   wOldByte += sqlPos - notePos ;
   // 写入sql语句
   ofs.write( beginNote.c_str(), beginNote.length() ) ;
   ofs.write( sqlText.c_str(), sqlText.length() ) ;
   ofs.write( endNote.c_str(), endNote.length() ) ;
   ofs.write( strOldSql.c_str() + wOldByte, strOldSql.length() - wOldByte) ;
}

// 获取相应文件的处理对象
shared_ptr<SqlPatchInterface> getSqlPatch(const string &strFileName)
{
   // 如果后缀是table_design
   auto posDot = strFileName.find_last_of('.') ;
   if ( string::npos != strFileName.find("table_design", posDot) ) {
      qDebug() << "table_design" ;
      return shared_ptr<SqlPatchInterface>(new SqlTable(strFileName)) ;
   }
   // 如果是sysconfig.xml
   auto baseName = boost::filesystem::basename(strFileName) ;// p( strFileName ) ;
   //qDebug() << toQStr(baseName) ;
   if ( baseName == "sysconfig" ) {
      return shared_ptr<SqlPatchInterface>(new SqlConfig(strFileName)) ;
   }
   
   // 如果是errNumbers.xml
   if ( baseName == "errNumbers" ) {
      return shared_ptr<SqlPatchInterface>(new SqlError(strFileName)) ;
   }
   // 如果是bussniessflag.xml
   if ( baseName == "bussniessflag" ) {
      return shared_ptr<SqlPatchInterface>(new SqlBusiness(strFileName)) ;
   }
   return nullptr ;
}
