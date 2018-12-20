/*
 * @file test_creationclass.cpp
 */

/*****************************************************************************
**  $Id: test_creationclass.cpp 8865 2008-02-04 18:54:02Z andrew $
**
**  This is part of the dxflib library
**  Copyright (C) 2001 Andrew Mustun
**
**  This program is free software; you can redistribute it and/or modify
**  it under the terms of the GNU Library General Public License as
**  published by the Free Software Foundation.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU Library General Public License for more details.
**
**  You should have received a copy of the GNU Library General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
******************************************************************************/

#include "stdafx.h"

#include "test_creationclass.h"

#include <iostream>
#include <stdio.h>

std::ofstream dout;


/**
 * Default constructor.
 */
Test_CreationClass::Test_CreationClass( )
{
}


/**
 * Sample implementation of the method which handles layers.
 */
void Test_CreationClass::addLayer(const DL_LayerData& data)
{
	dout << "LAYER: " << data.name.c_str() << " flags: " << data.flags << std::endl;
    printf("LAYER: %s flags: %d\n", data.name.c_str(), data.flags);
    printAttributes();
}

void Test_CreationClass::addBlock( const DL_BlockData& block )
{
	dout << "BLOCK: " << block.name << std::endl;
	printAttributes( );
}

/**
 * Sample implementation of the method which handles point entities.
 */
void Test_CreationClass::addPoint(const DL_PointData& data) {
	dout << "POINT x: " << data.x << " y: " << data.y << " z: " << data.z << std::endl;
	printf("POINT    (%6.3f, %6.3f, %6.3f)\n",
           data.x, data.y, data.z);
    printAttributes();
}

/**
 * Sample implementation of the method which handles line entities.
 */
void Test_CreationClass::addLine(const DL_LineData& data) {
	dout << "LINE x: " << data.x1 << " y: " << data.y1 << " z: " << data.z1
		<< " -to- x: " << data.x2 << " y: " << data.y2 << " z: " << data.z2 << std::endl;

	printf("LINE     (%6.3f, %6.3f, %6.3f) (%6.3f, %6.3f, %6.3f)\n",
           data.x1, data.y1, data.z1, data.x2, data.y2, data.z2);
    printAttributes();
}

/**
 * Sample implementation of the method which handles arc entities.
 */
void Test_CreationClass::addArc(const DL_ArcData& data) {
    printf("ARC      (%6.3f, %6.3f, %6.3f) %6.3f, %6.3f, %6.3f\n",
           data.cx, data.cy, data.cz,
           data.radius, data.angle1, data.angle2);
    printAttributes();
}

/**
 * Sample implementation of the method which handles circle entities.
 */
void Test_CreationClass::addCircle(const DL_CircleData& data) {
    printf("CIRCLE   (%6.3f, %6.3f, %6.3f) %6.3f\n",
           data.cx, data.cy, data.cz,
           data.radius);
    printAttributes();
}


/**
 * Sample implementation of the method which handles polyline entities.
 */
void Test_CreationClass::addPolyline(const DL_PolylineData& data) {
    printf("POLYLINE \n");
    printf("flags: %d\n", (int)data.flags);
    printAttributes();
}


/**
 * Sample implementation of the method which handles vertices.
 */
void Test_CreationClass::addVertex(const DL_VertexData& data) {
    printf("VERTEX   (%6.3f, %6.3f, %6.3f) %6.3f\n",
           data.x, data.y, data.z,
           data.bulge);
    printAttributes();
}


void Test_CreationClass::add3dFace(const DL_3dFaceData& data) {
    printf("3DFACE\n");
    for (int i=0; i<4; i++) {
        printf("   corner %d: %6.3f %6.3f %6.3f\n", 
            i, data.x[i], data.y[i], data.z[i]);
    }
    printAttributes();
}

// ................................................................................
std::string getColorStr( int color )
{
	if( ! color )
		return std::string( "BYBLOCK" );
	if( color == 256 )
		return std::string( "BYLAYER" );
	return boost::lexical_cast< std::string >( color );
}

// ................................................................................
std::string getWidthStr( int width )
{
	if( width == -1 )
		return std::string( "BYLAYER" );
	if( width == -2 )
	return std::string( "BYBLOCK" );
	if( width == -3 )
		return std::string( "DEFAULT" );
	return boost::lexical_cast< std::string >( width );
}

// ................................................................................
void Test_CreationClass::printAttributes( )
{
	dout << "  -ATR Lyr: " << attributes.getLayer( ).c_str( )
		<< " color: " << getColorStr( attributes.getColor( ) )
		<< " width: " << getWidthStr( attributes.getWidth( ) )
		<< " type: " <<attributes.getLinetype( )
		<< std::endl;
//    printf("  Attributes: Layer: %s, ", attributes.getLayer().c_str());
//    printf(" Color: ");
//    if (attributes.getColor()==256)	{
//        printf("BYLAYER");
//    } else if (attributes.getColor()==0) {
//        printf("BYBLOCK");
//    } else {
//        printf("%d", attributes.getColor());
//    }
//    printf(" Width: ");
//    if (attributes.getWidth()==-1) {
//        printf("BYLAYER");
//    } else if (attributes.getWidth()==-2) {
//        printf("BYBLOCK");
//    } else if (attributes.getWidth()==-3) {
//        printf("DEFAULT");
//    } else {
//        printf("%d", attributes.getWidth());
//    }
////    printf(" Type: %s\n", attributes.getLineType().c_str());
}
    
    

// EOF
