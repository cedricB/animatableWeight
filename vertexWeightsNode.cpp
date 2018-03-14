//
// Copyright (C) cedric BAZILLOU
// 
// File: vertexWeightsNode.cpp
//
// Dependency Graph Node: vertexWeights
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
#include <maya/MColor.h>
#include <maya/MImage.h>
#include <maya/MFloatVector.h>	
#include <maya/MPlugArray.h>	

#include <maya/MFloatArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MRenderUtil.h>
#include <maya/MFloatMatrix.h>
#include <maya/MDynamicsUtil.h>




class vertexWeights : public MPxNode
{
public:
						vertexWeights();
	virtual				~vertexWeights(); 

	virtual MStatus		compute( const MPlug& plug, MDataBlock& data );

	static  void*		creator();
	static  MStatus		initialize();

public:

	static  MObject		U_List;   
	static  MObject		V_List;   
	static  MObject		rampWeight;   
	static  MObject		rampColor;   
	static  MObject		inputColor;   
	static  MObject		weightList;   
	//static  MObject		weights;   
	static  MObject		weightDriver;   
	//static  MObject		influence;   
	static  MObject		offset_U;   
	static  MObject		offset_V;   
	//static  MObject		writeArrayList;   
	
	
	static	MTypeId		id;

	bool vertexWeights::InputShape_Connected ( MObject & inputMeshObj );
	void vertexWeights::write_weight_datas( MArrayDataHandle & weightsHandle , MDoubleArray & vertexWeigths, MObject & thisNode);
	void vertexWeights::offset_and_clamp_data( double & inDouble , double & offsetValue );
	MStatus vertexWeights::sample_texture_from_dynUtil(MFnDoubleArrayData & extractDoubleArrayFn, MObject & U_ListObj,double & offset_U_value,
													   double & offset_V_value, MObject & thisNode, MObject & V_ListObj, MDoubleArray & vertexWeigths );
	void vertexWeights::sample_texture_from_renderUtil(MObject & U_ListObj,MObject & V_ListObj, MDoubleArray & vertexWeigths );
	void vertexWeights::sample_texture_by_hand(MObject & U_ListObj,MObject & V_ListObj, MDoubleArray & vertexWeigths );
};



MTypeId     vertexWeights::id( 0x453AA22 );
  
MObject     vertexWeights::U_List;   
MObject     vertexWeights::V_List;   
MObject     vertexWeights::rampWeight;   
MObject     vertexWeights::rampColor;   
MObject     vertexWeights::inputColor;   
MObject     vertexWeights::weightList;   
//MObject     vertexWeights::weights;   
MObject     vertexWeights::weightDriver;  
//MObject     vertexWeights::influence; 
MObject     vertexWeights::offset_U;  
MObject     vertexWeights::offset_V;  
//MObject     vertexWeights::writeArrayList;  


vertexWeights::vertexWeights() {}
vertexWeights::~vertexWeights() {}


bool vertexWeights::InputShape_Connected ( MObject & inputMeshObj )
{
	bool inputMeshConnected = false;

	if (inputMeshObj.isNull() == false )
	{
		inputMeshConnected = true;
	}
	return inputMeshConnected;

}

void vertexWeights::write_weight_datas( MArrayDataHandle & weightsHandle , MDoubleArray & vertexWeigths, MObject & thisNode)
{
		MArrayDataBuilder cBuilder = weightsHandle.builder();
		unsigned int bldSize = vertexWeigths.length();
		/*
		cBuilder.growArray ( bldSize );
		int elemIndex = 0;
		MDataHandle currentDH ;
		MDataHandle outDH ;
		for ( unsigned int n=0; n< bldSize ;++n)
		{
			currentDH = cBuilder.addElement(n);	
			
			MDataHandle wtChild = currentDH.child( weights) ;
			MArrayDataHandle hArrWts ( wtChild ) ;			
			MArrayDataBuilder cBuilderB = hArrWts.builder();

			outDH = cBuilderB.addElement(0);	
			outDH.set(vertexWeigths[n]);
			hArrWts.set(cBuilderB);
		}
		unsigned int bldChck = cBuilder.elementCount();
		if (  bldChck>bldSize)
		{
			for ( unsigned int n=bldChck-1; n<bldSize-1;--n)
			{
				cBuilder.removeElement(n);
			}
		}
		weightsHandle.set(cBuilder);
		weightsHandle.setAllClean();
		*/
}

