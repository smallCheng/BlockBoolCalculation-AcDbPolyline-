#include "stdafx.h"
#include "GeometricOperations.h"
#include <brgbl.h>
#include <brbrep.h>
#include <brbetrav.h>
#include <bredge.h>
#include <brbftrav.h>
#include <brface.h>
#include <brletrav.h>
#include <brfltrav.h>
#include <brloop.h>
#include <brvtx.h>
#include <acedCmdNF.h>
#include "JoinToPline.h"


CGeometricOperations::CGeometricOperations()
{
	m_nOperType = -1;
}


CGeometricOperations::~CGeometricOperations()
{
}

void CGeometricOperations::SetBoolParam(int nOperType /*= 0*/)
{
	m_nOperType = nOperType;
}

void CGeometricOperations::MathDBoolean()
{
	//��ͬ�ķ�ʽ
	if (m_nOperType == 0)
	{
		acutPrintf(_T("[��ʼ]��\n"));
		GeometryIntersection();
	}
	else if (m_nOperType == 1)
	{
		acutPrintf(_T("[��ʼ]��\n"));
		GeometryUnion();
	}
	else if (m_nOperType == 2)
	{
		acutPrintf(_T("[��ʼ]���\n"));
		GeometrySubtraction();
	}
	else
	{
		acutPrintf(_T("[ʧ��]��Ч����ʽ\n"));
	}
}

void CGeometricOperations::MathDBooleanBat()
{
	//��ͬ�ķ�ʽ
	if (m_nOperType == 0)
	{
		acutPrintf(_T("[��ʼ]��\n"));
		GeometryIntersectionBat();
	}
	else if (m_nOperType == 1)
	{
		acutPrintf(_T("[��ʼ]��\n"));
		GeometryUnionBat();
	}
	else if (m_nOperType == 2)
	{
		acutPrintf(_T("[��ʼ]���\n"));
		GeometrySubtractionBat();
	}
	else
	{
		acutPrintf(_T("[ʧ��]��Ч����ʽ\n"));
	}
}

void CGeometricOperations::GeometryUnion()
{
	AcDbObjectIdArray idsPoly;
	AcDbObjectIdArray idsRegion;

	if (!GetEnts(idsPoly))
	{
		acutPrintf(L"\nѡ��ؿ�ʧ�ܣ�");
		return;
	}

	if (this->PolyLine2Region(idsPoly, idsRegion) == FALSE)
	{
		DeleteEnts(idsPoly);
		return;
	}

	DeleteEnts(idsPoly);

	//ʹ��ARX����ָ��д���������
	AcDbObjectPointer<AcDbRegion>pRegion1(idsRegion[0], AcDb::kForWrite);
	if (Acad::eOk != pRegion1.openStatus())
	{
		return;
	}
	AcDbObjectPointer<AcDbRegion>pRegion2(idsRegion[1], AcDb::kForWrite);
	if (Acad::eOk != pRegion2.openStatus())
	{
		return;
	}
	//ִ�в�������,���жϵؿ��Ƿ��н���������н����Ž��в�������
	if (!RegionIsBoolIntersect(pRegion1, pRegion2))
	{
		//���û�н����������ж��Ƿ��н��㣬������ߵؿ鲻�ܺϲ�����
		if (!RegionIsIntersectWith(pRegion1, pRegion2))
		{
			pRegion1->erase();
			pRegion2->erase();
			pRegion1->close();
			pRegion2->close();
			return;
		}
	}

	//ִ�в�������
	Acad::ErrorStatus es = pRegion1->booleanOper(AcDb::kBoolUnite, pRegion2);
	if (Acad::eOk != eOk)
	{
		acutPrintf(_T("\n��������ʧ��!,������:%s"), acadErrorStatusText(es));
		return;
	}
	//�ж��Ƿ��н��������Ϊ��,��ɾ������       
	if (pRegion1->isNull())
	{
		//acutPrintf(_T("\nû�в���,ɾ������!"));
		acutPrintf(_T("\nû�в���"));
		pRegion1->erase();
		pRegion2->erase();
		pRegion1->close();
		pRegion2->close();
		return;
	}
	//ɾ�����������ʵ�壬ִ�в�����������󣬲���������������Ϊ0��
	if (pRegion2->isNull())
	{
		pRegion2->erase();
	}

	//����ת�����
	//Region2PolyLine(pRegion1);
	AcDbObjectIdArray idsGetPoly;
	Region2PolyLine(pRegion1, idsGetPoly);

	//ɾ������
	pRegion1->erase();
	pRegion1->close();
}

void CGeometricOperations::GeometryIntersection()
{

	AcDbObjectIdArray idsPoly;
	AcDbObjectIdArray idsRegion;

	if (!GetEnts(idsPoly))
	{
		acutPrintf(L"\nѡ��ؿ�ʧ�ܣ�");
		return;
	}

	if (this->PolyLine2Region(idsPoly, idsRegion) == FALSE)
	{
		DeleteEnts(idsPoly);
		return;
	}
	DeleteEnts(idsPoly);

	//ʹ��ARX����ָ��д���������
	AcDbObjectPointer<AcDbRegion>pRegion1(idsRegion[0], AcDb::kForWrite);
	if (Acad::eOk != pRegion1.openStatus())
	{
		return;
	}
	AcDbObjectPointer<AcDbRegion>pRegion2(idsRegion[1], AcDb::kForWrite);
	if (Acad::eOk != pRegion2.openStatus())
	{
		return;
	}
	//ִ�в�������
	Acad::ErrorStatus es = pRegion1->booleanOper(AcDb::kBoolIntersect, pRegion2);
	if (Acad::eOk != eOk)
	{
		acutPrintf(_T("\n��������ʧ��!,������:%s"), acadErrorStatusText(es));
		return;
	}
	//�ж��Ƿ��н��������Ϊ��,��ɾ������       
	if (pRegion1->isNull())
	{
		acutPrintf(_T("\nû�н���,ɾ������!"));
		pRegion1->erase();
		pRegion2->erase();
		pRegion1->close();
		pRegion2->close();
		return;
	}
	//ɾ�����������ʵ�壬ִ�н�����������󣬲���������������Ϊ0��
	if (pRegion2->isNull())
	{
		pRegion2->erase();
	}

	//����ת�����
	//Region2PolyLine(pRegion1);
	AcDbObjectIdArray idsGetPoly;
	Region2PolyLine(pRegion1, idsGetPoly);

	//ɾ������
	pRegion1->erase();
	pRegion1->close();

}

