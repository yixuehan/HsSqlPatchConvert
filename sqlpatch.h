#ifndef SQLPATCH_H
#define SQLPATCH_H
#include <string>
#include <vector>
#include "pubfunc.h"
#include <boost/format.hpp>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <QDate>
#include <QDebug>
#include <algorithm>
#include <QMessageBox>
#include <QFileInfo>
using namespace std ;

void getTypeValue( const string &fieldName, string &strType, string &defaultValue) ;

struct SqlTable
{
   SqlTable()
      :bNeed(false)
   {}
   bool bNeed ;
} ;

// id    配置名称  配置类型  数据类型  char   int str  remark
struct SqlConfig
{
   string id ;
   string name ;
   string configType ;
   string dataType ;
   string intValue ;
   string charValue ;
   string strValue ;
   string remark ;
   string manager_level ;
   string access_level ;
};

struct SqlInfo
{
   SqlInfo()
      : notePos(0),
        bNeed(false)
   {}
   bool bNeed ;
   string sqlFileName ;
   string sqlText ;
   string sqlOldText ;
   string beginNote ;
   string endNote ;
   string note ;
   size_t notePos ;
   string changeNote ;
} ;

struct SqlErr{} ;

class SqlPatchInterface
{
public:
//   virtual void setTableName( const string &tableName ) = 0 ;
   virtual void setSelectedFields( const vector<string> &fields ) = 0 ;
   virtual const vector<string> getTableFields() const = 0 ;
   virtual const vector<string> getSelectedFields() const = 0 ;
   virtual void genSql( ) = 0 ;
};

template <class T>
class SqlPatch : public SqlPatchInterface
{
public:
   SqlPatch( const string& strFileName )
      :fileName(strFileName)
   {
      readPtreeFromUtf8(fileName, pt);
      init() ;
   }
   
   void init()
   {
      specialInit();
      if ( !bInit ) {
         // begin 记录原文件内容
         //ifstream ifs(sqlFileName, ios::binary ) ;
         //std::copy(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>(), std::back_inserter(sqlOldText)) ;
         bInit = true ;
      }
   }
   
   void specialInit() ;
   
   void genSql( ) ;
   
   void genSql( const string &strBase )
   {
      genAdd( strBase ) ;
   }
 
   
   void genAdd( const string &strBase ) ;
//   {
//      // 获取sql模板
//      boost::format fmtSql(getSetting()->get<string>(strBase + ".sqlAdd")) ;
//      // 表名 字段名 类型  默认值
//      for ( const auto &stdfield : selectedFields ) {
//         fmtSql.clear() ;
//         string strType ;
//         string strDefaultValue ;
//         getTypeValue( stdfield, strType, strDefaultValue );
//         fmtSql % tableName % stdfield % strType % strDefaultValue ;
//         mapSqlInfo[strBase].sqlText += fmtSql.str() + "\r\n" ;
//      }
//   }
   
   static void genNote( const string &strBase )
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
      
      QDate d = QDate::currentDate(); 
      fmtNote % d.year() % d.month() % d.day() 
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
      
      QFileInfo fileInfo ;
      fileInfo.setFile(toQStr(mapSqlInfo[strBase].sqlFileName));
      copyInfo.name = toCStr(fileInfo.baseName()) ;
      copyInfo.strCopy = fmtCopy.str() ;
      mapCopy[copyInfo.name] = copyInfo ;
      
      //qDebug() << toQStr(note + ":" + beginNote + ":" + endNote) ;
   }

   void setSelectedFields( const vector<string> &fields )
   {
      selectedFields = fields ;
   }

   const vector<string> getTableFields() const ; //  必须特化
   
   const vector<string> getSelectedFields() const
   {
      return /*mapSqlInfo["sqlCom"].*/selectedFields ;
   }
   
   const boost::property_tree::ptree &getPTree() const
   {
      return pt ;
   }
   
   static void writeSql() ;
   
private:
   static void writeSql( const string &strBase )
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

private:
   vector<string> selectedFields ;
   mutable vector<string> fields ;
   boost::property_tree::ptree pt ;
   string tableName ;
   string fileName ;
   mutable std::map<string, T> mapTableInfo ; // 记录字段的信息
   static std::map<string, SqlInfo> mapSqlInfo ;  // 记录表的sql语句
   static bool bInit ;
};


shared_ptr<SqlPatchInterface> getSqlPatch( const string &strFileName ) ;

#endif // SQLPATCH_H