void vertexWeights::offset_and_clamp_data( double & inDouble , double & offsetValue )
{
	inDouble += offsetValue;
	if ( inDouble > 1.0 )
	{
		inDouble = 1.0;
	}
	if ( inDouble < 0.0 )
	{
		inDouble = 0.0;
	}
}
void vertexWeights::sample_texture_by_hand(MObject & U_ListObj,MObject & V_ListObj, MDoubleArray & vertexWeigths )
{
/*
	MImage img;
	unsigned int width, height;

	MFnDependencyNode depFn( thisNode );
	MPlug imagePlug  = depFn.findPlug(inputColor);

	if ( imagePlug.isConnected() == true )
	{
		MPlugArray connectedPlugs;
		connectedPlugs.clear();
		imagePlug.connectedTo(connectedPlugs, true, false);

			if ( connectedPlugs.length() > 0 )
			{
				MObject textureNode = connectedPlugs[0].node();
				if ( ! textureNode.isNull() )
				{
					int x, y ;
					double valR;
					
					if (MS::kSuccess == img.readFromTextureNode(textureNode))
					{
						returnStatus = img.getSize( width, height);
						 if (width > 0 && height > 0 && (returnStatus != MStatus::kFailure) )
						 {
							unsigned int size = width * height;        
							unsigned char *pixels = img.pixels(); 
							unsigned char *pix;

							unsigned int depth  = img.depth(); 
							for ( unsigned k=0; k<vecLength;++k)
							{
								u_param = u_ListDt[k] ;
								offset_and_clamp_data( u_param  , offset_U_value);


								v_param = v_ListDt[k] ;
								offset_and_clamp_data( v_param  , offset_V_value);

								x = int(u_param * (width-1) );
								y = int(v_param * (height-1) );
								
								pix = pixels + (y * width * depth ) + x * depth ;
								
								if ( depth >=3 )
								{
									valR = 0.33 * *pix + 0.33 * *(pix+1) + 0.34 * *(pix+2);
									valR /=  255.0;
								} else {
									valR = *pix/255.0;
								}
								vertexWeigths.set(valR,k);
							}
						 }
					}						
				}
			}
	} 

	*/
}
void vertexWeights::sample_texture_from_renderUtil(MObject & U_ListObj,MObject & V_ListObj, MDoubleArray & vertexWeigths )
{
	/*
	MImage img;
	unsigned int width, height;

	MFnDependencyNode depFn( thisNode );
	MPlug imagePlug  = depFn.findPlug(inputColor);

	if ( imagePlug.isConnected() == true )
	{
		MPlugArray connectedPlugs;
		connectedPlugs.clear();
		imagePlug.connectedTo(connectedPlugs, true, false);

		if ( connectedPlugs.length() > 0 )
		{
			MObject textureNode = connectedPlugs[0].node();
			if ( ! textureNode.isNull() )
			{
				if ( sampleNetwork_value == false ) // David gould algorithm
				{
					// sample shading
					MFloatArray uCoords;
					MFloatArray vCoords;

					MFloatArray filterSizes;
					MFloatMatrix fakeCameraMatrix;

					uCoords.clear();
					vCoords.clear();
					//filterSizes.clear();

					uCoords.setLength(vecLength);
					vCoords.setLength(vecLength);
					//filterSizes.setLength(vecLength);

					MFloatVectorArray outputColors;
					MFloatVectorArray outputtransparency;

					for ( unsigned k=0; k<vecLength;++k)
					{
						u_param = u_ListDt[k] ;
						offset_and_clamp_data( u_param  , offset_U_value);
						v_param = v_ListDt[k] ;
						offset_and_clamp_data( v_param  , offset_V_value);

						uCoords.set(u_param,k);
						vCoords.set(v_param,k);
						//filterSizes.set(0.00001,k);
					}
					/////////////////////////////// Batch sample color value by lettting maya do the work /////////////////////

					

					if ( MS::kSuccess == MRenderUtil::sampleShadingNetwork 	( connectedPlugs[0].name(),	vecLength,false,false,fakeCameraMatrix,
															NULL,&uCoords,&vCoords,NULL,NULL,NULL,NULL,NULL,outputColors,outputtransparency	) )
					{
					
					//////////////////////////////////////////////////////////////////////////////////////////////////////
						if (outputColors.length() >= vecLength)
						{
							double clrVal;
							for ( unsigned k=0; k<vecLength;++k)
							{
								clrVal = (outputColors[k].x + outputColors[k].y+ outputColors[k].z )/3.0;								
								vertexWeigths.set(clrVal,k);
							}	
						}
					}
				}	
			}
		}
	} 

	*/
}
MStatus vertexWeights::sample_texture_from_dynUtil(MFnDoubleArrayData & extractDoubleArrayFn, MObject & U_ListObj,double & offset_U_value,
												   double & offset_V_value, MObject & thisNode, MObject & V_ListObj, MDoubleArray & vertexWeigths )
{			
	extractDoubleArrayFn.setObject(U_ListObj);
	MDoubleArray u_ListDt = extractDoubleArrayFn.array();

	extractDoubleArrayFn.setObject(V_ListObj);
	MDoubleArray v_ListDt = extractDoubleArrayFn.array();

	vertexWeigths.clear();
	unsigned int vecLength = u_ListDt.length();
	vertexWeigths.setLength(vecLength);
	
	bool hasTextureNode =	MDynamicsUtil::hasValidDynamics2dTexture( thisNode, inputColor );
	double u_param , v_param;

	if ( hasTextureNode )
	{
		MDoubleArray uCoords;
		MDoubleArray vCoords;
		uCoords.clear();
		vCoords.clear();

		uCoords.setLength(vecLength);
		vCoords.setLength(vecLength);

		MVectorArray Colors;
		MDoubleArray outputtrans;

		for ( unsigned k=0; k<vecLength;++k)
		{
			u_param = u_ListDt[k] ;
			offset_and_clamp_data( u_param  , offset_U_value);
			v_param = v_ListDt[k] ;
			offset_and_clamp_data( v_param  , offset_V_value);

			uCoords.set(u_param,k);
			vCoords.set(v_param,k);
		}

		MDynamicsUtil::evalDynamics2dTexture(thisNode,inputColor,uCoords,vCoords,&Colors,  &outputtrans);

		if (Colors.length() >= vecLength)
		{
			double clrVal;
			for ( unsigned k=0; k<vecLength;++k)
			{
				clrVal = (Colors[k].x + Colors[k].y + Colors[k].z )/3.0;								
				vertexWeigths.set(clrVal,k);
			}	
			return MStatus::kSuccess;
		} else {
			return MStatus::kFailure;
		}
	
	} else {
			return MStatus::kFailure;
	}
}

