//
// Copyright (C) cedric BAZILLOU
// 
// File: verticesMapperNode.cpp
//
// Dependency Graph Node: verticesMapper
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
 

class verticesMapper : public MPxNode
{
public:
						verticesMapper();
	virtual				~verticesMapper(); 

	virtual MStatus		compute( const MPlug& plug, MDataBlock& data );

	static  void*		creator();
	static  MStatus		initialize();

public:

	static  MObject		inputCurve;
	static  MObject		inputSurface;
	static  MObject		inMesh;
	static  MObject		weightList;
	static  MObject		weights;
	static  MObject		bindMode;
	static  MObject		weightMode;
	static  MObject		rampWeight;
	static  MObject		rampColor;	
	static  MObject		inputColor;	
	static  MObject		weightDriver;	

	static	MTypeId		id;

	bool verticesMapper::InputShape_Connected ( MObject & inputMeshObj );
	void verticesMapper::compute_spline_u_mapping( MFnNurbsCurve & incurveFn,  MPointArray & inPnts_List , MFloatArray & uvMap_List);
};



MTypeId     verticesMapper::id( 0x452AA21 );
  
MObject     verticesMapper::inputCurve;   
MObject     verticesMapper::inputSurface;   
MObject     verticesMapper::inMesh;   
MObject     verticesMapper::weightList;   
MObject     verticesMapper::weights;  
MObject     verticesMapper::bindMode;   
MObject     verticesMapper::weightMode;  
MObject     verticesMapper::rampWeight; 
MObject     verticesMapper::rampColor; 
MObject     verticesMapper::inputColor; 
MObject     verticesMapper::weightDriver; 


verticesMapper::verticesMapper() {}
verticesMapper::~verticesMapper() {}


bool verticesMapper::InputShape_Connected ( MObject & inputMeshObj )
{
	bool inputMeshConnected = false;

	if (inputMeshObj.isNull() == false )
	{
		inputMeshConnected = true;
	}
	return inputMeshConnected;

}

void verticesMapper::compute_spline_u_mapping( MFnNurbsCurve & incurveFn,  MPointArray & inPnts_List , MFloatArray & uvMap_List)
{
	double param = 0.0;
	MPoint clsPnt;
	
	double spanNum = incurveFn.numSpans()*1.0;
	for ( unsigned k=0; k<uvMap_List.length();++k)
	{
		clsPnt = incurveFn.closestPoint(inPnts_List[k],&param,0.00001,MSpace::kObject);
		uvMap_List.set(param/spanNum,k);
	}
}


MStatus verticesMapper::compute( const MPlug& plug, MDataBlock& data )
{
	MStatus returnStatus;
	if(  plug == weightList || plug.parent() == weights || plug == weights )
	{ 
		MDataHandle inputCurveHandle	= data.inputValue(inputCurve,&returnStatus);
		MDataHandle inputSurfaceHandle	= data.inputValue(inputSurface,&returnStatus);
		MDataHandle inMeshHandle		= data.inputValue(inMesh,&returnStatus);

		MDataHandle bindModeHandle		= data.inputValue(bindMode,&returnStatus);
		MDataHandle weightModeHandle	= data.inputValue(weightMode,&returnStatus);

		MDataHandle weightListHandle	= data.outputValue(weightList,&returnStatus);
		MArrayDataHandle weightsHandle	= data.outputArrayValue(weights,&returnStatus);		

		short bindMode_value	= bindModeHandle.asShort();
		short weightMode_value	= weightModeHandle.asShort();

		MObject inMeshObj		= inMeshHandle.asMesh();
		MObject inputCurveObj	= inputCurveHandle.asNurbsCurve();
		MObject inputSurfaceObj	= inputSurfaceHandle.asNurbsSurface();

		bool checkMeshInput		= InputShape_Connected(inMeshObj);
		bool checkcurveInput	= InputShape_Connected(inputCurveObj);
		bool checkSurfaceInput	= InputShape_Connected(inputSurfaceObj);


		cout << checkMeshInput << checkcurveInput << checkSurfaceInput;

		MObject thisNode = thisMObject();
		MRampAttribute rampWeightData(thisNode, rampWeight );
		MRampAttribute rampColorData(thisNode, rampColor );

		if ( checkMeshInput == false ) // Before computing data from the current binding method we at the minimum required that an input mesh flow in this node
		{ 
			return MS::kUnknownParameter;
		} else {
			MFnMesh inMeshFn(inMeshObj);
			MPointArray inPnts_List ;
			MDoubleArray vertexWeigths  ;
				
			inMeshFn.getPoints( inPnts_List, MSpace::kObject ); 
			unsigned int vecLength = inPnts_List.length();
			vertexWeigths.clear();
			vertexWeigths.setLength(vecLength);

			if ( bindMode_value == 0)// Curve bind Mode
			{
				if ( checkcurveInput == false )
				{
					return MS::kUnknownParameter;
				} else {
					MFnNurbsCurve incurveFn(inputCurveObj);
					MFloatArray uvMap_List;
					uvMap_List.clear();
					uvMap_List.setLength(vecLength);

					// in curve bind mode
					compute_spline_u_mapping( incurveFn, inPnts_List , uvMap_List);
					float rampVal = -1;
					for ( unsigned k=0; k<vecLength;++k)
					{
						rampWeightData.getValueAtPosition(uvMap_List[k], rampVal );
						vertexWeigths.set(rampVal,k);
					}
					MFnDoubleArrayData extractDoubleArrayFn;
					MObject DoubleArrayObj	= extractDoubleArrayFn.create(vertexWeigths)	;
					weightListHandle.set(DoubleArrayObj)	;

					weightListHandle.setClean();

					//cout << "\n" << vertexWeigths;

				}
			}
			if ( bindMode_value == 1)// surface bind Mode
			{
				if ( checkSurfaceInput == false )
				{
					return MS::kUnknownParameter;
				} else {
					///////////////////////
				}
			}
			if ( bindMode_value == 2)// UV bind Mode
			{
				///
			}
		}		
	} else {
		return MS::kUnknownParameter;
	}

	return MS::kSuccess;
}

