#ifndef SQLTABLE_H
#define SQLTABLE_H

#include "sqlpatch.h"

class SqlTable : public SqlPatchInterface 
{
public:
   SqlTable(const string &strFile) ;
   static void writeSql() ;
   virtual void init() ;
   
   virtual void genSql( ) ;
   
   //virtual void genSql( const string &strBase ) ;
   virtual void genAdd( const string &strBase ) ;
   virtual void setSelectedFields( const vector<string> &fields ) ;
   virtual const vector<string> getTableFields() const ; 
   
   //virtual const vector<string> getSelectedFields() const ;
   //virtual const boost::property_tree::ptree &getPTree() const ;
private:
   static bool bInit ;
   mutable std::map<string, FileTable> mapTableInfo ; // 记录每张表的字段信息
};

#endif // SQLTABLE_H