MStatus vertexWeights::compute( const MPlug& plug, MDataBlock& data )
{
	MStatus returnStatus;
	if(  plug == weightList )// || plug.parent() == weights || plug == weights )
	{ 
		MDataHandle U_ListHandle		= data.inputValue(U_List,&returnStatus);
		MDataHandle V_ListHandle		= data.inputValue(V_List,&returnStatus);
		MDataHandle inputColorHandle	= data.inputValue(inputColor,&returnStatus);
		MDataHandle weightDriverHandle	= data.inputValue(weightDriver,&returnStatus);
		MDataHandle offset_UHandle		= data.inputValue(offset_U,&returnStatus);
		MDataHandle offset_VHandle		= data.inputValue(offset_V,&returnStatus);
		//MDataHandle writeArrayList_Handle		= data.inputValue(writeArrayList,&returnStatus);		


		MDataHandle weightListHandle	= data.outputValue(weightList,&returnStatus);
		//MArrayDataHandle weightsHandle	= data.outputArrayValue(weights,&returnStatus);		

		short weightDriver_value		= weightDriverHandle.asShort();
		MFloatVector InputColorValue	= data.inputValue( inputColor ).asFloatVector();
		double offset_U_value			= offset_UHandle.asDouble();
		double offset_V_value			= offset_VHandle.asDouble();
		//bool writeArrayList_value		= writeArrayList_Handle.asBool();

		MObject U_ListObj		= U_ListHandle.data();
		MObject V_ListObj		= V_ListHandle.data();
		MObject inputColorObj	= inputColorHandle.data();

		bool checkU_List		= InputShape_Connected(U_ListObj);
		bool checkV_List		= InputShape_Connected(V_ListObj);


		MObject thisNode = thisMObject();
		MRampAttribute rampWeightData(thisNode, rampWeight );
		MRampAttribute rampColorData(thisNode, rampColor );
		MFnDoubleArrayData extractDoubleArrayFn;


		if ( checkU_List == true || checkV_List == true  )
		{
			MDoubleArray uvMap_List ;
			MDoubleArray vertexWeigths;

			uvMap_List.clear();
			vertexWeigths.clear();

			
			extractDoubleArrayFn.setObject(U_ListObj);

			uvMap_List = extractDoubleArrayFn.array();
			unsigned int vecLength = uvMap_List.length();
			vertexWeigths.setLength(vecLength);

			double u_param , v_param;
			v_param = 0.0;
			if ( weightDriver_value == 0)
			{
				float rampVal = -1;
				for ( unsigned k=0; k<vecLength;++k)
				{
					u_param = uvMap_List[k] ;
					offset_and_clamp_data( u_param  , offset_U_value);
					rampWeightData.getValueAtPosition(float(u_param), rampVal );
					vertexWeigths.set(rampVal,k);
				}

				MObject weightList_DoubleArrayObj	= extractDoubleArrayFn.create(vertexWeigths)	;
				weightListHandle.set(weightList_DoubleArrayObj)	;
				weightListHandle.setClean();
			} 
			if ( weightDriver_value == 1)
			{
				float colorDiv = 255.0;
				MColor rampColorValue;
				double clrVal;
				for ( unsigned k=0; k<vecLength;++k)
				{
					u_param = uvMap_List[k] ;
					offset_and_clamp_data( u_param  , offset_U_value);
					rampColorData.getColorAtPosition(float(u_param), rampColorValue );
					clrVal = (rampColorValue.r + rampColorValue.g+ rampColorValue.b )/3.0;
					
					vertexWeigths.set(clrVal,k);
				}

				MObject weightList_DoubleArrayObj	= extractDoubleArrayFn.create(vertexWeigths)	;
				weightListHandle.set(weightList_DoubleArrayObj)	;
				weightListHandle.setClean();

			}
			if ( weightDriver_value == 2)
			{
				MDoubleArray vertexWeigths;
				MStatus sampleStatus = sample_texture_from_dynUtil(extractDoubleArrayFn,U_ListObj,offset_U_value,offset_V_value,thisNode,V_ListObj,vertexWeigths );


				if ( sampleStatus == MS::kSuccess)
				{
					MObject weightList_DoubleArrayObj	= extractDoubleArrayFn.create(vertexWeigths)	;
					weightListHandle.set(weightList_DoubleArrayObj)	;
					weightListHandle.setClean();
				} else {
					vertexWeigths.clear();
					vertexWeigths.setLength(vecLength);
					MObject weightList_DoubleArrayObj	= extractDoubleArrayFn.create(vertexWeigths)	;
					weightListHandle.set(weightList_DoubleArrayObj)	;
					weightListHandle.setClean();
				}

			}

			/*
			if ( writeArrayList_value )
			{
				write_weight_datas(weightsHandle ,vertexWeigths, thisNode);
			}
			*/
		}

			
	} else {
		return MS::kUnknownParameter;
	}

	return MS::kSuccess;
}

