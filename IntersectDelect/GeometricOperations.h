#pragma once
class CGeometricOperations
{
public:
	CGeometricOperations();
	~CGeometricOperations();
public:
	int m_nOperType;//交并差-012
public:
	void SetBoolParam(int nOperType = 0);// 设置不同的处理方法
	void MathDBoolean(); // 交并差运算
	void MathDBooleanBat(); // 交并差运算
	void GeometryUnion();//并集
	void GeometryIntersection();//交集
	void GeometrySubtraction();//差集
	void GeometryUnionBat();//并集
	void GeometryIntersectionBat();//交集
	void GeometrySubtractionBat();//差集
	BOOL PolyLine2Region(const AcDbObjectIdArray idsPoly, AcDbObjectIdArray &idsRegion,int nu=2);//闭合多段线转面域
	BOOL PolyLine2Region(const AcDbObjectId idPoly, AcDbObjectIdArray &idsRegion);//闭合多段线转面域
	BOOL PolyLine2RegionBat(const AcDbObjectIdArray idsPoly, AcDbObjectIdArray &idsRegion);//闭合多段线转面域
	BOOL Region2PolyLine(const AcDbRegion* pRegion);//面域转闭合多段线
	BOOL Region2PolyLine(const AcDbRegion* pRegion,AcDbObjectIdArray &idsPoly);//面域转闭合多段线
	BOOL Region2PolyLine(const AcDbObjectIdArray idsRegion, AcDbObjectIdArray &idsPoly);//面域转闭合多段线
	AcDbObjectId PostToModelSpace(AcDbEntity *pEnt);//模型空间添加实体
	AcGePoint2d ToPoint2d(const AcGePoint3d pt);//三维点转二维点
	BOOL GetEnts(AcDbObjectIdArray &ids);//交互获取实体
	BOOL GetEnts(AcDbObjectId &id, AcDbObjectIdArray &ids);//交互获取实体(求差集)
	BOOL GetEntsBat(AcDbObjectId &id,AcDbObjectIdArray &ids);//交互获取实体
	BOOL GetEntsBat(AcDbObjectIdArray &ids);//交互获取实体
	BOOL IsCurveLenZero(const AcDbEntity *pEnt);//判断线长度是否为零
	BOOL IsSamePoint(const AcGePoint3dArray ptArr);//判断点数组是否都是同一个点
	BOOL DeleteEnts(const AcDbObjectIdArray ids);//删除实体
	BOOL ReMakePLine(const AcDbObjectIdArray ids, AcDbObjectIdArray &idsNew);//重新生成对象，去除多段线中线段为0的线段
	BOOL RegionIsBoolIntersect(const AcDbRegion * pRegion1, const AcDbRegion * pRegion2);//判断两个面域是否相交
	BOOL RegionIsIntersectWith(const AcDbRegion * pRegion1, const AcDbRegion * pRegion2);//判断两个面域是否有交点
	BOOL GetRegionUnionBat(AcDbObjectIdArray &idsRegion, AcDbObjectIdArray &idsRegionUnion);//从面域中提取合并后的面域
	BOOL GetRegionIntersectionBat(AcDbObjectIdArray &idsRegion, AcDbObjectIdArray &idsRegionIntersection);//从面域中提取合并后的面域
	BOOL GetRegionSubtractionBat(AcDbObjectId &idRegion, AcDbObjectIdArray &idsRegion, AcDbObjectIdArray &idsRegionIntersection);//从面域中提取差集后的面域
	
};

