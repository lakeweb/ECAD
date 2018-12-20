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


#ifndef FILTERDXFRW_H
#define FILTERDXFRW_H

//class dxfRW;

/**
 * This format filter class can import and export DXF files.
 * It depends on the dxflib library.
 *
 * @author Rallaz
 */
class FilterDXFRW : public DRW_Interface
{
public:
    FilterDXFRW( dxfRW* dxf_in );
    ~FilterDXFRW( );
	
    // Import:
    bool fileImport(const std::string& file, std::string& cp, const std::string& d, const std::string& l, const std::string& t);

    // Methods from DRW_CreationInterface:
    virtual void addHeader(const DRW_Header* data);
    virtual void addLType(const DRW_LType& /*data*/){}
    virtual void addLayer(const DRW_Layer& data);
    virtual void addDimStyle(const DRW_Dimstyle& /*data*/){}
    virtual void addBlock(const DRW_Block& /*data*/){}
    virtual void endBlock(){}
    virtual void addPoint(const DRW_Point& /*data*/){}
    virtual void addLine(const DRW_Line& data);
    virtual void addRay(const DRW_Ray& /*data*/){}
    virtual void addXline(const DRW_Xline& /*data*/){}
    virtual void addCircle(const DRW_Circle& /*data*/){}
    virtual void addArc(const DRW_Arc& /*data*/){}
    virtual void addEllipse(const DRW_Ellipse& /*data*/){}
    virtual void addLWPolyline(const DRW_LWPolyline& /*data*/){}
    virtual void addText(const DRW_Text& /*data*/){}
    virtual void addPolyline(const DRW_Polyline& /*data*/){}
    virtual void addSpline(const DRW_Spline* /*data*/){}
    virtual void addKnot(const DRW_Entity&) {}
    virtual void addInsert(const DRW_Insert& /*data*/){}
    virtual void addTrace(const DRW_Trace& /*data*/){}
    virtual void addSolid(const DRW_Solid& /*data*/){}
    virtual void addMText(const DRW_MText& data);
    virtual void addDimAlign(const DRW_DimAligned* /*data*/){}
    virtual void addDimLinear(const DRW_DimLinear* /*data*/){}
    virtual void addDimRadial(const DRW_DimRadial* /*data*/){}
    virtual void addDimDiametric(const DRW_DimDiametric* /*data*/){}
    virtual void addDimAngular(const DRW_DimAngular* /*data*/){}
    virtual void addDimAngular3P(const DRW_DimAngular3p* /*data*/){}
    virtual void addDimOrdinate(const DRW_DimOrdinate* /*data*/){}
    virtual void addLeader(const DRW_Leader* /*data*/){}
    virtual void addHatch(const DRW_Hatch* /*data*/){}
    virtual void addImage(const DRW_Image* /*data*/){}
    virtual void linkImage(const DRW_ImageDef* /*data*/){}

    virtual void add3dFace(const DRW_3Dface& /*data*/){}
    virtual void addComment(const char*){}

	/** Called for every VPORT table. */
	virtual void addVport(const DRW_Vport& data) { };
	/** Called for every text style. */
	virtual void addTextStyle(const DRW_Textstyle& data) { }
	//virtual void addViewport(const DRW_Viewport& data){ };
	virtual void addAppId(const DRW_AppId& data) { }
	virtual void setBlock(const int handle) { }
	virtual void addViewport(const DRW_Viewport& data) { }
	virtual void writeTextstyles() { }
	virtual void writeVports() { }
	virtual void writeAppId() { }

    // Export:
    bool fileExport( const std::string& file, DRW::Version v, std::string& cp, std::string& l, std::string& t);

    virtual void writeHeader(DRW_Header& data);
    virtual void writeEntities();
    virtual void writeLTypes();
    virtual void writeLayers();
    virtual void writeBlockRecords();
    virtual void writeBlocks();
    virtual void writeDimstyles();

    void writeLine();
    void writeMtext();

 /*   static std::string toDxfString(const std::string& str);
    static std::string toNativeString(const std::string& data);
*/

private:
    int version;
	std::string codePage;
    dxfRW *dxf;
	std::string doubleLine;
	std::string layerText;
	std::string textText;

};

#endif