void* vertexWeights::creator()
{
	return new vertexWeights();
}

MStatus vertexWeights::initialize()

{
	MFnNumericAttribute nAttr;
	MFnTypedAttribute typed_Attr;
	MFnEnumAttribute mode_Attr;
	MFnCompoundAttribute cAttr ;

	MStatus				stat;

	U_List = typed_Attr.create( "U_List", "uLs",MFnData::kDoubleArray );
	typed_Attr.setStorable(0);
	typed_Attr.setKeyable(0);
	typed_Attr.setHidden(1)	;

	V_List = typed_Attr.create( "V_List", "vLs",MFnData::kDoubleArray );
	typed_Attr.setStorable(0);
	typed_Attr.setKeyable(0);
	typed_Attr.setHidden(1)	;


	weightDriver = mode_Attr.create( "weightDriver", "wDr", 0  );
	mode_Attr.addField("curveRamp",0);
	mode_Attr.addField("colorRamp",1);
	mode_Attr.addField("texture",2);
	mode_Attr.setStorable(1);
	mode_Attr.setKeyable(1);
	mode_Attr.setHidden(0);	
	addAttribute(weightDriver);	


	MRampAttribute rmp_Attr ;
	rampWeight = rmp_Attr.createCurveRamp ("rampWeight", "rWg")	;
	rampColor = rmp_Attr.createColorRamp("rampColor", "rCl")	;

    inputColor = nAttr.createColor( "inputColor", "iCl" );
    nAttr.setDefault(0.0f, 1.0f, 0.0f);
    nAttr.setKeyable(false);
    nAttr.setStorable(true);
    nAttr.setUsedAsColor(true);

	offset_U = nAttr.create("offset_U","ofU", MFnNumericData::kDouble,0.0);
	nAttr.setWritable(1);
	nAttr.setStorable(1);
	nAttr.setReadable(1);
	nAttr.setKeyable(1);
	nAttr.setHidden(0)	;
	nAttr.setMin(-1.0)	;
	nAttr.setMax(1.0)	;
	addAttribute(offset_U )	;	

	offset_V = nAttr.create("offset_V","ofV", MFnNumericData::kDouble,0.0);
	nAttr.setWritable(1);
	nAttr.setStorable(1);
	nAttr.setReadable(1);
	nAttr.setKeyable(1);
	nAttr.setHidden(0)	;
	nAttr.setMin(-1.0)	;
	nAttr.setMax(1.0)	;
	addAttribute(offset_V )	;	




	weightList = typed_Attr.create( "weightList", "wLs",MFnData::kDoubleArray );
	typed_Attr.setStorable(1);
	typed_Attr.setWritable(false);
	typed_Attr.setKeyable(0);
	typed_Attr.setHidden(1)	;

	addAttribute( U_List );
	addAttribute( V_List );
	addAttribute( rampWeight );
	addAttribute( rampColor );
	addAttribute( inputColor );
	addAttribute( weightList );

	attributeAffects( U_List , weightList );
	attributeAffects( V_List , weightList );
	attributeAffects( rampWeight , weightList );
	attributeAffects( rampColor , weightList );
	attributeAffects( inputColor , weightList );
	attributeAffects( weightDriver , weightList );
	attributeAffects( offset_U , weightList );
	attributeAffects( offset_V , weightList );	


	/*
		writeArrayList = nAttr.create("writeArrayList","wAl", MFnNumericData::kBoolean,false);
		nAttr.setWritable(1);
		nAttr.setStorable(1);
		nAttr.setReadable(1);
		nAttr.setKeyable(1);
		nAttr.setHidden(0)	;
		addAttribute(writeArrayList )	;	

		weights = cAttr.create( "weights", "ws") ;
		cAttr.setWritable(false);
		cAttr.setStorable(1);
		cAttr.setArray(true);
		cAttr.setUsesArrayDataBuilder(true);
		//cAttr.setConnectable(true) ;
		nAttr.setHidden(1)	;

		influence = nAttr.create( "influence", "inf", MFnNumericData::kDouble, 0.0 );

		nAttr.setWritable(false);
		nAttr.setStorable(1);
		nAttr.setArray(true);	
		nAttr.setUsesArrayDataBuilder(true);
		nAttr.setHidden(1)	;
		cAttr.addChild( influence ) ;

		addAttribute( writeArrayList );
		addAttribute( weights );


		attributeAffects( U_List , weights );
		attributeAffects( V_List , weights );
		attributeAffects( rampWeight , weights );
		attributeAffects( rampColor , weights );
		attributeAffects( inputColor , weights );
		attributeAffects( weightDriver , weights );
		attributeAffects( offset_U , weights );
		attributeAffects( offset_V , weights );	
		attributeAffects( writeArrayList , weights );
	*/



	return MS::kSuccess;

}