void CGeometricOperations::GeometrySubtraction()
{
	AcDbObjectIdArray idsPoly;
	AcDbObjectId idPoly;
	AcDbObjectIdArray idsRegion;
	AcDbObjectIdArray idRegion;

	if (!GetEnts(idPoly, idsPoly))
	{
		acutPrintf(L"\nѡ��ؿ�ʧ�ܣ�");
		return;
	}

	AcDbObjectIdArray idsTemp;
	idsTemp.append(idPoly);
	if (this->PolyLine2Region(idsTemp[0], idRegion) == FALSE)
	{
		DeleteEnts(idsTemp);
		return;
	}
	DeleteEnts(idsTemp);

	if (this->PolyLine2Region(idsPoly[0], idsRegion) == FALSE)
	{
		DeleteEnts(idsPoly);
		return;
	}
	DeleteEnts(idsPoly);

	//ʹ��ARX����ָ��д���������
	AcDbObjectPointer<AcDbRegion>pRegion1(idRegion[0], AcDb::kForWrite);
	if (Acad::eOk != pRegion1.openStatus())
	{
		return;
	}
	AcDbObjectPointer<AcDbRegion>pRegion2(idsRegion[0], AcDb::kForWrite);
	if (Acad::eOk != pRegion2.openStatus())
	{
		return;
	}
	//����н������������������ǰ�����ϵ����ô�Ͳ����ɲ
	if ((!RegionIsIntersectWith(pRegion1, pRegion2)) && (RegionIsBoolIntersect(pRegion1, pRegion2)))
	{
		acutPrintf(_T("\nû�в,ɾ������!"));
		pRegion1->erase();
		pRegion2->erase();
		pRegion1->close();
		pRegion2->close();
		return;
	}
	//ִ�в�������
	Acad::ErrorStatus es = pRegion1->booleanOper(AcDb::kBoolSubtract, pRegion2);
	if (Acad::eOk != eOk)
	{
		acutPrintf(_T("\n��������ʧ��!,������:%s"), acadErrorStatusText(es));
		return;
	}
	//�ж��Ƿ��н��������Ϊ��,��ɾ������       
	if (pRegion1->isNull())
	{
		acutPrintf(_T("\nû�в,ɾ������!"));
		pRegion1->erase();
		pRegion2->erase();
		pRegion1->close();
		pRegion2->close();
		return;
	}
	//ɾ�����������ʵ�壬ִ�н�����������󣬲���������������Ϊ0��
	if (pRegion2->isNull())
	{
		pRegion2->erase();
	}

	//����ת�����
	//Region2PolyLine(pRegion1);
	AcDbObjectIdArray idsGetPoly;
	Region2PolyLine(pRegion1, idsGetPoly);

	//ɾ������
	pRegion1->erase();
	pRegion1->close();
}

void CGeometricOperations::GeometryUnionBat()
{
	AcDbObjectIdArray idsPoly;
	AcDbObjectIdArray idsRegion;

	if (!GetEntsBat(idsPoly))
	{
		acutPrintf(L"\nѡ��ؿ�ʧ�ܣ�");
		return;
	}

	if (this->PolyLine2RegionBat(idsPoly, idsRegion) == FALSE)
	{
		DeleteEnts(idsPoly);
		return;
	}
	DeleteEnts(idsPoly);

	AcDbObjectIdArray idsRegionUnion;
	//����������ȡ�ϲ�������
	GetRegionUnionBat(idsRegion, idsRegionUnion);

	//����ת�����
	AcDbObjectIdArray idsGetPoly;
	Region2PolyLine(idsRegionUnion, idsGetPoly);


	//ɾ������
	DeleteEnts(idsRegion);
	DeleteEnts(idsRegionUnion);

}

void CGeometricOperations::GeometryIntersectionBat()
{
	AcDbObjectIdArray idsPoly;
	AcDbObjectIdArray idsRegion;

	if (!GetEntsBat(idsPoly))
	{
		acutPrintf(L"\nѡ��ؿ�ʧ�ܣ�");
		return;
	}

	if (this->PolyLine2RegionBat(idsPoly, idsRegion) == FALSE)
	{
		DeleteEnts(idsPoly);
		return;
	}
	DeleteEnts(idsPoly);

	AcDbObjectIdArray idsRegionIntersection;
	//����������ȡ�ϲ�������
	if (!GetRegionIntersectionBat(idsRegion, idsRegionIntersection))
	{
		//ɾ������
		DeleteEnts(idsRegion);
		DeleteEnts(idsRegionIntersection);
		return;
	}

	//����ת�����
	AcDbObjectIdArray idsGetPoly;
	Region2PolyLine(idsRegionIntersection, idsGetPoly);

	//ɾ������
	DeleteEnts(idsRegion);
	DeleteEnts(idsRegionIntersection);

}

