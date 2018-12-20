/****************************************************************************
**
** This file is part of the LibreCAD project, a 2D CAD program
**
**  Copyright (C) 2011 Rallaz, rallazz@gmail.com
** Copyright (C) 2010 R. van Twisk (librecad@rvt.dds.nl)
**
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License as published by the Free Software
** Foundation either version 2 of the License, or (at your option)
**  any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
**
**********************************************************************/
#include "stdafx.h"

#ifndef DXFLIB
#include "filterdxfrw.h"
//#include "lib/libdxfrw.h"

#include <stdio.h>


/**
 * Default constructor.
 *
 */
FilterDXFRW::FilterDXFRW( dxfRW* dxf_in )
	:dxf( dxf_in )
	//:DRW_Interface( )
{
}

/**
 * Destructor.
 */
FilterDXFRW::~FilterDXFRW( )
{
}


/**
 * Implementation of the method used for communicate
 * with this filter.
 *
 * @param g The graphic in which the entities from the file
 * will be created or the graphics from which the entities are
 * taken to be stored in a file.
 */
bool FilterDXFRW::fileImport( const std::string& file, std::string& cp, const std::string& d, const std::string& l, const std::string& t)
{
    doubleLine = d;
    layerText = l;
    textText = t;
    cp.clear( );

    dxf = new dxfRW( file.c_str( ) );
    bool success = dxf->read( this, true );

    if( ! success )
	{
        return false;
    }
    delete dxf;
//    cp.append( codePage.be );
    return true;
}


/**
 * Implementation of the method which handles layers.
 */
void FilterDXFRW::addLayer( const DRW_Layer &data )
{

    //QString name =QString::fromUtf8(data.name.c_str());
    //layerText->append(name);
}

/**
 * Implementation of the method which handles line entities.
 */
void FilterDXFRW::addLine(const DRW_Line& data)
{
    // QString str = QString("%1").arg(data.basePoint.x, 0, 'f', 8);
    //doubleLine->setText( str );
}


/**
 * Implementation of the method which handles line entities.
 */
void FilterDXFRW::addMText(const DRW_MText& data)
{
    //QString cont = toNativeString( QString::fromUtf8(data.text.c_str()) );
    //textText->append( cont );
}

/**
 * Sets the header variables from the DXF file.
 */
void FilterDXFRW::addHeader(const DRW_Header* data)
{
    std::map<std::string,DRW_Variant *>::const_iterator it;
    for ( auto it : data->vars )//.begin() ; it != data->vars.end(); it++ )
	{
 /*       QString key = QString::fromStdString((*it).first);
        if (key == "$DWGCODEPAGE")
		{
            DRW_Variant *var = (*it).second;
            codePage = QString::fromStdString(*var->content.s);
        }
 */   }
}


/**
 * Implementation of the method used for Export to communicate
 * with this filter.
 *
 * @param file Full path to the DXF file that will be written.
 */
bool FilterDXFRW::fileExport( const std::string& file, DRW::Version v, std::string& cp, std::string& l, std::string& t )
{
    codePage = cp;
    layerText = l;
    textText = t;
    // check if we can write to that directory:
#ifndef Q_OS_WIN

    //QString path = QFileInfo(file).absolutePath();
    //if (QFileInfo(path).isWritable()==false) {
    //    return false;
    //}
    //
#endif

    // set version for DXF filter:
    DRW::Version exportVersion = v;
    version = v;

    dxf = new dxfRW( file.c_str( ) );
    bool success = dxf->write(this, exportVersion, false); //ascii
//    bool success = dxf->write(this, exportVersion, true); //binary

    if (!success) {
        return false;
    }
    return success;
}

/**
 * Writes blocks (just the definition, not the entities in it).
 */
void FilterDXFRW::writeBlockRecords(){
}

void FilterDXFRW::writeBlocks() {
}


void FilterDXFRW::writeHeader(DRW_Header& data){
    DRW_Variant *curr = new DRW_Variant();
    if (version == DRW::AC1009)
        curr->addString("AC1009");
    else if (version == DRW::AC1014)
        curr->addString("AC1014");
    else if (version == DRW::AC1015)
        curr->addString("AC1015");
    else if (version == DRW::AC1018)
        curr->addString("AC1018");
    else if (version == DRW::AC1021)
        curr->addString("AC1021");
    else
        curr->addString("AC1024");
    curr->code = 1;
    data.vars["$$ACADVER"] =curr;

    curr = new DRW_Variant();
    if( ! codePage.size( ) || codePage == "unknown" )
        curr->addString("ANSI_1252");
    else
        curr->addString( codePage );
    curr->code = 3;
    data.vars["$DWGCODEPAGE"] =curr;
}

