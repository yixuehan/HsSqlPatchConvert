#include "sqlpatch.h"
#include <boost/filesystem.hpp>
#include "sqlbusiness.h"
#include "sqlconfig.h"
#include "sqlerror.h"
#include "sqltable.h"

std::map<string, SqlInfo> SqlPatchInterface::mapSqlInfo ;


SqlPatchInterface::SqlPatchInterface( const string& strFileName )
   :fileName(strFileName)
{
   readPtreeFromUtf8(fileName, pt);
   //init() ;
}

void SqlPatchInterface::genSql( const string &strBase )
{
   genAdd( strBase ) ;
}

void SqlPatchInterface::genNote( const string &strBase )
{
   boost::format fmtNote(getSetting()->get<string>(strBase + ".note")) ;
   boost::format fmtCopy(getSetting()->get<string>("copyFmt")) ;
   // �޸ĵ��� �汾 
   auto & mapCopy = *getCopy() ;
   // --V8.15.1.%1%   %2%-%3%-%4%      %5%   %6%   %7% 	 %8%" 
   // �汾��   �� ��  ��  �޸ĵ�  �޸��� ������  �޸�˵��
   string notePosStr = getSetting()->get<string>(strBase + ".notePosStr") ;
   auto &notePos = mapSqlInfo[strBase].notePos ;
   const auto &strOldSql = mapSqlInfo[strBase].sqlOldText ;
   notePos = strOldSql.find( notePosStr ) ;
   qDebug() << toQStr(notePosStr) << ":" << (size_t)notePos ;
//      QMessageBox msg ;
//      msg.setText(toQStr(strOldSql)) ;
//      msg.exec() ;
//      msg.setText(toQStr(notePosStr))  ;
//      msg.exec() ;
   // ��¼�޸ļ�¼���ļ�λ��
   notePos += notePosStr.length() ;
   // ����汾��
   auto preLength = getSetting()->get<int>(strBase + ".preLength") ;
   auto spacePos = strOldSql.find_first_of(' ', notePos + preLength ) ;
   // ȡ�汾
   auto dotPos = strOldSql.find_last_of('.', spacePos ) ;
   string strVer = strOldSql.substr(dotPos+1, spacePos - dotPos-1) ;
   //qDebug() << toQStr("strBase:" + strBase + "|||strVer:" + strVer + "|||fmtCopy" + fmtCopy.str()) ;
   fmtNote % (std::stoi(strVer)+1) ;
   
   boost::gregorian::date d(boost::gregorian::day_clock::local_day()) ;
   
   fmtNote % (int)d.year() % (int)d.month() % (int)d.day() 
           % getSetting()->get<string>("modifyNo")
           % getSetting()->get<string>("userName")
           % getSetting()->get<string>("applyUserName")
           % mapSqlInfo[strBase].changeNote ;
   
   spacePos = fmtNote.str().find_first_of(' ', preLength ) ;
   fmtCopy % getSetting()->get<string>("modifyNo") % fmtNote.str().substr( preLength, spacePos- preLength ) ;
   
   mapSqlInfo[strBase].note = fmtNote.str() ;
   mapSqlInfo[strBase].beginNote = boost::algorithm::replace_first_copy(mapSqlInfo[strBase].note, "--", "\r\n-- begin ") ;
   mapSqlInfo[strBase].endNote = boost::algorithm::replace_first_copy(mapSqlInfo[strBase].note, "--", "-- end ") ;
   
   // ��¼��������
   struct CopyInfo copyInfo ;
   copyInfo.name = boost::filesystem::basename( mapSqlInfo[strBase].sqlFileName ) ;
   copyInfo.strCopy = fmtCopy.str() ;
   mapCopy[copyInfo.name] = copyInfo ;
}


void SqlPatchInterface::setSelectedFields( const vector<string> &fields )
{
//   QMessageBox msg ;
//   msg.setText("SqlPatch<T>::setSelectedFields") ;
//   msg.exec( ) ;
   selectedFields = fields ;
}

const vector<string> SqlPatchInterface::getSelectedFields() const
{
   return selectedFields ;
}

const boost::property_tree::ptree &SqlPatchInterface::getPTree() const
{
   return pt ;
}

void SqlPatchInterface::writeSql( const string &strBase )
{
   ofstream ofs(getSetting()->get<string>(strBase + ".fileName"), ios::binary) ;
   // ��д�������޸�˵����λ�ã�д���޸�˵������д������sql����λ�ã�д��sql��䣬��д���ļ�
   size_t wOldByte = 0;
   auto &strOldSql = mapSqlInfo[strBase].sqlOldText ;
   auto &notePos = mapSqlInfo[strBase].notePos ;
   auto &note = mapSqlInfo[strBase].note ;
   auto &beginNote  = mapSqlInfo[strBase].beginNote ;
   auto &endNote  = mapSqlInfo[strBase].endNote ;
   auto &sqlText = mapSqlInfo[strBase].sqlText ;
   ofs.write( strOldSql.c_str(), notePos ) ;
   wOldByte = notePos ;
   ofs.write( note.c_str(), note.length() ) ;
   // ��ȡд��sql����λ��
   size_t sqlPos = strOldSql.find(getSetting()->get<string>(strBase + ".sqlEnd")) - 1 ;
   ofs.write( strOldSql.c_str() + wOldByte, sqlPos - notePos) ;
   wOldByte += sqlPos - notePos ;
   // д��sql���
   ofs.write( beginNote.c_str(), beginNote.length() ) ;
   ofs.write( sqlText.c_str(), sqlText.length() ) ;
   ofs.write( endNote.c_str(), endNote.length() ) ;
   ofs.write( strOldSql.c_str() + wOldByte, strOldSql.length() - wOldByte) ;
}

// ��ȡ��Ӧ�ļ��Ĵ������
shared_ptr<SqlPatchInterface> getSqlPatch(const string &strFileName)
{
   // �����׺��table_design
   auto posDot = strFileName.find_last_of('.') ;
   if ( string::npos != strFileName.find("table_design", posDot) ) {
      qDebug() << "table_design" ;
      return shared_ptr<SqlPatchInterface>(new SqlTable(strFileName)) ;
   }
   // �����sysconfig.xml
   auto baseName = boost::filesystem::basename(strFileName) ;// p( strFileName ) ;
   //qDebug() << toQStr(baseName) ;
   if ( baseName == "sysconfig" ) {
      return shared_ptr<SqlPatchInterface>(new SqlConfig(strFileName)) ;
   }
   
   // �����errNumbers.xml
   if ( baseName == "errNumbers" ) {
      return shared_ptr<SqlPatchInterface>(new SqlError(strFileName)) ;
   }
   // �����bussniessflag.xml
   if ( baseName == "bussniessflag" ) {
      return shared_ptr<SqlPatchInterface>(new SqlBusiness(strFileName)) ;
   }
   return nullptr ;
}