MStatus initializePlugin( MObject obj )
{ 
	MStatus   status;
	MFnPlugin plugin( obj, "cedric BAZILLOU", "0.2", "Any");

	status = plugin.registerNode( "vertexWeights", vertexWeights::id, vertexWeights::creator,
								  vertexWeights::initialize );
	if (!status) {
		status.perror("registerNode");
		return status;
	}

	if (MGlobal::mayaState() == MGlobal::kInteractive )
	{
		MString command("string $uiprf =  `windowPref -exists \"cedricGreetingsUI\"` ;" ) ;
		command +=		"\n if ( $uiprf  == true)" ; 
		command += 		"\n {" ;
		command += 		"\n windowPref -r \"cedricGreetingsUI\";" ;
		command += 		"\n }" ;
		command += 		"\n string $uiExist =  `window  -exists \"cedricGreetingsUI\"` ;";
		command += 		"\n if ( $uiExist == true)";
		command += 		"\n {" ;
		command += 		"\n deleteUI(\"cedricGreetingsUI\");";
		command += 		"\n }" ;
		command += 		"\n window -t \"Greetings\" -s 0 \"cedricGreetingsUI\";" ;
		command += 		"\n frameLayout -borderVisible 0 -labelVisible 0;" ;
		command += 		"\n scrollField -w 400 -h 180 -bgc 0.0 0.2 0.3 -tx \"Author: cedric BAZILLOU \\nContact Mail:cedricbazillou@gmail.com\\nGreetings\\nI hope you will find this suite of plugins useful\\nyou are free to study , play or run with it\" -ed false;" ;
		command += 		"\n showWindow  \"cedricGreetingsUI\";" ;
		command += 		"\n window -edit -widthHeight 400 200  \"cedricGreetingsUI\";"  ;

		MGlobal::executeCommand ( command,false,false ) ;

	}

	return status;
}

MStatus uninitializePlugin( MObject obj)
{
	MStatus   status;
	MFnPlugin plugin( obj );

	status = plugin.deregisterNode( vertexWeights::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}

	return status;
}