void CGeometricOperations::GeometrySubtractionBat()
{
	AcDbObjectIdArray idsPoly;
	AcDbObjectId idPoly;
	AcDbObjectIdArray idsRegion;
	AcDbObjectIdArray idRegion;

	if (!GetEntsBat(idPoly, idsPoly))
	{
		acutPrintf(L"\nѡ��ؿ�ʧ�ܣ�");
		return;
	}

	AcDbObjectIdArray idsTemp;
	idsTemp.append(idPoly);
	if (this->PolyLine2Region(idsTemp[0], idRegion) == FALSE)
	{
		DeleteEnts(idsTemp);
		return;
	}
	DeleteEnts(idsTemp);

	if (this->PolyLine2Region(idsPoly, idsRegion,1) == FALSE)
	{
		DeleteEnts(idsPoly);
		return;
	}
	DeleteEnts(idsPoly);

	AcDbObjectIdArray idRegionSubtraction;
	//����������ȡ���۳��������
	if (!GetRegionSubtractionBat(idRegion.at(0), idsRegion, idRegionSubtraction))
	{
		//ɾ������
		DeleteEnts(idRegion);
		DeleteEnts(idsRegion);
		DeleteEnts(idRegionSubtraction);
		return;
	}

	//����ת�����
	AcDbObjectIdArray idsGetPoly;
	Region2PolyLine(idRegionSubtraction, idsGetPoly);

	//ɾ������
	DeleteEnts(idRegion);
	DeleteEnts(idsRegion);
	DeleteEnts(idRegionSubtraction);

}

BOOL CGeometricOperations::PolyLine2Region(const AcDbObjectIdArray idsPoly, AcDbObjectIdArray &idsRegion,int nu)
{
	if (idsPoly == NULL)
	{
		return FALSE;
	}
	if (idsPoly.length() < 1)
	{
		return FALSE;
	}
	if (idsPoly.length() <nu)
	{
		return FALSE;
	}

	AcDbObjectIdArray regionIds;	//���ɵ�����ID����
	AcDbVoidPtrArray curves;		//ָ����Ϊ����߽�����ߵ�ָ������
	AcDbVoidPtrArray regions;		//ָ�򴴽�����������ָ������
	AcDbEntity *pEnt = NULL;		//��ʱָ�룬�����رձ߽�����
	AcDbRegion *pRegion = NULL;		//��ʱ����������������ӵ�ģ�Ϳռ�

	//��idsPoly��ʼ��cures
	for (int i = 0; i < idsPoly.length(); i++)
	{
		acdbOpenAcDbEntity(pEnt, idsPoly.at(i), AcDb::kForRead);
		if (pEnt->isKindOf(AcDbCurve::desc()))
		{
			curves.append(static_cast<void*>(pEnt));
		}
		else
		{
			pEnt->close();
		}
	}

	Acad::ErrorStatus es = AcDbRegion::createFromCurves(curves, regions);
	if (es == Acad::eOk)
	{
		//�����ɵ�������ӵ�ģ�Ϳռ�
		for (int i = 0; i < regions.length(); i++)
		{
			//����ָ��ת��Ϊָ�������ָ��
			pRegion = static_cast<AcDbRegion *>(regions[i]);
			pRegion->setDatabaseDefaults();
			AcDbObjectId regionId;
			regionId = PostToModelSpace(pRegion);
			pRegion->close();
			regionIds.append(regionId);
		}
	}
	else
	{
		for (int i = 0; i < regions.length(); i++)
		{
			delete (AcRxObject*)regions[i];
		}
	}

	//�ر���Ϊ�߽��ߵĶ���
	for (int i = 0; i < curves.length(); i++)
	{
		pEnt = static_cast<AcDbEntity*>(curves[i]);
		pEnt->close();
	}

	idsRegion = regionIds;

	return TRUE;
}

BOOL CGeometricOperations::PolyLine2Region(const AcDbObjectId idPoly, AcDbObjectIdArray & idsRegion)
{
	if (idPoly == NULL)
	{
		return FALSE;
	}

	AcDbObjectIdArray regionIds;	//���ɵ�����ID����
	AcDbVoidPtrArray curves;		//ָ����Ϊ����߽�����ߵ�ָ������
	AcDbVoidPtrArray regions;		//ָ�򴴽�����������ָ������
	AcDbEntity *pEnt = NULL;		//��ʱָ�룬�����رձ߽�����
	AcDbRegion *pRegion = NULL;		//��ʱ����������������ӵ�ģ�Ϳռ�

									//��idsPoly��ʼ��cures

	acdbOpenAcDbEntity(pEnt, idPoly, AcDb::kForRead);
	if (pEnt->isKindOf(AcDbCurve::desc()))
	{
		curves.append(static_cast<void*>(pEnt));
	}
	else
	{
		pEnt->close();
	}


	Acad::ErrorStatus es = AcDbRegion::createFromCurves(curves, regions);
	if (es == Acad::eOk)
	{
		//�����ɵ�������ӵ�ģ�Ϳռ�
		for (int i = 0; i < regions.length(); i++)
		{
			//����ָ��ת��Ϊָ�������ָ��
			pRegion = static_cast<AcDbRegion *>(regions[i]);
			pRegion->setDatabaseDefaults();
			AcDbObjectId regionId;
			regionId = PostToModelSpace(pRegion);
			pRegion->close();
			regionIds.append(regionId);
		}
	}
	else
	{
		for (int i = 0; i < regions.length(); i++)
		{
			delete (AcRxObject*)regions[i];
		}
	}

	//�ر���Ϊ�߽��ߵĶ���
	for (int i = 0; i < curves.length(); i++)
	{
		pEnt = static_cast<AcDbEntity*>(curves[i]);
		pEnt->close();
	}

	idsRegion = regionIds;

	return TRUE;
}

