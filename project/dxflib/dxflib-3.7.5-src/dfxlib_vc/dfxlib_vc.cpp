/*
* @file main.cpp
*/

/*****************************************************************************
**  $Id: main.cpp 3591 2006-10-18 21:23:25Z andrew $
**
**  This is part of the dxflib library
**  Copyright (C) 2000-2001 Andrew Mustun
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

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include <dxflib/dl_dxf.h>
#include <dxflib/dl_writer_ascii.h>


#include "test_creationclass.h"
#include "dl_creationadapter.h"


void usage();
void testReading(char* file);
void testWriting();


/*
* @brief Main function for DXFLib test program.
*
* @param argc Number of delimited items on command line,
*		including program name.
* @param argv Pointer to array of command line items
*
* @retval 0 if missing input file argument or
*		file couldn't be opened
* @retval 1 if file opened
*/

// ....................................................
void test_read( );

// ....................................................
int main( int argc, char** argv )
{

	test_read( );
	// Check given arguments:
	if( argc < 2 )
	{
		usage( );
		return 0;
	}

	testReading( argv[ 1 ] );

	testWriting( );

	return 0;
}

// ....................................................
void test_read( )
{
	{
		dout.open( "Libre_save.log", std::ios::ate );

		//	std::string filename( "myfile.dxf" );
		std::string filename( "Libre_save.dxf" );

		dout << "FROM FILE: " << filename << std::endl;

		DL_Dxf* dxf= new DL_Dxf;
		Test_CreationClass* cc= new Test_CreationClass;
		dxf->in( filename, cc ); 
		dout.close( );
	}
	{
		dout.open( "myfile.log", std::ios::ate );

		std::string filename( "myfile.dxf" );

		dout << "FROM FILE: " << filename << std::endl;

		DL_Dxf* dxf= new DL_Dxf;
		Test_CreationClass* cc= new Test_CreationClass;
		dxf->in( filename, cc ); 
		dout.close( );
	}
}

/*
* @brief Prints error message if file name not specified as command
* line argument.
*/
void usage() {
	std::cout << "\nUsage: test <DXF file>\n\n";
}

// ....................................................
void testReading(char* file) {
	// Load DXF file into memory:
	std::cout << "Reading file " << file << "...\n";
	Test_CreationClass* creationClass = new Test_CreationClass();
	DL_Dxf* dxf = new DL_Dxf();
	if (!dxf->in(file, creationClass)) { // if file open failed
		std::cerr << file << " could not be opened.\n";
		return;
	}
	delete dxf;
	delete creationClass;
}

bool StartDXF_write( DL_Dxf* dxf, DL_WriterA* dw );
bool EndDXF_write( DL_Dxf* dxf, DL_WriterA* dw );

// ....................................................

void testWriting() {
	DL_Dxf* dxf = new DL_Dxf();
	DL_Codes::version exportVersion = DL_Codes::AC1015;
	DL_WriterA* dw = dxf->out("myfile.dxf", exportVersion);
	if( ! dw )
	{
		printf("Cannot open file 'myfile.dxf' \
               for writing.");
		// abort function e.g. with return
	}
	StartDXF_write( dxf, dw );

	//....................
	dw->sectionEntities();

	// write all your entities..
	dxf->writePoint(
		*dw,
		DL_PointData(10.0,
			45.0,
			0.0),
		DL_Attributes("mainlayer", 256, -1, "BYLAYER",1));

	dxf->writeLine(
		*dw,
		DL_LineData(25.0,   // start point
			30.0,
			0.0,
			100.0,   // end point
			120.0,
			0.0),
		DL_Attributes("mainlayer", 256, -1, "BYLAYER",2));

	dw->sectionEnd();

	EndDXF_write( dxf, dw );
}

