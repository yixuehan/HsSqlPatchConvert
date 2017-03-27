#ifndef SQLPATCH_H
#define SQLPATCH_H
#include <string>
#include <vector>
#include "pubfunc.h"
#include <boost/format.hpp>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <QDebug>
#include <algorithm>
#include <QMessageBox>
#include <boost/filesystem.hpp>
#include <boost/date_time.hpp>
using namespace std ;

struct FileTable
{
   FileTable()
      :bNeed(false)
   {}
   bool bNeed ;
} ;

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

// id    配置名称  配置类型  数据类型  char   int str  remark
struct FileConfig
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



struct FileError
{
   string id ;
   string name ;
   string constant ;
} ;

struct FileBusiness
{
   string id; 
   string name; 
   string business_kind; 
   string business_subject; 
   string business_group; 
   string borrow_lend; 
   string join_business_flag; 
   string opp_business_flag ;
};


//class SqlPatchInterface
//{
//public:
//   //virtual void setTableName( const string &tableName ) = 0 ;
//   virtual void setSelectedFields( const vector<string> &fields ) = 0 ;
//   virtual const vector<string> getTableFields() const = 0 ;
//   virtual const vector<string> getSelectedFields() const = 0 ;
//   virtual void genSql( ) = 0 ;
//   //virtual void genSource() = 0 ;
//};

//template <class T>
class  SqlPatchInterface
{
public:
   SqlPatchInterface( const string& strFileName );

   virtual void init() = 0 ;
   
   virtual void genSql( ) = 0;
   
   virtual void genSql( const string &strBase ) ;
 
   virtual void genAdd( const string &strBase ) = 0 ;

   static void genNote( const string &strBase ) ;

   virtual void setSelectedFields( const vector<string> &fields );

   virtual const vector<string> getTableFields() const = 0; 
   
   virtual const vector<string> getSelectedFields() const ;
   
   virtual const boost::property_tree::ptree &getPTree() const ;
  
protected:
   static void writeSql( const string &strBase );

protected:
   vector<string> selectedFields ;
   mutable vector<string> fields ;
   boost::property_tree::ptree pt ;
   string tableName ;
   string fileName ;
   //mutable std::map<string, T> mapTableInfo ; // 记录每张表的字段信息
   static std::map<string, SqlInfo> mapSqlInfo ;  // 记录每个文件的sql语句
   
};

//template <class T>
//std::map<string, SqlInfo> SqlPatch<T>::mapSqlInfo ;



//template <class T>
//bool SqlPatch<T>::bInit = false ;


// 获取相应文件的处理对象
shared_ptr<SqlPatchInterface> getSqlPatch( const string &strFileName ) ;

//#include "sqltable.cpp"
//#include "sqlconfig.cpp"
//#include "sqlbusiness.cpp"
//#include "sqlerror.cpp"
//#include "sqlpatch.cpp"

#endif // SQLPATCH_H