BOOL CGeometricOperations::PolyLine2RegionBat(const AcDbObjectIdArray idsPoly, AcDbObjectIdArray &idsRegion)
{
	if (idsPoly == NULL)
	{
		return FALSE;
	}
	if (idsPoly.length() < 2)
	{
		return FALSE;
	}


	AcDbObjectIdArray regionIds;	//���ɵ�����ID����
	AcDbVoidPtrArray curves;		//ָ����Ϊ����߽�����ߵ�ָ������
	AcDbVoidPtrArray regions;		//ָ�򴴽�����������ָ������
	AcDbEntity *pEnt = NULL;		//��ʱָ�룬�����رձ߽�����
	AcDbRegion *pRegion = NULL;		//��ʱ����������������ӵ�ģ�Ϳռ�

									//��idsPoly��ʼ��cures
	for (int i = 0; i < idsPoly.length(); i++)
	{
		acdbOpenAcDbEntity(pEnt, idsPoly.at(i), AcDb::kForRead);
		if (pEnt->isKindOf(AcDbCurve::desc()))
		{
			curves.append(static_cast<void*>(pEnt));
		}
		else
		{
			pEnt->close();
		}
	}

	Acad::ErrorStatus es = AcDbRegion::createFromCurves(curves, regions);
	if (es == Acad::eOk)
	{
		//�����ɵ�������ӵ�ģ�Ϳռ�
		for (int i = 0; i < regions.length(); i++)
		{
			//����ָ��ת��Ϊָ�������ָ��
			pRegion = static_cast<AcDbRegion *>(regions[i]);
			pRegion->setDatabaseDefaults();
			AcDbObjectId regionId;
			regionId = PostToModelSpace(pRegion);
			pRegion->close();
			regionIds.append(regionId);
		}
	}
	else
	{
		for (int i = 0; i < regions.length(); i++)
		{
			delete (AcRxObject*)regions[i];
		}
	}

	//�ر���Ϊ�߽��ߵĶ���
	for (int i = 0; i < curves.length(); i++)
	{
		pEnt = static_cast<AcDbEntity*>(curves[i]);
		pEnt->close();
	}

	idsRegion = regionIds;

	return TRUE;
}

BOOL CGeometricOperations::Region2PolyLine(const AcDbRegion* pRegion)
{

	//�ֽ�����
	Acad::ErrorStatus es;
	AcDbVoidPtrArray ptrArray;
	es = pRegion->explode(ptrArray);
	if (Acad::eOk != es)
	{
		acutPrintf(_T("\n����ֽ����ʧ��!,������:%s"), acadErrorStatusText(es));
		return FALSE;
	}

	AcDbObjectIdArray idsTemp;

	//������ӷֽ���ʵ�嵽��ǰ�ռ�,������������ݿ����Ҫ�Լ�delete�ֽ��Ķ���
	for (int i = 0; i < ptrArray.length(); i++)
	{
		AcDbEntity*pEnt = (AcDbEntity*)ptrArray.at(i);
		if (pEnt != NULL)
		{
			if (pEnt->isKindOf(AcDbRegion::desc()))
			{
				AcDbVoidPtrArray subptrArray;
				es = pEnt->explode(subptrArray);
				if (Acad::eOk != es)
				{
					acutPrintf(_T("\n������ֽ����ʧ��!,������:%s"), acadErrorStatusText(es));
					continue;
				}
				for (int j = 0; j < subptrArray.length(); j++)
				{
					AcDbEntity*pSubEnt = (AcDbEntity*)subptrArray.at(j);
					if (pSubEnt != NULL)
					{
						if (!IsCurveLenZero(pSubEnt))
						{
							pSubEnt->setColorIndex(1);
							idsTemp.append(PostToModelSpace(pSubEnt));
						}
						//�ر�pSubEnt����
						pSubEnt->close();
					}
				}
			}
			else
			{
				if (!IsCurveLenZero(pEnt))
				{
					pEnt->setColorIndex(1);
					idsTemp.append(PostToModelSpace(pEnt));
				}
			}
			//�ر�pEnt����
			pEnt->close();
		}
	}

	ads_name result;
	acedSSAdd(NULL, NULL, result);
	for (int i = 0; i < idsTemp.length(); i++)
	{
		ads_name entName;
		acdbGetAdsName(entName, idsTemp.at(i));
		acedSSAdd(entName, result, result);
		acedSSFree(entName);
	}

	acedCommandS(RTSTR, _T("PEDIT"),
		RTSTR, _T("M"),
		RTPICKS, result,
		RTSTR, _T(""),
		RTSTR, _T("Y"),
		RTSTR, _T("J"),
		RTSTR, _T("0"),//��ʾģ������
		RTSTR, _T("C"),
		RTSTR, _T(""),
		RTNONE);

	acedSSFree(result);

	return TRUE;
}

