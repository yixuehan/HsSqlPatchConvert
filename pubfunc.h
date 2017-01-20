#ifndef PUBFUNC_H
#define PUBFUNC_H
#include <QString>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/info_parser.hpp>
//#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/locale.hpp>
using namespace std ;

const char *toCStr(const QString &str);

//const char *toCStr(const char *str);

QString toQStr(const string &str);

const char *toCStr(const string &str);

void readPtreeFromUtf8(const string &fileName, boost::property_tree::ptree &pt);


struct FileInfo
{
   std::string fileName ;
   // std::vector<std::string> addFields ;
   boost::property_tree::ptree pt ;
};

shared_ptr<boost::property_tree::ptree> getSetting() ;

// void setSetting( const boost::property_tree::ptree &) ;


#endif // PUBFUNC_H
