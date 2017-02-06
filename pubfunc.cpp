#include "pubfunc.h"



const char *toCStr(const QString &qStr)
{
   static string str ;
   str = qStr.toLocal8Bit().toStdString();
   return str.c_str() ;
}

//const char *toCStr(const char *str)
//{
//   return str ;
//}


QString toQStr(const string &str)
{
   return QString::fromLocal8Bit(str.c_str()) ;
}

const char *toCStr(const string &str)
{
   static string _str ;
   _str = str;
   return _str.c_str();
}


void readPtreeFromUtf8(const string &fileName, boost::property_tree::ptree &pt)
{ 
   pt.clear();
   ifstream ifs(fileName) ;
   if ( !ifs ) {
      return ;
   }
   ifs.seekg(0, ios::end) ;
   size_t filesize = ifs.tellg();
   char *p = new char[filesize+1] ;
   boost::shared_array<char> pArray(p) ;
   
   memset(pArray.get(), 0, filesize+1) ;
   ifs.seekg(0, ios::beg);
   ifs.read(pArray.get(), filesize) ;
   auto outstr = boost::locale::conv::between(pArray.get(),"gbk", "utf8") ;

   std::stringstream iostr(outstr) ;
   
   boost::property_tree::read_xml(iostr, pt);
   
}

shared_ptr<boost::property_tree::ptree> getSetting()
{
   static shared_ptr<boost::property_tree::ptree> pSetting ;
   if ( nullptr == pSetting ) {
      pSetting.reset(new boost::property_tree::ptree);
      boost::property_tree::read_info("conf/setting.info", *pSetting);
   }
   return pSetting ;
}

//void setSetting(const boost::property_tree::ptree &pt)
//{
//   *pSetting = new boost::property_tree::ptree(pt) ;
//}

shared_ptr<map<string, CopyInfo> > getCopy()
{
   static shared_ptr<map<string, CopyInfo>> pCopyInfo ;
   
   if ( nullptr == pCopyInfo ) {
      pCopyInfo.reset( new map<string, CopyInfo>);
   }
   return pCopyInfo ;
}