BOOL CGeometricOperations::Region2PolyLine(const AcDbRegion* pRegion, AcDbObjectIdArray &idsPoly)
{
	//�ֽ�����
	Acad::ErrorStatus es;
	AcDbVoidPtrArray ptrArray;
	es = pRegion->explode(ptrArray);
	if (Acad::eOk != es)
	{
		acutPrintf(_T("\n����ֽ����ʧ��!,������:%s"), acadErrorStatusText(es));
		return FALSE;
	}

	AcDbObjectIdArray idsTemp;

	//������ӷֽ���ʵ�嵽��ǰ�ռ�,������������ݿ����Ҫ�Լ�delete�ֽ��Ķ���
	for (int i = 0; i < ptrArray.length(); i++)
	{
		AcDbEntity*pEnt = (AcDbEntity*)ptrArray.at(i);
		if (pEnt != NULL)
		{
			if (pEnt->isKindOf(AcDbRegion::desc()))
			{
				AcDbVoidPtrArray subptrArray;
				es = pEnt->explode(subptrArray);
				if (Acad::eOk != es)
				{
					acutPrintf(_T("\n������ֽ����ʧ��!,������:%s"), acadErrorStatusText(es));
					continue;
				}
				for (int j = 0; j < subptrArray.length(); j++)
				{
					AcDbEntity*pSubEnt = (AcDbEntity*)subptrArray.at(j);
					if (pSubEnt != NULL)
					{
						if (!IsCurveLenZero(pSubEnt))
						{
							pSubEnt->setColorIndex(1);
							idsTemp.append(PostToModelSpace(pSubEnt));
						}
						//�ر�pSubEnt����
						pSubEnt->close();
					}
				}
			}
			else
			{
				if (!IsCurveLenZero(pEnt))
				{
					pEnt->setColorIndex(1);
					idsTemp.append(PostToModelSpace(pEnt));
				}
			}
			//�ر�pEnt����
			pEnt->close();
		}
	}

	//��¼id����ɾ��ʵ��ʹ��
	AcDbObjectIdArray idsTemp1 = idsTemp;

	while (idsTemp.length() > 0)
	{
		AcDbEntity *p = CJoinToPline::MakeJonedPoly(idsTemp);
		if (p)
		{
			if (AcDbPolyline::cast(p))
			{
				AcDbPolyline::cast(p)->setClosed(true);
				p->setColorIndex(1);
			}
			idsPoly.append(PostToModelSpace(p));
			p->close();
		}
		else
		{
			acutPrintf(_T("\nError in data!"));
			return FALSE;
		}
	}

	//ɾ����ɢ���ʵ��
	DeleteEnts(idsTemp1);

	if (idsPoly.length() <= 0)
		return FALSE;
	return TRUE;
}

BOOL CGeometricOperations::Region2PolyLine(const AcDbObjectIdArray idsRegion, AcDbObjectIdArray & idsPoly)
{
	for (int i = 0; i < idsRegion.length(); i++)
	{
		AcDbObjectPointer<AcDbRegion> pRegion(idsRegion[i], AcDb::kForRead);
		if (pRegion.openStatus() != Acad::eOk)
		{
			continue;
		}
		AcDbObjectIdArray ids;
		Region2PolyLine(pRegion, ids);
		if (ids.length() > 0)
		{
			idsPoly.append(ids);
		}
	}
	return TRUE;
}

AcDbObjectId CGeometricOperations::PostToModelSpace(AcDbEntity *pEnt)
{
	//�򿪵�ǰ���ݿ�ĵ�ǰ����¼
	AcDbBlockTableRecordPointer pBlkRcd(curDoc()->database()->currentSpaceId(), AcDb::kForWrite);
	if (Acad::eOk != pBlkRcd.openStatus())
	{
		acutPrintf(_T("\n�򿪿���¼ʧ��!,������:%s"), acadErrorStatusText(pBlkRcd.openStatus()));
		return NULL;
	}
	AcDbObjectId id;
	pBlkRcd->appendAcDbEntity(id, pEnt);
	pBlkRcd->close();
	return id;
}


AcGePoint2d CGeometricOperations::ToPoint2d(const AcGePoint3d pt)
{
	return AcGePoint2d(pt.x, pt.y);
}

BOOL CGeometricOperations::GetEnts(AcDbObjectIdArray &ids)
{
	ads_name ss;
	struct resbuf *rb = NULL;
	acutBuildList(RTDXF0, _T("PLINE"), RTNONE);
	CString strTile = L"";
	if (m_nOperType == 0)
	{
		strTile.Format(L"\n��ѡ��������Ҫ��������(%s)�ĵؿ�: ", L"����");
	}
	else if (m_nOperType == 1)
	{
		strTile.Format(L"\n��ѡ��������Ҫ��������(%s)�ĵؿ�: ", L"����");
	}
	else if (m_nOperType == 2)
	{
		strTile.Format(L"\n��ѡ��������Ҫ��������(%s)�ĵؿ�: ", L"�");
	}
	else
	{
		return FALSE;
	}
	acutPrintf(strTile);
	if (RTNORM != acedSSGet(NULL, NULL, NULL, rb, ss))
	{
		acutRelRb(rb);
		return FALSE;
	}
	acutRelRb(rb);
	Adesk::Int32 nssLen = 0;
	acedSSLength(ss, &nssLen);
	if (nssLen < 2)
	{
		acedSSFree(ss);
		return FALSE;
	}
	ads_name ent;
	acedSSName(ss, 0, ent);
	AcDbObjectId objId1;
	acdbGetObjectId(objId1, ent);
	acedSSFree(ent);
	acedSSName(ss, 1, ent);
	AcDbObjectId objId2;
	acdbGetObjectId(objId2, ent);
	acedSSFree(ent);

	acedSSFree(ss);

	ids.append(objId1);
	ids.append(objId2);

	AcDbEntity *pEnt = NULL;
	AcDbPolyline *pPline = NULL;
	if (acdbOpenAcDbEntity(pEnt, objId1, AcDb::kForRead) != Acad::eOk)
		return FALSE;
	pPline = AcDbPolyline::cast(pEnt);
	if (pPline == NULL)
	{
		pEnt->close();
		return FALSE;
	}
	if (!pPline->isClosed())
	{
		pEnt->close();
		return FALSE;
	}
	pEnt->close();
	pEnt = NULL;
	pPline = NULL;

	if (acdbOpenAcDbEntity(pEnt, objId2, AcDb::kForRead) != Acad::eOk)
		return FALSE;
	pPline = AcDbPolyline::cast(pEnt);
	if (pPline == NULL)
	{
		pEnt->close();
		return FALSE;
	}
	if (!pPline->isClosed())
	{
		pEnt->close();
		return FALSE;
	}
	pEnt->close();
	pEnt = NULL;
	pPline = NULL;

	//���������
	AcDbObjectIdArray idsTemp;
	ReMakePLine(ids, idsTemp);
	ids.removeAll();
	ids = idsTemp;

	return TRUE;
}