void FilterDXFRW::writeLTypes(){
}

void FilterDXFRW::writeLayers(){
    DRW_Layer lay;
    //const QString layers =layerText->toPlainText();
    //QStringList layerNames = layers.split('\n', QString::SkipEmptyParts);
    //for (int i = 0; i < layerNames.size(); ++i) {
    //    lay.name = layerNames.at(i).toUtf8().data();
    //    dxf->writeLayer(&lay);
    //}
}

void FilterDXFRW::writeDimstyles(){
}

void FilterDXFRW::writeEntities(){
writeLine();
writeMtext();
}

void FilterDXFRW::writeLine() {
    DRW_Line l;
    //double start = 1.2564;
    //QString layers =layerText->toPlainText();
    //QStringList layerNames = layers.split('\n', QString::SkipEmptyParts);
    //for (int i = 0; i < layerNames.size(); ++i) {
    //    l.basePoint.x = 1.2564;
    //    l.basePoint.y = start * (i+1);
    //    l.secPoint.x = 8.6897;
    //    l.secPoint.y = start * (i+1);
    //    l.layer = layerNames.at(i).toUtf8().data();
    //    dxf->writeLine(&l);
    //}
}

void FilterDXFRW::writeMtext() {
    DRW_MText t;
    std::string text =textText;//->toPlainText();
        t.basePoint.x = 8.6897;
        t.basePoint.y = 8.6897;
        t.text = text;//.toUtf8().data();
        //t.text = toDxfString(text).toUtf8().data();
        dxf->writeMText(&t);
}



/**
 * Converts a native unicode string into a DXF encoded string.
 *
 * DXF endoding includes the following special sequences:
 * - %%%c for a diameter sign
 * - %%%d for a degree sign
 * - %%%p for a plus/minus sign
 */
/*
std::string FilterDXFRW::toDxfString(const std::string& str) {
    std::string res;
    int j=0;
    for( size_t i= 0; i < str.size( ); ++i )
	{
        int c = str.at(i);//.unicode();
        if (c>175 || c<11){
            res.append( str.substr( j, i - j ) );
            j=i;

            switch (c) {
            case 0x0A:
                res+="\\P";
                break;
                // diameter:
#ifdef Q_OS_WIN
    //windows, as always, is special.
            case 0x00D8:
#else
            case 0x2205:
#endif
                res+="%%C";
                break;
                // degree:
            case 0x00B0:
                res+="%%D";
                break;
                // plus/minus
            case 0x00B1:
                res+="%%P";
                break;
            default:
                j--;
                break;
            }
            j++;
        }

    }
    res.append( str.substr( j ) );
    return res;
}



/**
 * Converts a DXF encoded string into a native Unicode string.
 */
/*
std::string FilterDXFRW::toNativeString( const std::string& data) {
    std::string res;

    // Ignore font tags:
    int j = 0;
    for( size_t i= 0; i < data.size( ); ++i )
	{
        if (data.at( i ) == 0x7B){ //is '{' ?
            if( data.at( i + 1 ) == 0x5c && data.at( i + 2 ) == 0x66 )
			{ //is "\f" ?
                //found font tag, append parsed part
                res.append(data.substr( j, i - j ) );
                //skip to ';'
                for( size_t k= i + 3; k < data.size( ); ++k )
				{
                    if( data.at( k ) == 0x3B )
					{
                        i= j= ++k;
                        break;
                    }
                }
                //add to '}'
                for( size_t k= i; k < data.size( ); ++k )
				{
                    if (data.at( k ) == 0x7D)
					{
                        res.append( data.substr( i, k - i ) );
                        i= j= ++k;
                        break;
                    }
                }

            }
        }
    }
    res.append( data.substr( j ) );

    // Line feed:
    //res = res.replace(QRegExp("\\\\P"), "\n");
    //// Space:
    //res = res.replace(QRegExp("\\\\~"), " ");
    //// diameter:
#ifdef Q_OS_WIN
    //windows, as always, is special.
    res = res.replace(QRegExp("%%[cC]"), QChar(0xD8));
#else
//    res = res.replace(QRegExp("%%[cC]"), QChar(0x2205));//Empty_set, diameter is 0x2300
#endif
    //// degree:
    //res = res.replace(QRegExp("%%[dD]"), QChar(0x00B0));
    //// plus/minus
    //res = res.replace(QRegExp("%%[pP]"), QChar(0x00B1));

    return res;
}

// EOF
*/

#endif //  DXFLIB