void* verticesMapper::creator()
{
	return new verticesMapper();
}

MStatus verticesMapper::initialize()

{
	MFnNumericAttribute nAttr;
	MFnTypedAttribute typed_Attr;
	MFnEnumAttribute mode_Attr;	
	MStatus				stat;


	bindMode = mode_Attr.create( "bindMode", "bMd", 0  );
	mode_Attr.addField("curve",0);
	mode_Attr.addField("surface",1);	
	mode_Attr.addField("uv_map",2);
	mode_Attr.setStorable(1);
	mode_Attr.setKeyable(1);
	mode_Attr.setHidden(0);	
	addAttribute(bindMode);	

	weightMode = mode_Attr.create( "weightMode", "wMd", 0  );
	mode_Attr.addField("U_Propagation",0);
	mode_Attr.addField("UV_Space",1);
	mode_Attr.setStorable(1);
	mode_Attr.setKeyable(1);
	mode_Attr.setHidden(0);	
	addAttribute(weightMode);	

	weightDriver = mode_Attr.create( "weightDriver", "wDr", 0  );
	mode_Attr.addField("curveRamp",0);
	mode_Attr.addField("colorRamp",1);
	mode_Attr.addField("texture",2);
	mode_Attr.setStorable(1);
	mode_Attr.setKeyable(1);
	mode_Attr.setHidden(0);	
	addAttribute(weightDriver);	

	inputCurve = typed_Attr.create( "inputCurve", "inCrv", MFnNurbsCurveData::kNurbsCurve );
	typed_Attr.setStorable(0);
	typed_Attr.setKeyable(0);
	typed_Attr.setHidden(1);

	inputSurface = typed_Attr.create( "inputSurface", "inSrf", MFnNurbsSurfaceData::kNurbsSurface );
	typed_Attr.setStorable(0);
	typed_Attr.setKeyable(0);
	typed_Attr.setHidden(1);

	inMesh = typed_Attr.create( "inMesh", "in", MFnMeshData::kMesh);
	typed_Attr.setStorable(0);
	typed_Attr.setKeyable(0);
	typed_Attr.setHidden(1);

	MRampAttribute rmp_Attr ;
	rampWeight = rmp_Attr.createCurveRamp ("rampWeight", "rWg")	;
	rampColor = rmp_Attr.createColorRamp("rampColor", "rCl")	;

    inputColor = nAttr.createColor( "inputColor", "iCl" );
    nAttr.setDefault(0.0f, 1.0f, 0.0f);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);
    nAttr.setUsedAsColor(true);
    nAttr.setReadable(true);
    nAttr.setWritable(true);

	weightList = typed_Attr.create( "weightList", "wLs",MFnData::kDoubleArray );
	typed_Attr.setStorable(1);
	typed_Attr.setWritable(false);
	typed_Attr.setKeyable(0);
	typed_Attr.setHidden(1)	;

	weights = nAttr.create( "weights", "ws", MFnNumericData::kDouble, 0.0 );
	nAttr.setWritable(false);
	nAttr.setStorable(1);
	nAttr.setArray(false);
	nAttr.setUsesArrayDataBuilder(true);
	nAttr.setConnectable(true) ;
	nAttr.setHidden(1)	;

	stat = addAttribute( inputCurve );
		if (!stat) { stat.perror("fail to add inputCurve attribute"); return stat;}
	stat = addAttribute( inputSurface );
		if (!stat) { stat.perror("fail to add inputSurface attribute"); return stat;}
	stat = addAttribute( inMesh );
		if (!stat) { stat.perror("fail to add inMesh attribute"); return stat;}
	stat = addAttribute( rampWeight );
		if (!stat) { stat.perror("fail to add rampWeight attribute"); return stat;}
	stat = addAttribute( rampColor );
		if (!stat) { stat.perror("fail to add rampColor attribute"); return stat;}
	stat = addAttribute( inputColor );
		if (!stat) { stat.perror("fail to add inputColor attribute"); return stat;}


	stat = addAttribute( weightList );
		if (!stat) { stat.perror("fail to add weightList attribute"); return stat;}
	stat = addAttribute( weights );
		if (!stat) { stat.perror("fail to add weights attribute"); return stat;}


	attributeAffects( inputCurve , weightList );
	attributeAffects( inputSurface , weightList );
	attributeAffects( inMesh , weightList );

	attributeAffects( inputCurve ,  weights );
	attributeAffects( inputSurface ,  weights );
	attributeAffects( inMesh ,  weights );

	return MS::kSuccess;

}

MStatus initializePlugin( MObject obj )
{ 
	MStatus   status;
	MFnPlugin plugin( obj, "cedric BAZILLOU", "0.1", "Any");

	status = plugin.registerNode( "verticesMapper", verticesMapper::id, verticesMapper::creator,
								  verticesMapper::initialize );
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

	status = plugin.deregisterNode( verticesMapper::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}

	return status;
}