BOOL CGeometricOperations::GetEnts(AcDbObjectId &id, AcDbObjectIdArray & ids)
{


	CString strTile = L"";
	if (m_nOperType == 0)
	{
		strTile.Format(L"\n��ѡ��������Ҫ��������(%s)�ĵؿ�: ", L"����");
	}
	else if (m_nOperType == 1)
	{
		strTile.Format(L"\n��ѡ��������Ҫ��������(%s)�ĵؿ�: ", L"����");
	}
	else if (m_nOperType == 2)
	{
		strTile.Format(L"\n��ѡ��������Ҫ��������(%s)�ĵؿ�: ", L"�");
	}
	else
	{
		return FALSE;
	}
	acutPrintf(strTile);
	ads_name name1, name2;
	ads_point pt1, pt2;
	acedEntSel(L"\nѡ��Ҫ���۳���ʵ�壺", name1, pt1);
	acedEntSel(L"\nѡ����Ҫ�۳���ʵ�壺", name2, pt2);


	AcDbObjectId objId1;
	acdbGetObjectId(objId1, name1);
	acedSSFree(name1);

	AcDbObjectId objId2;
	acdbGetObjectId(objId2, name2);
	acedSSFree(name2);


	ids.append(objId1);
	ids.append(objId2);

	AcDbEntity *pEnt = NULL;
	AcDbPolyline *pPline = NULL;
	if (acdbOpenAcDbEntity(pEnt, objId1, AcDb::kForRead) != Acad::eOk)
		return FALSE;
	pPline = AcDbPolyline::cast(pEnt);
	if (pPline == NULL)
	{
		pEnt->close();
		return FALSE;
	}
	if (!pPline->isClosed())
	{
		pEnt->close();
		return FALSE;
	}
	pEnt->close();
	pEnt = NULL;
	pPline = NULL;

	if (acdbOpenAcDbEntity(pEnt, objId2, AcDb::kForRead) != Acad::eOk)
		return FALSE;
	pPline = AcDbPolyline::cast(pEnt);
	if (pPline == NULL)
	{
		pEnt->close();
		return FALSE;
	}
	if (!pPline->isClosed())
	{
		pEnt->close();
		return FALSE;
	}
	pEnt->close();
	pEnt = NULL;
	pPline = NULL;

	//���������
	AcDbObjectIdArray idsTemp;
	ReMakePLine(ids, idsTemp);
	ids.removeAll();
	id = idsTemp.at(0);
	idsTemp.remove(id);
	ids = idsTemp;

	return TRUE;
}

BOOL CGeometricOperations::GetEntsBat(AcDbObjectId & id, AcDbObjectIdArray & ids)
{

	ads_name name1;
	ads_point pt1;
	acedEntSel(L"\nѡ��Ҫ���۳���ʵ�壺", name1, pt1);
	AcDbObjectId objId1;
	acdbGetObjectId(objId1, name1);
	acedSSFree(name1);

	ads_name ss;
	struct resbuf *rb = NULL;
	acutBuildList(RTDXF0, _T("PLINE"), RTNONE);
	CString strTile = L"";
	if (m_nOperType == 2)
	{
		strTile.Format(L"\n��ѡ��۳��ĵؿ飺");
	}
	else
	{
		return FALSE;
	}
	acutPrintf(strTile);
	if (RTNORM != acedSSGet(NULL, NULL, NULL, rb, ss))
	{
		acutRelRb(rb);
		return FALSE;
	}
	acutRelRb(rb);
	Adesk::Int32 nssLen = 0;
	acedSSLength(ss, &nssLen);
	if (nssLen < 1)
	{
		acedSSFree(ss);
		return FALSE;
	}

	ids.append(objId1);

	for (int i = 0; i < nssLen; i++)
	{
		ads_name ent;
		acedSSName(ss, i, ent);
		AcDbObjectId objId;
		acdbGetObjectId(objId, ent);
		ids.append(objId);
		acedSSFree(ent);
	}
	acedSSFree(ss);


	for (int i = 0; i < ids.length(); i++)
	{
		AcDbEntity *pEnt = NULL;
		AcDbPolyline *pPline = NULL;
		if (acdbOpenAcDbEntity(pEnt, ids.at(i), AcDb::kForRead) != Acad::eOk)
			return FALSE;
		pPline = AcDbPolyline::cast(pEnt);
		if (pPline == NULL)
		{
			pEnt->close();
			return FALSE;
		}
		if (!pPline->isClosed())
		{
			pEnt->close();
			return FALSE;
		}
		pEnt->close();
		pEnt = NULL;
		pPline = NULL;
	}

	//���������
	AcDbObjectIdArray idsTemp;
	ReMakePLine(ids, idsTemp);
	id = idsTemp.at(0);
	idsTemp.remove(id);
	ids = idsTemp;

	return TRUE;
}

