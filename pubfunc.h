#ifndef PUBFUNC_H
#define PUBFUNC_H
#include <QString>
#include <string>
#include <map>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/info_parser.hpp>
//#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/locale.hpp>
using namespace std ;

//const char *toCStr(const QString &str);

QString toQStr(const string &str);

string toStr(const QString &qStr) ;

void readPtreeFromUtf8(const string &fileName, boost::property_tree::ptree &pt);

struct FileInfo
{
   std::string fileName ;
   boost::property_tree::ptree pt ;
};

shared_ptr<boost::property_tree::ptree> getSetting() ;

struct CopyInfo
{
   string name ;
   string strCopy ;
};

// ��ȡ���а�
shared_ptr<map<string, CopyInfo>> getCopy() ;

// ��ȡ�ֶ�����ȡ��Ӧ��oracle���ͺ�Ĭ��ֵ
void getTypeValue( const string &fieldName, string &strType, string &defaultValue ) ;

#endif // PUBFUNC_H
