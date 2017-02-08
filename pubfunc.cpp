#include "pubfunc.h"

const char *toCStr(const QString &qStr)
{
   static string str ;
   str = qStr.toLocal8Bit().toStdString();
   return str.c_str() ;
}

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
   // 读取源文件，将UTF8->GBK，再放入property_tree
   pt.clear();
   ifstream ifs(fileName) ;
   if ( !ifs ) {
      return ;
   }
   string strText ;
   std::copy( std::istreambuf_iterator<char>(ifs),
              std::istreambuf_iterator<char>(),
              std::back_inserter(strText)) ;
   ifs.close();
   strText = boost::locale::conv::between(strText, "gbk", "utf8") ;
   std::stringstream iostr(strText) ;
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