bool StartDXF_write( DL_Dxf* dxf, DL_WriterA* dw )
{
	dxf->writeHeader(*dw);
	// int variable:

	// 1 inches 4 millimeters
	dw->dxfString(9, "$INSUNITS");
	dw->dxfInt(70, 1);

	// real (double, float) variable:
	dw->dxfString(9, "$DIMEXE");
	dw->dxfReal(40, 1.25);
	// string variable:
	dw->dxfString(9, "$TEXTSTYLE");
	dw->dxfString(7, "Standard");
	// vector variable:
	dw->dxfString(9, "$LIMMIN");
	dw->dxfReal(10, 0.0);
	dw->dxfReal(20, 0.0);
	dw->sectionEnd();

	//....................
	dw->sectionTables();
	dxf->writeVPort(*dw);
	dw->tableLinetypes(25);
	dxf->writeLinetype(*dw,
		DL_LinetypeData("CONTINUOUS", "disc", 0, 0, 0 ));
	//dxf->writeLinetype(*dw, DL_LinetypeData("BYBLOCK", 0));
	//dxf->writeLinetype(*dw, DL_LinetypeData("BYLAYER", 0));
	//dxf->writeLinetype(*dw,
	//	DL_LinetypeData("ACAD_ISO02W100", 0));
	//dxf->writeLinetype(*dw,
	//	DL_LinetypeData("ACAD_ISO03W100", 0));
	//dxf->writeLinetype(*dw,
	//	DL_LinetypeData("ACAD_ISO04W100", 0));
	//dxf->writeLinetype(*dw,
	//	DL_LinetypeData("ACAD_ISO05W100", 0));
	//dxf->writeLinetype(*dw, DL_LinetypeData("BORDER", 0));
	//dxf->writeLinetype(*dw, DL_LinetypeData("BORDER2", 0));
	//dxf->writeLinetype(*dw, DL_LinetypeData("BORDERX2", 0));
	//dxf->writeLinetype(*dw, DL_LinetypeData("CENTER", 0));
	//dxf->writeLinetype(*dw, DL_LinetypeData("CENTER2", 0));
	//dxf->writeLinetype(*dw, DL_LinetypeData("CENTERX2", 0));
	//dxf->writeLinetype(*dw, DL_LinetypeData("DASHDOT", 0));
	//dxf->writeLinetype(*dw, DL_LinetypeData("DASHDOT2", 0));
	//dxf->writeLinetype(*dw,
	//	DL_LinetypeData("DASHDOTX2", 0));
	//dxf->writeLinetype(*dw, DL_LinetypeData("DASHED", 0));
	//dxf->writeLinetype(*dw, DL_LinetypeData("DASHED2", 0));
	//dxf->writeLinetype(*dw, DL_LinetypeData("DASHEDX2", 0));
	//dxf->writeLinetype(*dw, DL_LinetypeData("DIVIDE", 0));
	//dxf->writeLinetype(*dw, DL_LinetypeData("DIVIDE2", 0));
	//dxf->writeLinetype(*dw,
	//	DL_LinetypeData("DIVIDEX2", 0));
	//dxf->writeLinetype(*dw, DL_LinetypeData("DOT", 0));
	//dxf->writeLinetype(*dw, DL_LinetypeData("DOT2", 0));
	//dxf->writeLinetype(*dw, DL_LinetypeData("DOTX2", 0));
	dw->tableEnd();


	//....................
	int numberOfLayers = 3;
	dw->tableLayers(numberOfLayers);

	dxf->writeLayer(*dw,
		DL_LayerData("0", 0),
		DL_Attributes(
			std::string(""),      // leave empty
			DL_Codes::black,        // default color
			100,                  // default width
			"CONTINUOUS", 1));       // default line style

	dxf->writeLayer(*dw,
		DL_LayerData("mainlayer", 0),
		DL_Attributes(
			std::string(""),
			DL_Codes::red,
			100,
			"CONTINUOUS",1));

	dxf->writeLayer(*dw,
		DL_LayerData("anotherlayer", 0),
		DL_Attributes(
			std::string(""),
			DL_Codes::black,
			100,
			"CONTINUOUS",1));

	dw->tableEnd();

	//....................
	dw->tableStyle( 1 );
	dxf->writeStyle(*dw, DL_StyleData("standard", 0, 2.5, 1.0, 0.0, 0, 2.5, "standard", ""));
	dw->tableEnd( );

	dxf->writeView(*dw);
	dxf->writeUcs(*dw);

	dw->tableAppid(1);
	dw->tableAppidEntry(0x12);
	dw->dxfString(2, "ACAD");
	dw->dxfInt(70, 0);
	dw->tableEnd();

	//....................
	dxf->writeDimStyle(*dw,
		1,
		1,
		1,
		1,
		1);
	dxf->writeBlockRecord(*dw);
	dxf->writeBlockRecord(*dw, "myblock1");
	dxf->writeBlockRecord(*dw, "myblock2");
	dw->tableEnd();
	dw->sectionEnd();


	//....................
	dw->sectionBlocks();

	dxf->writeBlock(*dw,
		DL_BlockData("*Model_Space", 0, 0.0, 0.0, 0.0));
	dxf->writeEndBlock(*dw, "*Model_Space");

	dxf->writeBlock(*dw,
		DL_BlockData("*Paper_Space", 0, 0.0, 0.0, 0.0));
	dxf->writeEndBlock(*dw, "*Paper_Space");

	//dxf->writeBlock(*dw,
	//	DL_BlockData("*Paper_Space0", 0, 0.0, 0.0, 0.0));
	//dxf->writeEndBlock(*dw, "*Paper_Space0");

	dxf->writeBlock(*dw,
		DL_BlockData("myblock1", 0, 0.0, 0.0, 0.0));
	// ...
	// write block entities e.g. with dxf->writeLine(), ..
	// ...
	dxf->writeEndBlock(*dw, "myblock1");

	dxf->writeBlock(*dw,
		DL_BlockData("myblock2", 0, 0.0, 0.0, 0.0));
	// ...
	// write block entities e.g. with dxf->writeLine(), ..
	// ...
	dxf->writeEndBlock(*dw, "myblock2");

	dw->sectionEnd();

	return true;
}

