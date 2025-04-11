// ShapeTransfer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <TopoDS_Edge.h>
#include <TopoDS_Wire.h>

int main()
{
    std::cout << "Hello World!\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

Handle( TopTools_HSequenceOfShape ) CImportExport::ReadSTEPFile( const Handle( AIS_InteractiveContext )& anInteractiveContext )
{
	STEPControl_Reader reader;
	IFSelect_ReturnStatus status = reader.ReadFile( "D:\\360MoveData\\Users\\20102074\\Desktop\\1.stp" );
	Handle( TopTools_HSequenceOfShape ) aSequence = new TopTools_HSequenceOfShape();


	//	// 生成管道
	//	TopoDS_Edge line1 = BRepBuilderAPI_MakeEdge(gp_Pnt(0,0,0), gp_Pnt(10,0,0)).Edge();  // 直线段1
	//    //Handle(Geom_Circle) circle = GC_MakeCircle(gp_Ax2(gp_Pnt(10,5,0), gp_Dir(0,0,1)), 5.0);  // 圆弧
	//	 GC_MakeArcOfCircle arcMaker(gp_Pnt(10,0,0), gp_Pnt(15,5,0),gp_Pnt(10,10,0));
	//	 Handle(Geom_TrimmedCurve) arc1 = arcMaker.Value();
	//    TopoDS_Edge arc = BRepBuilderAPI_MakeEdge(arc1).Edge();  // 圆弧边
	//    TopoDS_Edge line2 = BRepBuilderAPI_MakeEdge(gp_Pnt(10,10,0), gp_Pnt(0,10,0)).Edge();  // 直线段2
	//
	//    // 组合路径
	//    TopoDS_Wire path = BRepBuilderAPI_MakeWire(line1, arc, line2).Wire();
	//
	//    // 创建截面（圆形）
	//    gp_Ax2 circleAxis(gp_Pnt(0,0,0), gp_Dir(1,0,0));
	//    gp_Circ circleGeom(circleAxis, 2.0);
	//	TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(circleGeom).Edge(); 
	//	BRepBuilderAPI_MakeWire mkWire; 
	//	mkWire.Add(edge);  
	//	mkWire.Build();
	//TopoDS_Wire wire = mkWire.Wire();  
	// 
	//// 创建面  
	//BRepBuilderAPI_MakeFace mkFace(wire);  
	//    TopoDS_Face circleFace = mkFace.Face();
	//
	//    // 生成管道
	//    BRepOffsetAPI_MakePipe pipe(path,circleFace );
	//    TopoDS_Shape pipeShape = pipe.Shape();
	//	STEPControl_Writer writer;
	//	writer.Transfer(pipeShape, STEPControl_AsIs);
	//	IFSelect_ReturnStatus writeStatus = writer.Write("D:\\360MoveData\\Users\\20102074\\Desktop\\newTube.stp");
	//
	//	aSequence->Append(pipeShape);
	// 
	//	return aSequence;


	if( status != IFSelect_RetDone ){
		std::cerr << "Error reading STEP file!" << std::endl;
	}

	// 导入所有实体
	reader.TransferRoots();
	TopoDS_Shape originalShape = reader.OneShape();

	// 修复
	// 初始化修复工具
	Handle( ShapeFix_Shape ) aFixShape = new ShapeFix_Shape();
	aFixShape->Init( originalShape ); // theShape为待修复的几何体
	// 设置精度和容差
	aFixShape->SetPrecision( 1e-3 );    // 基本精度
	aFixShape->SetMaxTolerance( 0.1 );  // 最大允许容差
	aFixShape->SetMinTolerance( 1e-3 ); // 最小允许边长
	// 执行修复
	aFixShape->Perform();
	// 获取修复后的形状
	TopoDS_Shape aResult = aFixShape->Shape();

	BRepBuilderAPI_Sewing sewer( true, true, 1e-3, 1e-3 );
	sewer.Add( aResult );
	sewer.Perform();
	TopoDS_Shape seweredResult = sewer.SewedShape();

	std::vector<TopoDS_Face> StandSurfaces;

	// 2. 遍历所有面并进行曲面替换
	BRep_Builder builder;
	TopTools_ListOfShape modifiedFaces;
	std::cerr << "开始遍历!" << std::endl;
	TopExp_Explorer faceExplorer( seweredResult, TopAbs_FACE );
	long allFace = 0;
	long BSplineFace = 0;
	for( ; faceExplorer.More(); faceExplorer.Next() ){
		TopoDS_Face face = TopoDS::Face( faceExplorer.Current() );
		Handle( Geom_Surface ) originalSurface = BRep_Tool::Surface( face );
		TopLoc_Location location;
		allFace++;
		std::cerr << "开始判断转换" + allFace << std::endl;

		BRepAdaptor_Surface FaceElementAdaptor1( face, true );
		GeomAbs_SurfaceType theTypeElement1 = FaceElementAdaptor1.GetType();

		// 检查是否为B样条曲面
		if( theTypeElement1 == GeomAbs_BSplineSurface ){
			BSplineFace++;
			Handle( Geom_Surface ) adaptedSurf = FaceElementAdaptor1.Surface().Surface();
			Handle( Geom_BSplineSurface ) bsplSurface = Handle( Geom_BSplineSurface )::DownCast( adaptedSurf );
			//Handle(Geom_BSplineSurface) bsplSurface = Handle(Geom_BSplineSurface)::DownCast(originalSurface);


			// 确保输入曲面有效
			if( bsplSurface.IsNull() ){
				std::cerr << "Error: Input B-spline surface is null!" << std::endl;
				return aSequence;
			}

			// 尝试转换为圆柱面



			std::cerr << "尝试转换为圆柱面!" << std::endl;
			GeomConvert_SurfToAnaSurf converter( bsplSurface );
			converter.SetConvType( GeomConvert_Target );
			converter.SetTarget( GeomAbs_Cylinder );
			Handle( Geom_Surface ) newSurface = converter.ConvertToAnalytical( 1e-2 );

			if( !newSurface.IsNull() && newSurface->DynamicType() == STANDARD_TYPE( Geom_CylindricalSurface ) ){
				//Handle(Geom_CylindricalSurface) cylinderSurface =new Geom_CylindricalSurface(newSurface);
				Handle( Geom_CylindricalSurface ) cylinderSurface = Handle( Geom_CylindricalSurface )::DownCast( newSurface );
				std::cerr << "是圆柱面!" << std::endl;
				gp_Cylinder cyl = cylinderSurface->Cylinder();
				gp_Ax1 axis = cyl.Axis();
				gp_Pnt location = axis.Location();  // 轴线起点（位置）
				gp_Dir direction = axis.Direction(); // 轴线方向

				std::cout << "圆柱轴线信息：" << std::endl;
				std::cout << "起点坐标: (" << location.X() << ", "
					<< location.Y() << ", " << location.Z() << ")" << std::endl;
				std::cout << "方向向量: (" << direction.X() << ", "
					<< direction.Y() << ", " << direction.Z() << ")" << std::endl;
				std::cout << "半径: " << cyl.Radius() << std::endl;
				gp_Dir axisDir = axis.Direction();
				gp_Pnt axisLoc = axis.Location();

				Standard_Real uMin = 0.0;
				Standard_Real uMax = 0;
				Standard_Real vMin;
				Standard_Real vMax;

				// 计算 V 方向范围
				cylinderSurface->Bounds( uMin, uMax, vMin, vMax );
				//std::vector<double> lengths = getBoundaryLengths(bsplSurface, direction);
				Standard_Integer firstUKnot = bsplSurface->FirstUKnotIndex();
				gp_Pnt p00 = bsplSurface->Value( 0, 0 );    // u=0, v=0
				gp_Pnt p10 = bsplSurface->Value( 1, 0 );    // u=1, v=0
				gp_Pnt p01 = bsplSurface->Value( 0, 1 );    // u=0, v=1
				gp_Pnt p11 = bsplSurface->Value( 1, 1 );    // u=1, v=1

				double dx = p01.X() - p00.X();
				double dy = p01.Y() - p00.Y();
				double dz = p01.Z() - p00.Z();
				double distance = sqrt( dx * dx + dy * dy + dz * dz );

				vMin = 0;
				vMax = distance;
				BRepBuilderAPI_MakeFace faceMaker( cyl, uMin, uMax, vMin, vMax );
				TopoDS_Face newCylinderSurface = faceMaker.Face();
				//TopoDS_Face newCylinderSurface = BRepBuilderAPI_MakeFace(cylinderSurface, 1.e-3).Face();
				//newCylinderSurface.Orientation(cylinderSurface.Orientation());
				//modifiedFaces.Append(newCylinderSurface);
				StandSurfaces.push_back( newCylinderSurface );
				continue;
			}

			std::cerr << "尝试转换为圆环面!" << std::endl;
			GeomConvert_SurfToAnaSurf converter1( bsplSurface );
			converter1.SetConvType( GeomConvert_Simplest );
			converter1.SetTarget( GeomAbs_Torus );
			Handle( Geom_Surface ) newSurface1 = converter1.ConvertToAnalytical( 1 );

			// 若失败则尝试转换为圆环面
			if( !newSurface1.IsNull() && newSurface1->DynamicType() == STANDARD_TYPE( Geom_ToroidalSurface ) ){
				Handle( Geom_ToroidalSurface ) toroidalSurface = Handle( Geom_ToroidalSurface )::DownCast( newSurface1 );
				std::cerr << "是圆环面!" << std::endl;

				gp_Torus tor = toroidalSurface->Torus();
				double tubeRadius = tor.MinorRadius();

				// 圆环面的U表示0~2pi，定义了圆环的范围
				Standard_Real uMin = 0.0;
				Standard_Real uMax = 2 * M_PI;
				// 圆环面的V是0到管半径π倍，定义了垂直方向的范围（这里固定是0~半径π倍）
				Standard_Real vMin = 0;
				Standard_Real vMax = 2 * M_PI;



				//BRepTools::UVBounds(face, uMin, uMax, vMin, vMax);
				//TopoDS_Wire outer_wire = ShapeAnalysis::OuterWire(face);

				// 检查面的完整性
				BRepCheck_Analyzer analyzer( face );
				if( !analyzer.IsValid() ){
					// 
					// ShapeFix_Face faceFix(face);

					//  //执行修复
					// faceFix.AutoCorrectPrecisionMode() = Standard_True; 
					//     faceFix.SetPrecision(1e-6);            // 设置全局几何精度
					//     faceFix.SetMinTolerance(1e-6);         // 设置最小公差（影响所有子工具）
					//     faceFix.SetMaxTolerance(1e-3);         // 设置最大公差（影响所有子工具）
					//    //faceFix.FixWireTool()->SetFixTolerance(1e-6); 
					//     faceFix.FixAddNaturalBoundMode() = Standard_True; // 启用自然边界添加
					//    // faceFix.FixPeriodicDegenerated() = Standard_True;  // 处理周期性退化
					//     faceFix.FixIntersectingWiresMode() = Standard_True; // 修复线交叉
					//     faceFix.FixSmallAreaWireMode() = Standard_True;     // 删除微小线
					//  // 获取修复后的面
					//Standard_Boolean result=  faceFix.Perform();

					//BRepBuilderAPI_Transform transform(faceFix.Result(), gp_Trsf()); // 重新构建拓扑
					//// TopoDS_Face face1 = transform.Shape();
					// TopoDS_Face face1 = faceFix.Face();
					//   

					// //ShapeFix_Shape shapeFix(face);
					// //shapeFix.Perform();
					// //// 获取修复后的几何形状
					// //TopoDS_Shape face1 = shapeFix.Shape(); 
					// BRepCheck_Analyzer fixedAnalyzer(face1);



					// 
					// if (fixedAnalyzer.IsValid()) {
					//     std::cout << "Face fixed successfully!" << std::endl;
					// } 
					// face = face1;
				}

				//std::vector<TopoDS_Wire> wires = GetAllWiresFromFace(face);
				//std::vector<TopoDS_Wire> wires;
				TopTools_HSequenceOfShape wire1 = ExtractFreeBoundary( face );
				//wires.push_back(wire1);
				//std::vector<Standard_Real> minArray;
				//std::vector<Standard_Real> maxArray;
			   // for (TopoDS_Wire& wire : wires) {
				GetToroidalSurfaceBounds( face, *toroidalSurface, wire1, uMin, uMax );
				//   minArray.push_back(uMin);
			   //    maxArray.push_back(uMax);
			   // }
			  // uMin = *std::min_element(minArray.begin(), minArray.end());
			  // uMax = *std::max_element(maxArray.begin(), maxArray.end());


			   //GetToroidalSurfaceBounds(face, *toroidalSurface, outer_wire, uMin, uMax);

				if( std::abs( uMin - uMax ) <= 0.001 || std::abs( uMin + 3.1415926 - uMax ) <= 0.001 ){
					// StandSurfaces.push_back(face);
					// continue;
				}

				BRepBuilderAPI_MakeFace faceMaker2( toroidalSurface, uMin, uMax, vMin, vMax, 1e-3 );

				TopoDS_Face newToroidalSurface = faceMaker2.Face();

				//TopoDS_Face partialTorus = CreatePartialTorusFace(200, 30, 0, M_PI, 0, 2 * M_PI);
				StandSurfaces.push_back( newToroidalSurface );
			}

			if( !newSurface.IsNull() ){
				// 应用原始面的位置变换
				//newSurface->Transform(location.Transformation());

				// 创建新面并保留原始面的方向
				/*
				TopoDS_Face newFace = BRepBuilderAPI_MakeFace(newSurface, 1.e-3).Face();
				newFace.Orientation(face.Orientation());
				modifiedFaces.Append(newFace);*/
				//modifiedFaces.Append(face); // 保留原始面
			}
			else{
				//modifiedFaces.Append(face); // 保留原始面
			}
		}
		else{
			//StandSurfaces.push_back(face);
			//modifiedFaces.Append(face); // 非样条面保留
		}
	}

	//TopoDS_Face partialTorus = CreatePartialTorusFace(200, 30, 0, M_PI, 0, 2 * M_PI);
	//StandSurfaces.push_back(partialTorus);

	//TopoDS_Face partialTorus2 = CreatePartialTorusFace(400, 30, 0, M_PI, 0, 2 * M_PI);
	//StandSurfaces.push_back(partialTorus2);


	TopoDS_Shape aShape;
	for( const auto& face : StandSurfaces ){
		//aShape = new AIS_Shape(face);
		Handle( AIS_Shape ) aisFace = new AIS_Shape( face );
		aSequence->Append( face );
		SetCursor( AfxGetApp()->LoadStandardCursor( IDC_ARROW ) );
	}

	// 组合成壳体
	BRep_Builder builder1;
	TopoDS_Shell shell;
	builder1.MakeShell( shell );

	for( const auto& face : StandSurfaces ){
		builder.Add( shell, face );
	}
	BRepBuilderAPI_MakeSolid solidMaker( shell );
	TopoDS_Solid solid = TopoDS::Solid( solidMaker.Shape() );

	// 生成管道
	TopoDS_Compound newShape;
	//BRep_Builder builder;
	//builder.MakeCompound(newShape);
	//for (const auto& face : StandSurfaces) {
	//	builder.Add(newShape, face);
	//}
	STEPControl_Writer writer;
	writer.Transfer( solid, STEPControl_AsIs );
	IFSelect_ReturnStatus writeStatus = writer.Write( "D:\\360MoveData\\Users\\20102074\\Desktop\\newTube2.stp" );

	//aSequence->Append(pipeShape);


	return aSequence;

}
