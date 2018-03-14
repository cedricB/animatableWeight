//
// Copyright (C) cedric BAZILLOU
// 
// File: vertexBlendNode.cpp
//
// Dependency Graph Node: vertexBlend
//
// Author: Maya Plug-in Wizard 2.0
//

#include <string.h>
#include <maya/MIOStream.h>
#include <math.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MGlobal.h>

#include <maya/MPxNode.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MTypeId.h> 
#include <maya/MFnPlugin.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNurbsCurveData.h>
#include <maya/MFnNurbsSurfaceData.h>
#include <maya/MFnMeshData.h>
#include <maya/MFnData.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMesh.h>
#include <maya/MRampAttribute.h>
#include <maya/MPointArray.h>
#include <maya/MPoint.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MFloatArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MArrayDataBuilder.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnMeshData.h>




class vertexBlend : public MPxNode
{
public:
						vertexBlend();
	virtual				~vertexBlend(); 

	virtual MStatus		compute( const MPlug& plug, MDataBlock& data );

	static  void*		creator();
	static  MStatus		initialize();

public:

	static  MObject		inMesh;   
	static  MObject		targetMesh;   
	static  MObject		outMesh;  
	static  MObject		weightList;   

	
	static	MTypeId		id;

	bool vertexBlend::InputShape_Connected ( MObject & inputMeshObj );
};



MTypeId     vertexBlend::id( 0x454AA23 );
  
  
MObject     vertexBlend::inMesh; 
MObject     vertexBlend::targetMesh; 
MObject     vertexBlend::outMesh; 
MObject     vertexBlend::weightList;   



vertexBlend::vertexBlend() {}
vertexBlend::~vertexBlend() {}


bool vertexBlend::InputShape_Connected ( MObject & inputMeshObj )
{
	bool inputMeshConnected = false;

	if (inputMeshObj.isNull() == false )
	{
		inputMeshConnected = true;
	}
	return inputMeshConnected;

}

MStatus vertexBlend::compute( const MPlug& plug, MDataBlock& data )
{
	MStatus returnStatus;
	if(  plug == outMesh ) 
	{ 
		MDataHandle inMeshHandle		= data.inputValue(inMesh,&returnStatus);
		MDataHandle targetMeshHandle	= data.inputValue(targetMesh,&returnStatus);
		MDataHandle weightListHandle	= data.inputValue(weightList,&returnStatus);

		MDataHandle outMeshHandle		= data.outputValue(outMesh,&returnStatus);

		MObject inMeshObj		= inMeshHandle.data();
		MObject targetMeshObj	= targetMeshHandle.data();
		MObject weightListObj	= weightListHandle.data();

		bool check_inMesh		= InputShape_Connected(inMeshObj);
		bool check_targetMesh	= InputShape_Connected(targetMeshObj);	
		bool check_weightList	= InputShape_Connected(weightListObj);	

		MFnDoubleArrayData extractDoubleArrayFn;

		if ( check_inMesh == true &&check_targetMesh == true  && check_weightList == true  )
		{
			MDoubleArray vertexWeigths;
			vertexWeigths.clear();
			
			extractDoubleArrayFn.setObject(weightListObj);
			vertexWeigths = extractDoubleArrayFn.array();

			MFnMesh inMeshFn(inMeshObj);
			MFnMesh trgMeshFn(targetMeshObj);

			MPointArray inPnts_List;
			MPointArray trgPnts_List;

			MVectorArray ofssetlist;
				
			inMeshFn.getPoints( inPnts_List,MSpace::kObject ); 
			trgMeshFn.getPoints( trgPnts_List,MSpace::kObject ) ;

			unsigned int vecLength = inPnts_List.length();
			
			if ( vertexWeigths.length() != vecLength)
			{
					MGlobal::displayError("\n****************** InputMesh and vertexWeigths dont have the same number of elements..." );
					return MS::kUnknownParameter;
			} else {
				if ( trgPnts_List.length() != vecLength)
				{
					MGlobal::displayError("\n****************** InputMesh and target mesh dont have the same number of points..." );
					return MS::kUnknownParameter;
				} else {
					ofssetlist.setLength(vecLength);
					for ( unsigned k=0; k< vecLength;++k)
					{
						ofssetlist.set(trgPnts_List[k]-inPnts_List[k],k);
					}

					MFnMesh ouputMeshFn;
					MFnMeshData meshDataFn ;
					MObject newMeshObj = meshDataFn.create() ;	
					ouputMeshFn.copy( inMeshObj, newMeshObj ) ;

					MPointArray	trgPnts_ListB ;
					trgPnts_ListB.setLength(vecLength);	

					for ( unsigned k=0; k< vecLength;++k)
					{
						trgPnts_ListB.set( inPnts_List[k]+ofssetlist[k]*vertexWeigths[k] , k);
					}


					ouputMeshFn.setPoints( trgPnts_ListB, MSpace::kObject ) ;
					outMeshHandle.set(newMeshObj);
					data.setClean(plug);
				}
			}
		}

			
	} else {
		return MS::kUnknownParameter;
	}

	return MS::kSuccess;
}

void* vertexBlend::creator()
{
	return new vertexBlend();
}

MStatus vertexBlend::initialize()

{
	MFnNumericAttribute nAttr;
	MFnTypedAttribute typed_Attr;
	MFnEnumAttribute mode_Attr;

	MStatus				stat;

	inMesh = typed_Attr.create( "inMesh", "in", MFnMeshData::kMesh);
	typed_Attr.setStorable(0);
	typed_Attr.setKeyable(0);
	typed_Attr.setHidden(1);

	targetMesh = typed_Attr.create( "targetMesh", "trg", MFnMeshData::kMesh);
	typed_Attr.setStorable(0);
	typed_Attr.setKeyable(0);
	typed_Attr.setHidden(1);

	outMesh = typed_Attr.create( "outMesh", "ou", MFnMeshData::kMesh);
	typed_Attr.setStorable(0);
	typed_Attr.setKeyable(0);
	typed_Attr.setHidden(1);

	weightList = typed_Attr.create( "weightList", "wLs",MFnData::kDoubleArray );
	typed_Attr.setStorable(1);
	typed_Attr.setKeyable(0);
	typed_Attr.setHidden(1)	;


	addAttribute( weightList );
	addAttribute( inMesh  );
	addAttribute( targetMesh  );
	addAttribute( outMesh  );

	attributeAffects( weightList , outMesh );
	attributeAffects( inMesh ,outMesh );
	attributeAffects( targetMesh ,outMesh );

	return MS::kSuccess;

}

MStatus initializePlugin( MObject obj )
{ 
	MStatus   status;
	MFnPlugin plugin( obj, "cedric BAZILLOU", "0.1", "Any");

	status = plugin.registerNode( "vertexBlend", vertexBlend::id, vertexBlend::creator,
								  vertexBlend::initialize );
	if (!status) {
		status.perror("registerNode");
		return status;
	}

	return status;
}

MStatus uninitializePlugin( MObject obj)
{
	MStatus   status;
	MFnPlugin plugin( obj );

	status = plugin.deregisterNode( vertexBlend::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}

	return status;
}