bool EndDXF_write( DL_Dxf* dxf, DL_WriterA* dw )
{
	//....................
	dxf->writeObjects(*dw);
	dxf->writeObjectsEnd(*dw);

	dw->dxfEOF();
	dw->close();

	delete dw;
	delete dxf;
	dw= NULL;
	dxf= NULL;
	return true;
}
//// ....................................................
//void testWriting() {
//	DL_Dxf* dxf = new DL_Dxf();
//	DL_Codes::version exportVersion = DL_Codes::AC1015;
//	DL_WriterA* dw = dxf->out("myfile.dxf", exportVersion);
//	if( ! dw )
//	{
//		printf("Cannot open file 'myfile.dxf' \
//               for writing.");
//		// abort function e.g. with return
//	}
//	dxf->writeHeader(*dw);
//	// int variable:
//	dw->dxfString(9, "$INSUNITS");
//	dw->dxfInt(70, 4);
//	// real (double, float) variable:
//	dw->dxfString(9, "$DIMEXE");
//	dw->dxfReal(40, 1.25);
//	// string variable:
//	dw->dxfString(9, "$TEXTSTYLE");
//	dw->dxfString(7, "Standard");
//	// vector variable:
//	dw->dxfString(9, "$LIMMIN");
//	dw->dxfReal(10, 0.0);
//	dw->dxfReal(20, 0.0);
//	dw->sectionEnd();
//
//	//....................
//	dw->sectionTables();
//	dxf->writeVPort(*dw);
//	dw->tableLinetypes(25);
//	dxf->writeLinetype(*dw,
//		DL_LinetypeData("CONTINUOUS", "disc", 0, 0, 0 ));
//	//dxf->writeLinetype(*dw, DL_LinetypeData("BYBLOCK", 0));
//	//dxf->writeLinetype(*dw, DL_LinetypeData("BYLAYER", 0));
//	//dxf->writeLinetype(*dw,
//	//	DL_LinetypeData("ACAD_ISO02W100", 0));
//	//dxf->writeLinetype(*dw,
//	//	DL_LinetypeData("ACAD_ISO03W100", 0));
//	//dxf->writeLinetype(*dw,
//	//	DL_LinetypeData("ACAD_ISO04W100", 0));
//	//dxf->writeLinetype(*dw,
//	//	DL_LinetypeData("ACAD_ISO05W100", 0));
//	//dxf->writeLinetype(*dw, DL_LinetypeData("BORDER", 0));
//	//dxf->writeLinetype(*dw, DL_LinetypeData("BORDER2", 0));
//	//dxf->writeLinetype(*dw, DL_LinetypeData("BORDERX2", 0));
//	//dxf->writeLinetype(*dw, DL_LinetypeData("CENTER", 0));
//	//dxf->writeLinetype(*dw, DL_LinetypeData("CENTER2", 0));
//	//dxf->writeLinetype(*dw, DL_LinetypeData("CENTERX2", 0));
//	//dxf->writeLinetype(*dw, DL_LinetypeData("DASHDOT", 0));
//	//dxf->writeLinetype(*dw, DL_LinetypeData("DASHDOT2", 0));
//	//dxf->writeLinetype(*dw,
//	//	DL_LinetypeData("DASHDOTX2", 0));
//	//dxf->writeLinetype(*dw, DL_LinetypeData("DASHED", 0));
//	//dxf->writeLinetype(*dw, DL_LinetypeData("DASHED2", 0));
//	//dxf->writeLinetype(*dw, DL_LinetypeData("DASHEDX2", 0));
//	//dxf->writeLinetype(*dw, DL_LinetypeData("DIVIDE", 0));
//	//dxf->writeLinetype(*dw, DL_LinetypeData("DIVIDE2", 0));
//	//dxf->writeLinetype(*dw,
//	//	DL_LinetypeData("DIVIDEX2", 0));
//	//dxf->writeLinetype(*dw, DL_LinetypeData("DOT", 0));
//	//dxf->writeLinetype(*dw, DL_LinetypeData("DOT2", 0));
//	//dxf->writeLinetype(*dw, DL_LinetypeData("DOTX2", 0));
//	dw->tableEnd();
//
//
//	//....................
//	int numberOfLayers = 3;
//	dw->tableLayers(numberOfLayers);
//
//	dxf->writeLayer(*dw,
//		DL_LayerData("0", 0),
//		DL_Attributes(
//			std::string(""),      // leave empty
//			DL_Codes::black,        // default color
//			100,                  // default width
//			"CONTINUOUS", 1));       // default line style
//
//	dxf->writeLayer(*dw,
//		DL_LayerData("mainlayer", 0),
//		DL_Attributes(
//			std::string(""),
//			DL_Codes::red,
//			100,
//			"CONTINUOUS",1));
//
//	dxf->writeLayer(*dw,
//		DL_LayerData("anotherlayer", 0),
//		DL_Attributes(
//			std::string(""),
//			DL_Codes::black,
//			100,
//			"CONTINUOUS",1));
//
//	dw->tableEnd();
//
//	//....................
//	dxf->writeStyle(*dw, DL_StyleData("standard", 0, 2.5, 1.0, 0.0, 0, 2.5, "standard", ""));
//	dxf->writeView(*dw);
//	dxf->writeUcs(*dw);
//
//	dw->tableAppid(1);
//	dw->tableAppidEntry(0x12);
//	dw->dxfString(2, "ACAD");
//	dw->dxfInt(70, 0);
//	dw->tableEnd();
//
//	//....................
//	dxf->writeDimStyle(*dw,
//		1,
//		1,
//		1,
//		1,
//		1);
//	dxf->writeBlockRecord(*dw);
//	dxf->writeBlockRecord(*dw, "myblock1");
//	dxf->writeBlockRecord(*dw, "myblock2");
//	dw->tableEnd();
//	dw->sectionEnd();
//
//
//	//....................
//	dw->sectionBlocks();
//
//	dxf->writeBlock(*dw,
//		DL_BlockData("*Model_Space", 0, 0.0, 0.0, 0.0));
//	dxf->writeEndBlock(*dw, "*Model_Space");
//
//	dxf->writeBlock(*dw,
//		DL_BlockData("*Paper_Space", 0, 0.0, 0.0, 0.0));
//	dxf->writeEndBlock(*dw, "*Paper_Space");
//
//	//dxf->writeBlock(*dw,
//	//	DL_BlockData("*Paper_Space0", 0, 0.0, 0.0, 0.0));
//	//dxf->writeEndBlock(*dw, "*Paper_Space0");
//
//	dxf->writeBlock(*dw,
//		DL_BlockData("myblock1", 0, 0.0, 0.0, 0.0));
//	// ...
//	// write block entities e.g. with dxf->writeLine(), ..
//	// ...
//	dxf->writeEndBlock(*dw, "myblock1");
//
//	dxf->writeBlock(*dw,
//		DL_BlockData("myblock2", 0, 0.0, 0.0, 0.0));
//	// ...
//	// write block entities e.g. with dxf->writeLine(), ..
//	// ...
//	dxf->writeEndBlock(*dw, "myblock2");
//
//	dw->sectionEnd();
//
//
//	//....................
//	dw->sectionEntities();
//
//	// write all your entities..
//	dxf->writePoint(
//		*dw,
//		DL_PointData(10.0,
//			45.0,
//			0.0),
//		DL_Attributes("mainlayer", 256, -1, "BYLAYER",1));
//
//	dxf->writeLine(
//		*dw,
//		DL_LineData(25.0,   // start point
//			30.0,
//			0.0,
//			100.0,   // end point
//			120.0,
//			0.0),
//		DL_Attributes("mainlayer", 256, -1, "BYLAYER",2));
//
//	dw->sectionEnd();
//
//
//	//....................
//	dxf->writeObjects(*dw);
//	dxf->writeObjectsEnd(*dw);
//
//	dw->dxfEOF();
//	dw->close();
//
//	delete dw;
//	delete dxf;
//}
//
