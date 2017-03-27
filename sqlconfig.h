#ifndef SQLCONFIG_H
#define SQLCONFIG_H

#include "sqlpatch.h"


class SqlConfig : public SqlPatchInterface 
{
public:
   SqlConfig(const string &strFile) ;
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
   mutable std::map<string, FileConfig> mapTableInfo ; // ��¼ÿ�ű���ֶ���Ϣ
};

#endif // SQLCONFIG_H
