#ifndef SQLERROR_H
#define SQLERROR_H

#include "sqlpatch.h"


class SqlError : public SqlPatchInterface 
{
public:
   SqlError(const string &strFile) ;
   static void writeSql() ;
   virtual void init() ;
   
   virtual void genSql( ) ;
   
   //virtual void genSql( const string &strBase ) ;
   virtual void genAdd( const string &strBase ) ;
   //virtual void setSelectedFields( const vector<string> &fields ) ;
   virtual const vector<string> getTableFields() const ; 
   
   //virtual const vector<string> getSelectedFields() const ;
   //virtual const boost::property_tree::ptree &getPTree() const ;
private:
   static bool bInit ;
   mutable std::map<string, FileError> mapTableInfo ; // 记录每张表的字段信息
};

#endif // SQLERROR_H
