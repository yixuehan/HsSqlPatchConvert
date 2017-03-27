#ifndef SQLBUSINESS_H
#define SQLBUSINESS_H

#include "sqlpatch.h"


class SqlBusiness : public SqlPatchInterface 
{                               
public:
   SqlBusiness( const string &strFile ) ;
   static void writeSql() ;
   virtual void init() ;
   
   virtual void genSql( ) ;
   
   virtual void genAdd( const string &strBase ) ;

   virtual const vector<string> getTableFields() const; 
   
private:
   static bool bInit ;
   mutable std::map<string, FileBusiness> mapTableInfo ; // ��¼ÿ�ű���ֶ���Ϣ
};

#endif // SQLBUSINESS_H