BOOL CGeometricOperations::GetEntsBat(AcDbObjectIdArray & ids)
{
	ads_name ss;
	struct resbuf *rb = NULL;
	acutBuildList(RTDXF0, _T("PLINE"), RTNONE);
	CString strTile = L"";
	if (m_nOperType == 0)
	{
		strTile.Format(L"\n��ѡ����Ҫ��������(%s)�ĵؿ�: ", L"����");
	}
	else if (m_nOperType == 1)
	{
		strTile.Format(L"\n��ѡ����Ҫ��������(%s)�ĵؿ�: ", L"����");
	}
	else if (m_nOperType == 2)
	{
		strTile.Format(L"\n��ѡ����Ҫ��������(%s)�ĵؿ�: ", L"�");
	}
	else
	{
		return FALSE;
	}
	acutPrintf(strTile);
	if (RTNORM != acedSSGet(NULL, NULL, NULL, rb, ss))
	{
		acutRelRb(rb);
		return FALSE;
	}
	acutRelRb(rb);
	Adesk::Int32 nssLen = 0;
	acedSSLength(ss, &nssLen);
	if (nssLen < 2)
	{
		acedSSFree(ss);
		return FALSE;
	}

	for (int i = 0; i < nssLen; i++)
	{
		ads_name ent;
		acedSSName(ss, i, ent);
		AcDbObjectId objId;
		acdbGetObjectId(objId, ent);
		ids.append(objId);
		acedSSFree(ent);
	}
	acedSSFree(ss);


	for (int i = 0; i < ids.length(); i++)
	{
		AcDbEntity *pEnt = NULL;
		AcDbPolyline *pPline = NULL;
		if (acdbOpenAcDbEntity(pEnt, ids.at(i), AcDb::kForRead) != Acad::eOk)
			return FALSE;
		pPline = AcDbPolyline::cast(pEnt);
		if (pPline == NULL)
		{
			pEnt->close();
			return FALSE;
		}
		if (!pPline->isClosed())
		{
			pEnt->close();
			return FALSE;
		}
		pEnt->close();
		pEnt = NULL;
		pPline = NULL;
	}

	//���������
	AcDbObjectIdArray idsTemp;
	ReMakePLine(ids, idsTemp);
	ids.removeAll();
	ids = idsTemp;

	return TRUE;

}

