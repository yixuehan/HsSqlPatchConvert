#include "pubfunc.h"

string toStr(const QString &qStr)
{
   //static string str ;
   return qStr.toLocal8Bit().toStdString();
   //return str.c_str() ;
}


//const char *toCStr(const QString &qStr)
//{
//   static string str ;
//   str = qStr.toLocal8Bit().toStdString();
//   return str.c_str() ;
//}

QString toQStr(const string &str)
{
   return QString::fromLocal8Bit(str.c_str()) ;
}

//const char *toCStr(const string &str)
//{
//   static string _str ;
//   _str = str;
//   return _str.c_str();
//}


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

// 获取剪切板
shared_ptr<map<string, CopyInfo> > getCopy()
{
   static shared_ptr<map<string, CopyInfo>> pCopyInfo ;
   
   if ( nullptr == pCopyInfo ) {
      pCopyInfo.reset( new map<string, CopyInfo>);
   }
   return pCopyInfo ;
}

// 获取字段名获取对应的oracle类型和默认值
void getTypeValue( const string &fieldName, string &strType, string &defaultValue )
{
   static bool bInit = false ;
   static map<string, string> mapfield2type ;
   if ( !bInit ) {
      map<string, string> mapStdfields ;
      map<string, string> mapDataType ;
      boost::property_tree::ptree pt ;
      readPtreeFromUtf8(getSetting()->get<string>("stdfieldsFile"), pt);
      for ( const auto &child : pt.get_child("hsdoc")) {
         if ( "stdfield" == child.first )
            mapStdfields[child.second.get<string>("<xmlattr>.name")] =
               child.second.get<string>("<xmlattr>.type") ;
      }
      readPtreeFromUtf8(getSetting()->get<string>("dataTypeFile"), pt);
      for ( const auto &child : pt.get_child("hsdoc")) {
         if ( "userType" == child.first )
            mapDataType[child.second.get<string>("<xmlattr>.name")] =
                  child.second.get<string>("database.map.<xmlattr>.value") ;
      }
      for ( const auto &valuePair : mapStdfields ) {
         mapfield2type[valuePair.first] = mapDataType[valuePair.second] ;
      }
   }
   strType = mapfield2type[fieldName] ;
   defaultValue = (string::npos == strType.find("number") ? "'' ''" : "0.0") ;
}
