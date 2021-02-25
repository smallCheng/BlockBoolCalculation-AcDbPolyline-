#pragma once
class CGeometricOperations
{
public:
	CGeometricOperations();
	~CGeometricOperations();
public:
	int m_nOperType;//������-012
public:
	void SetBoolParam(int nOperType = 0);// ���ò�ͬ�Ĵ�����
	void MathDBoolean(); // ����������
	void MathDBooleanBat(); // ����������
	void GeometryUnion();//����
	void GeometryIntersection();//����
	void GeometrySubtraction();//�
	void GeometryUnionBat();//����
	void GeometryIntersectionBat();//����
	void GeometrySubtractionBat();//�
	BOOL PolyLine2Region(const AcDbObjectIdArray idsPoly, AcDbObjectIdArray &idsRegion,int nu=2);//�պ϶����ת����
	BOOL PolyLine2Region(const AcDbObjectId idPoly, AcDbObjectIdArray &idsRegion);//�պ϶����ת����
	BOOL PolyLine2RegionBat(const AcDbObjectIdArray idsPoly, AcDbObjectIdArray &idsRegion);//�պ϶����ת����
	BOOL Region2PolyLine(const AcDbRegion* pRegion);//����ת�պ϶����
	BOOL Region2PolyLine(const AcDbRegion* pRegion,AcDbObjectIdArray &idsPoly);//����ת�պ϶����
	BOOL Region2PolyLine(const AcDbObjectIdArray idsRegion, AcDbObjectIdArray &idsPoly);//����ת�պ϶����
	AcDbObjectId PostToModelSpace(AcDbEntity *pEnt);//ģ�Ϳռ����ʵ��
	AcGePoint2d ToPoint2d(const AcGePoint3d pt);//��ά��ת��ά��
	BOOL GetEnts(AcDbObjectIdArray &ids);//������ȡʵ��
	BOOL GetEnts(AcDbObjectId &id, AcDbObjectIdArray &ids);//������ȡʵ��(��)
	BOOL GetEntsBat(AcDbObjectId &id,AcDbObjectIdArray &ids);//������ȡʵ��
	BOOL GetEntsBat(AcDbObjectIdArray &ids);//������ȡʵ��
	BOOL IsCurveLenZero(const AcDbEntity *pEnt);//�ж��߳����Ƿ�Ϊ��
	BOOL IsSamePoint(const AcGePoint3dArray ptArr);//�жϵ������Ƿ���ͬһ����
	BOOL DeleteEnts(const AcDbObjectIdArray ids);//ɾ��ʵ��
	BOOL ReMakePLine(const AcDbObjectIdArray ids, AcDbObjectIdArray &idsNew);//�������ɶ���ȥ����������߶�Ϊ0���߶�
	BOOL RegionIsBoolIntersect(const AcDbRegion * pRegion1, const AcDbRegion * pRegion2);//�ж����������Ƿ��ཻ
	BOOL RegionIsIntersectWith(const AcDbRegion * pRegion1, const AcDbRegion * pRegion2);//�ж����������Ƿ��н���
	BOOL GetRegionUnionBat(AcDbObjectIdArray &idsRegion, AcDbObjectIdArray &idsRegionUnion);//����������ȡ�ϲ��������
	BOOL GetRegionIntersectionBat(AcDbObjectIdArray &idsRegion, AcDbObjectIdArray &idsRegionIntersection);//����������ȡ�ϲ��������
	BOOL GetRegionSubtractionBat(AcDbObjectId &idRegion, AcDbObjectIdArray &idsRegion, AcDbObjectIdArray &idsRegionIntersection);//����������ȡ��������
	
};