BOOL CGeometricOperations::IsCurveLenZero(const AcDbEntity * pEnt)
{
	AcDbCurve *pCur = NULL;
	pCur = AcDbCurve::cast(pEnt);
	if (pCur != NULL)
	{
		double parmStart = 0;
		double parmEnd = 0;
		pCur->getStartParam(parmStart);
		pCur->getEndParam(parmEnd);
		if (parmStart == parmEnd)
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CGeometricOperations::IsSamePoint(const AcGePoint3dArray ptArr)
{
	if (ptArr.length() < 2)
		return FALSE;
	double x = ptArr[0].x, y = ptArr[0].y, z = ptArr[0].z;
	for (int i = 1; i < ptArr.length(); i++)
	{
		if (ptArr[i].x != x)
			return FALSE;
		if (ptArr[i].y != y)
			return FALSE;
		if (ptArr[i].z != z)
			return FALSE;
	}
	return TRUE;
}

BOOL CGeometricOperations::DeleteEnts(const AcDbObjectIdArray ids)
{
	for each (AcDbObjectId var in ids)
	{
		AcDbEntity *pEnt;
		if (Acad::eOk == acdbOpenAcDbEntity(pEnt, var, AcDb::kForWrite))
		{
			pEnt->erase(true);
			pEnt->close();
			pEnt = NULL;
		}
	}
	return TRUE;
}

BOOL CGeometricOperations::ReMakePLine(const AcDbObjectIdArray ids, AcDbObjectIdArray &idsNew)
{

	for each (AcDbObjectId id in ids)
	{
		AcDbEntity *pEnt = NULL;
		Acad::ErrorStatus es;
		es = acdbOpenAcDbEntity(pEnt, id, AcDb::kForRead);
		if (es != Acad::eOk)
			continue;
		AcDbObjectIdArray idsTemp;
		if (pEnt->isKindOf(AcDbPolyline::desc()))
		{
			AcDbVoidPtrArray ptrArray;
			es = pEnt->explode(ptrArray);
			if (es == Acad::eOk)
			{
				//������ӷֽ���ʵ�嵽��ǰ�ռ�,������������ݿ����Ҫ�Լ�delete�ֽ��Ķ���
				for (int i = 0; i < ptrArray.length(); i++)
				{
					AcDbEntity*pEnt1 = (AcDbEntity*)ptrArray.at(i);
					if (pEnt1 != NULL)
					{
						if (!IsCurveLenZero(pEnt1))
						{
							idsTemp.append(PostToModelSpace(pEnt1));
						}
					}
					pEnt1->close();
				}
			}

		}
		pEnt->close();
		AcDbObjectIdArray idsTemp1 = idsTemp;
		if (idsTemp.length() > 0)
		{
			AcDbPolyline *pPLineTemp = CJoinToPline::MakeJonedPoly(idsTemp);
			if (pPLineTemp != NULL)
			{
				if (!pPLineTemp->isClosed())
				{
					pPLineTemp->setClosed(true);
				}
				idsNew.append(PostToModelSpace(pPLineTemp));
				pPLineTemp->close();
			}
		}
		//ɾ������ʵ��
		DeleteEnts(idsTemp1);
	}
	return TRUE;
}

BOOL CGeometricOperations::RegionIsIntersectWith(const AcDbRegion * pRegion1, const AcDbRegion * pRegion2)
{
	AcGePoint3dArray ptArr;
	pRegion1->intersectWith(pRegion2, AcDb::Intersect::kOnBothOperands, ptArr);
	if (ptArr.length() < 2)
	{
		return FALSE;
	}
	else
	{
		if (IsSamePoint(ptArr))
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
}

BOOL CGeometricOperations::GetRegionUnionBat(AcDbObjectIdArray & idsRegion, AcDbObjectIdArray & idsRegionUnion)
{
	for (int i = 0; i < idsRegion.length() - 1; i++)
	{
		for (int j = 0; j < idsRegion.length(); j++)
		{
			AcDbObjectPointer<AcDbRegion>pRegion1(idsRegion[i], AcDb::kForWrite);
			if (Acad::eOk != pRegion1.openStatus())
			{
				continue;;
			}
			for (int k = i + 1; k < idsRegion.length(); k++)
			{
				AcDbObjectPointer<AcDbRegion>pRegion2(idsRegion[k], AcDb::kForWrite);
				if (Acad::eOk != pRegion2.openStatus())
				{
					pRegion2->close();
					continue;;
				}

				//ִ�в�������,���жϵؿ��Ƿ��н���������н����Ž��в�������
				if (!RegionIsBoolIntersect(pRegion1, pRegion2))
				{
					//���û�н����������ж��Ƿ��н��㣬������ߵؿ鲻�ܺϲ�����
					if (!RegionIsIntersectWith(pRegion1, pRegion2))
					{
						pRegion2->close();
						continue;
					}
				}

				//ִ�в�������
				Acad::ErrorStatus es = pRegion1->booleanOper(AcDb::kBoolUnite, pRegion2);
				if (Acad::eOk != eOk)
				{
					acutPrintf(_T("\n��������ʧ��!,������:%s"), acadErrorStatusText(es));
					pRegion2->close();
					continue;;
				}
				//ɾ�����������ʵ�壬ִ�в�����������󣬲���������������Ϊ0��
				if (pRegion2->isNull())
				{
					if (!idsRegionUnion.contains(pRegion1->objectId()))
					{
						idsRegionUnion.append(pRegion1->objectId());
					}
					pRegion2->erase();
					pRegion2->close();
					idsRegion.removeAt(k);
					k--;
				}
			}
		}
	}

	return TRUE;
}

BOOL CGeometricOperations::GetRegionIntersectionBat(AcDbObjectIdArray &idsRegion, AcDbObjectIdArray &idsRegionIntersection)
{

	AcDbObjectPointer<AcDbRegion>pRegion1(idsRegion[0], AcDb::kForWrite);
	if (Acad::eOk != pRegion1.openStatus())
	{
		return FALSE;
	}
	for (int k = 1; k < idsRegion.length(); k++)
	{
		AcDbObjectPointer<AcDbRegion>pRegion2(idsRegion[k], AcDb::kForWrite);
		if (Acad::eOk != pRegion2.openStatus())
		{
			pRegion2->close();
			continue;;
		}

		//ִ�в�������
		Acad::ErrorStatus es = pRegion1->booleanOper(AcDb::kBoolIntersect, pRegion2);
		if (Acad::eOk != eOk)
		{
			acutPrintf(_T("\n��������ʧ��!,������:%s"), acadErrorStatusText(es));
			pRegion2->close();
			continue;;
		}
		//û�н���
		if (pRegion1->isNull())
		{
			return FALSE;
		}
		//ɾ�����������ʵ�壬ִ�в�����������󣬲���������������Ϊ0��
		if (pRegion2->isNull())
		{
			if (!idsRegionIntersection.contains(pRegion1->objectId()))
			{
				idsRegionIntersection.append(pRegion1->objectId());
			}
			pRegion2->erase();
			pRegion2->close();
		}
	}

	return TRUE;
}

BOOL CGeometricOperations::GetRegionSubtractionBat(AcDbObjectId & idRegion, AcDbObjectIdArray & idsRegion, AcDbObjectIdArray & idsRegionIntersection)
{
	AcDbObjectPointer<AcDbRegion>pRegion1(idRegion, AcDb::kForWrite);
	if (Acad::eOk != pRegion1.openStatus())
	{
		return FALSE;
	}
	for (int k = 0; k < idsRegion.length(); k++)
	{
		AcDbObjectPointer<AcDbRegion>pRegion2(idsRegion[k], AcDb::kForWrite);
		if (Acad::eOk != pRegion2.openStatus())
		{
			pRegion2->close();
			continue;;
		}


		//����н������������������ǰ�����ϵ����ô�Ͳ����ɲ
		if ((!RegionIsIntersectWith(pRegion1, pRegion2)) && (RegionIsBoolIntersect(pRegion1, pRegion2)))
		{
			acutPrintf(_T("\nû�в,ɾ������!"));
			pRegion1->erase();
			pRegion2->erase();
			pRegion1->close();
			pRegion2->close();
			continue;;
		}
		//ִ�в�������
		Acad::ErrorStatus es = pRegion1->booleanOper(AcDb::kBoolSubtract, pRegion2);
		if (Acad::eOk != eOk)
		{
			acutPrintf(_T("\n��������ʧ��!,������:%s"), acadErrorStatusText(es));
			continue;;
		}
		//�ж��Ƿ��н��������Ϊ��,��ɾ������       
		if (pRegion1->isNull())
		{
			acutPrintf(_T("\nû�в,ɾ������!"));
			pRegion1->erase();
			pRegion2->erase();
			pRegion1->close();
			pRegion2->close();
			continue;;
		}
		//ɾ�����������ʵ�壬ִ�н�����������󣬲���������������Ϊ0��
		if (pRegion2->isNull())
		{
			pRegion2->erase();
		}
	}

	idsRegionIntersection.append(idRegion);

	return TRUE;
}

BOOL CGeometricOperations::RegionIsBoolIntersect(const AcDbRegion * pRegion1, const AcDbRegion * pRegion2)
{
	AcDbRegion *pRegion3 = (AcDbRegion *)pRegion1->clone();
	AcDbRegion *pRegion4 = (AcDbRegion *)pRegion2->clone();
	Acad::ErrorStatus es = pRegion3->booleanOper(AcDb::kBoolIntersect, pRegion4);
	if (Acad::eOk != eOk)
	{
		acutPrintf(_T("\n��������ʧ��!,������:%s"), acadErrorStatusText(es));
		return FALSE;
	}
	if (pRegion3->isNull())
	{
		pRegion3->erase();
		pRegion4->erase();
		pRegion3->close();
		pRegion4->close();
		return FALSE;
	}
	else
	{
		pRegion3->erase();
		pRegion4->erase();
		pRegion3->close();
		pRegion4->close();
		return TRUE;
	}
}



