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
	//不同的方式
	if (m_nOperType == 0)
	{
		acutPrintf(_T("[开始]求交\n"));
		GeometryIntersection();
	}
	else if (m_nOperType == 1)
	{
		acutPrintf(_T("[开始]求并\n"));
		GeometryUnion();
	}
	else if (m_nOperType == 2)
	{
		acutPrintf(_T("[开始]求差\n"));
		GeometrySubtraction();
	}
	else
	{
		acutPrintf(_T("[失败]无效处理方式\n"));
	}
}

void CGeometricOperations::MathDBooleanBat()
{
	//不同的方式
	if (m_nOperType == 0)
	{
		acutPrintf(_T("[开始]求交\n"));
		GeometryIntersectionBat();
	}
	else if (m_nOperType == 1)
	{
		acutPrintf(_T("[开始]求并\n"));
		GeometryUnionBat();
	}
	else if (m_nOperType == 2)
	{
		acutPrintf(_T("[开始]求差\n"));
		GeometrySubtractionBat();
	}
	else
	{
		acutPrintf(_T("[失败]无效处理方式\n"));
	}
}

void CGeometricOperations::GeometryUnion()
{
	AcDbObjectIdArray idsPoly;
	AcDbObjectIdArray idsRegion;

	if (!GetEnts(idsPoly))
	{
		acutPrintf(L"\n选择地块失败！");
		return;
	}

	if (this->PolyLine2Region(idsPoly, idsRegion) == FALSE)
	{
		DeleteEnts(idsPoly);
		return;
	}

	DeleteEnts(idsPoly);

	//使用ARX智能指针写打开面域对象
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
	//执行布尔操作,先判断地块是否有交集，如果有交集才进行并集运算
	if (!RegionIsBoolIntersect(pRegion1, pRegion2))
	{
		//如果没有交集，可以判断是否有交点，解决共线地块不能合并问题
		if (!RegionIsIntersectWith(pRegion1, pRegion2))
		{
			pRegion1->erase();
			pRegion2->erase();
			pRegion1->close();
			pRegion2->close();
			return;
		}
	}

	//执行布尔操作
	Acad::ErrorStatus es = pRegion1->booleanOper(AcDb::kBoolUnite, pRegion2);
	if (Acad::eOk != eOk)
	{
		acutPrintf(_T("\n布尔操作失败!,错误码:%s"), acadErrorStatusText(es));
		return;
	}
	//判断是否有交集，如果为空,则删除对象。       
	if (pRegion1->isNull())
	{
		//acutPrintf(_T("\n没有并集,删除对象!"));
		acutPrintf(_T("\n没有并集"));
		pRegion1->erase();
		pRegion2->erase();
		pRegion1->close();
		pRegion2->close();
		return;
	}
	//删除多余的面域实体，执行并集布尔运算后，参数计算的面域面积为0。
	if (pRegion2->isNull())
	{
		pRegion2->erase();
	}

	//面域转多段线
	//Region2PolyLine(pRegion1);
	AcDbObjectIdArray idsGetPoly;
	Region2PolyLine(pRegion1, idsGetPoly);

	//删除面域
	pRegion1->erase();
	pRegion1->close();
}

void CGeometricOperations::GeometryIntersection()
{

	AcDbObjectIdArray idsPoly;
	AcDbObjectIdArray idsRegion;

	if (!GetEnts(idsPoly))
	{
		acutPrintf(L"\n选择地块失败！");
		return;
	}

	if (this->PolyLine2Region(idsPoly, idsRegion) == FALSE)
	{
		DeleteEnts(idsPoly);
		return;
	}
	DeleteEnts(idsPoly);

	//使用ARX智能指针写打开面域对象
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
	//执行布尔操作
	Acad::ErrorStatus es = pRegion1->booleanOper(AcDb::kBoolIntersect, pRegion2);
	if (Acad::eOk != eOk)
	{
		acutPrintf(_T("\n布尔操作失败!,错误码:%s"), acadErrorStatusText(es));
		return;
	}
	//判断是否有交集，如果为空,则删除对象。       
	if (pRegion1->isNull())
	{
		acutPrintf(_T("\n没有交集,删除对象!"));
		pRegion1->erase();
		pRegion2->erase();
		pRegion1->close();
		pRegion2->close();
		return;
	}
	//删除多余的面域实体，执行交集布尔运算后，参数计算的面域面积为0。
	if (pRegion2->isNull())
	{
		pRegion2->erase();
	}

	//面域转多段线
	//Region2PolyLine(pRegion1);
	AcDbObjectIdArray idsGetPoly;
	Region2PolyLine(pRegion1, idsGetPoly);

	//删除面域
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
		acutPrintf(L"\n选择地块失败！");
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

	//使用ARX智能指针写打开面域对象
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
	//如果有交集，但是两个面域是包含关系，那么就不生成差集
	if ((!RegionIsIntersectWith(pRegion1, pRegion2)) && (RegionIsBoolIntersect(pRegion1, pRegion2)))
	{
		acutPrintf(_T("\n没有差集,删除对象!"));
		pRegion1->erase();
		pRegion2->erase();
		pRegion1->close();
		pRegion2->close();
		return;
	}
	//执行布尔操作
	Acad::ErrorStatus es = pRegion1->booleanOper(AcDb::kBoolSubtract, pRegion2);
	if (Acad::eOk != eOk)
	{
		acutPrintf(_T("\n布尔操作失败!,错误码:%s"), acadErrorStatusText(es));
		return;
	}
	//判断是否有交集，如果为空,则删除对象。       
	if (pRegion1->isNull())
	{
		acutPrintf(_T("\n没有差集,删除对象!"));
		pRegion1->erase();
		pRegion2->erase();
		pRegion1->close();
		pRegion2->close();
		return;
	}
	//删除多余的面域实体，执行交集布尔运算后，参数计算的面域面积为0。
	if (pRegion2->isNull())
	{
		pRegion2->erase();
	}

	//面域转多段线
	//Region2PolyLine(pRegion1);
	AcDbObjectIdArray idsGetPoly;
	Region2PolyLine(pRegion1, idsGetPoly);

	//删除面域
	pRegion1->erase();
	pRegion1->close();
}

void CGeometricOperations::GeometryUnionBat()
{
	AcDbObjectIdArray idsPoly;
	AcDbObjectIdArray idsRegion;

	if (!GetEntsBat(idsPoly))
	{
		acutPrintf(L"\n选择地块失败！");
		return;
	}

	if (this->PolyLine2RegionBat(idsPoly, idsRegion) == FALSE)
	{
		DeleteEnts(idsPoly);
		return;
	}
	DeleteEnts(idsPoly);

	AcDbObjectIdArray idsRegionUnion;
	//从面域中提取合并的面域
	GetRegionUnionBat(idsRegion, idsRegionUnion);

	//面域转多段线
	AcDbObjectIdArray idsGetPoly;
	Region2PolyLine(idsRegionUnion, idsGetPoly);


	//删除面域
	DeleteEnts(idsRegion);
	DeleteEnts(idsRegionUnion);

}

void CGeometricOperations::GeometryIntersectionBat()
{
	AcDbObjectIdArray idsPoly;
	AcDbObjectIdArray idsRegion;

	if (!GetEntsBat(idsPoly))
	{
		acutPrintf(L"\n选择地块失败！");
		return;
	}

	if (this->PolyLine2RegionBat(idsPoly, idsRegion) == FALSE)
	{
		DeleteEnts(idsPoly);
		return;
	}
	DeleteEnts(idsPoly);

	AcDbObjectIdArray idsRegionIntersection;
	//从面域中提取合并的面域
	if (!GetRegionIntersectionBat(idsRegion, idsRegionIntersection))
	{
		//删除面域
		DeleteEnts(idsRegion);
		DeleteEnts(idsRegionIntersection);
		return;
	}

	//面域转多段线
	AcDbObjectIdArray idsGetPoly;
	Region2PolyLine(idsRegionIntersection, idsGetPoly);

	//删除面域
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
		acutPrintf(L"\n选择地块失败！");
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
	//从面域中提取被扣除后的面域
	if (!GetRegionSubtractionBat(idRegion.at(0), idsRegion, idRegionSubtraction))
	{
		//删除面域
		DeleteEnts(idRegion);
		DeleteEnts(idsRegion);
		DeleteEnts(idRegionSubtraction);
		return;
	}

	//面域转多段线
	AcDbObjectIdArray idsGetPoly;
	Region2PolyLine(idRegionSubtraction, idsGetPoly);

	//删除面域
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

	AcDbObjectIdArray regionIds;	//生成的面域ID数组
	AcDbVoidPtrArray curves;		//指向作为面域边界的曲线的指针数组
	AcDbVoidPtrArray regions;		//指向创建的面域对象的指针数组
	AcDbEntity *pEnt = NULL;		//临时指针，用来关闭边界曲线
	AcDbRegion *pRegion = NULL;		//临时对象，用来将面域添加到模型空间

	//用idsPoly初始化cures
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
		//将生成的面域添加到模型空间
		for (int i = 0; i < regions.length(); i++)
		{
			//将空指针转化为指向面域的指针
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

	//关闭作为边界线的对象
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

	AcDbObjectIdArray regionIds;	//生成的面域ID数组
	AcDbVoidPtrArray curves;		//指向作为面域边界的曲线的指针数组
	AcDbVoidPtrArray regions;		//指向创建的面域对象的指针数组
	AcDbEntity *pEnt = NULL;		//临时指针，用来关闭边界曲线
	AcDbRegion *pRegion = NULL;		//临时对象，用来将面域添加到模型空间

									//用idsPoly初始化cures

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
		//将生成的面域添加到模型空间
		for (int i = 0; i < regions.length(); i++)
		{
			//将空指针转化为指向面域的指针
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

	//关闭作为边界线的对象
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


	AcDbObjectIdArray regionIds;	//生成的面域ID数组
	AcDbVoidPtrArray curves;		//指向作为面域边界的曲线的指针数组
	AcDbVoidPtrArray regions;		//指向创建的面域对象的指针数组
	AcDbEntity *pEnt = NULL;		//临时指针，用来关闭边界曲线
	AcDbRegion *pRegion = NULL;		//临时对象，用来将面域添加到模型空间

									//用idsPoly初始化cures
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
		//将生成的面域添加到模型空间
		for (int i = 0; i < regions.length(); i++)
		{
			//将空指针转化为指向面域的指针
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

	//关闭作为边界线的对象
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

	//分解面域
	Acad::ErrorStatus es;
	AcDbVoidPtrArray ptrArray;
	es = pRegion->explode(ptrArray);
	if (Acad::eOk != es)
	{
		acutPrintf(_T("\n面域分解操作失败!,错误码:%s"), acadErrorStatusText(es));
		return FALSE;
	}

	AcDbObjectIdArray idsTemp;

	//遍历添加分解后的实体到当前空间,如果不加入数据库就需要自己delete分解后的对象
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
					acutPrintf(_T("\n子面域分解操作失败!,错误码:%s"), acadErrorStatusText(es));
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
						//关闭pSubEnt对象
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
			//关闭pEnt对象
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
		RTSTR, _T("0"),//表示模糊距离
		RTSTR, _T("C"),
		RTSTR, _T(""),
		RTNONE);

	acedSSFree(result);

	return TRUE;
}

BOOL CGeometricOperations::Region2PolyLine(const AcDbRegion* pRegion, AcDbObjectIdArray &idsPoly)
{
	//分解面域
	Acad::ErrorStatus es;
	AcDbVoidPtrArray ptrArray;
	es = pRegion->explode(ptrArray);
	if (Acad::eOk != es)
	{
		acutPrintf(_T("\n面域分解操作失败!,错误码:%s"), acadErrorStatusText(es));
		return FALSE;
	}

	AcDbObjectIdArray idsTemp;

	//遍历添加分解后的实体到当前空间,如果不加入数据库就需要自己delete分解后的对象
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
					acutPrintf(_T("\n子面域分解操作失败!,错误码:%s"), acadErrorStatusText(es));
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
						//关闭pSubEnt对象
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
			//关闭pEnt对象
			pEnt->close();
		}
	}

	//记录id后续删除实体使用
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

	//删除打散后的实体
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
	//打开当前数据库的当前块表记录
	AcDbBlockTableRecordPointer pBlkRcd(curDoc()->database()->currentSpaceId(), AcDb::kForWrite);
	if (Acad::eOk != pBlkRcd.openStatus())
	{
		acutPrintf(_T("\n打开块表记录失败!,错误码:%s"), acadErrorStatusText(pBlkRcd.openStatus()));
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
		strTile.Format(L"\n请选择两个需要布尔运算(%s)的地块: ", L"交集");
	}
	else if (m_nOperType == 1)
	{
		strTile.Format(L"\n请选择两个需要布尔运算(%s)的地块: ", L"并集");
	}
	else if (m_nOperType == 2)
	{
		strTile.Format(L"\n请选择两个需要布尔运算(%s)的地块: ", L"差集");
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

	//重做多段线
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
		strTile.Format(L"\n请选择两个需要布尔运算(%s)的地块: ", L"交集");
	}
	else if (m_nOperType == 1)
	{
		strTile.Format(L"\n请选择两个需要布尔运算(%s)的地块: ", L"并集");
	}
	else if (m_nOperType == 2)
	{
		strTile.Format(L"\n请选择两个需要布尔运算(%s)的地块: ", L"差集");
	}
	else
	{
		return FALSE;
	}
	acutPrintf(strTile);
	ads_name name1, name2;
	ads_point pt1, pt2;
	acedEntSel(L"\n选择要被扣除的实体：", name1, pt1);
	acedEntSel(L"\n选择需要扣除的实体：", name2, pt2);


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

	//重做多段线
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
	acedEntSel(L"\n选择要被扣除的实体：", name1, pt1);
	AcDbObjectId objId1;
	acdbGetObjectId(objId1, name1);
	acedSSFree(name1);

	ads_name ss;
	struct resbuf *rb = NULL;
	acutBuildList(RTDXF0, _T("PLINE"), RTNONE);
	CString strTile = L"";
	if (m_nOperType == 2)
	{
		strTile.Format(L"\n请选择扣除的地块：");
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

	//重做多段线
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
		strTile.Format(L"\n请选择需要布尔运算(%s)的地块: ", L"交集");
	}
	else if (m_nOperType == 1)
	{
		strTile.Format(L"\n请选择需要布尔运算(%s)的地块: ", L"并集");
	}
	else if (m_nOperType == 2)
	{
		strTile.Format(L"\n请选择需要布尔运算(%s)的地块: ", L"差集");
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

	//重做多段线
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
				//遍历添加分解后的实体到当前空间,如果不加入数据库就需要自己delete分解后的对象
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
		//删除构造实体
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

				//执行布尔操作,先判断地块是否有交集，如果有交集才进行并集运算
				if (!RegionIsBoolIntersect(pRegion1, pRegion2))
				{
					//如果没有交集，可以判断是否有交点，解决共线地块不能合并问题
					if (!RegionIsIntersectWith(pRegion1, pRegion2))
					{
						pRegion2->close();
						continue;
					}
				}

				//执行布尔操作
				Acad::ErrorStatus es = pRegion1->booleanOper(AcDb::kBoolUnite, pRegion2);
				if (Acad::eOk != eOk)
				{
					acutPrintf(_T("\n布尔操作失败!,错误码:%s"), acadErrorStatusText(es));
					pRegion2->close();
					continue;;
				}
				//删除多余的面域实体，执行并集布尔运算后，参数计算的面域面积为0。
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

		//执行布尔操作
		Acad::ErrorStatus es = pRegion1->booleanOper(AcDb::kBoolIntersect, pRegion2);
		if (Acad::eOk != eOk)
		{
			acutPrintf(_T("\n布尔操作失败!,错误码:%s"), acadErrorStatusText(es));
			pRegion2->close();
			continue;;
		}
		//没有交集
		if (pRegion1->isNull())
		{
			return FALSE;
		}
		//删除多余的面域实体，执行并集布尔运算后，参数计算的面域面积为0。
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


		//如果有交集，但是两个面域是包含关系，那么就不生成差集
		if ((!RegionIsIntersectWith(pRegion1, pRegion2)) && (RegionIsBoolIntersect(pRegion1, pRegion2)))
		{
			acutPrintf(_T("\n没有差集,删除对象!"));
			pRegion1->erase();
			pRegion2->erase();
			pRegion1->close();
			pRegion2->close();
			continue;;
		}
		//执行布尔操作
		Acad::ErrorStatus es = pRegion1->booleanOper(AcDb::kBoolSubtract, pRegion2);
		if (Acad::eOk != eOk)
		{
			acutPrintf(_T("\n布尔操作失败!,错误码:%s"), acadErrorStatusText(es));
			continue;;
		}
		//判断是否有交集，如果为空,则删除对象。       
		if (pRegion1->isNull())
		{
			acutPrintf(_T("\n没有差集,删除对象!"));
			pRegion1->erase();
			pRegion2->erase();
			pRegion1->close();
			pRegion2->close();
			continue;;
		}
		//删除多余的面域实体，执行交集布尔运算后，参数计算的面域面积为0。
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
		acutPrintf(_T("\n布尔操作失败!,错误码:%s"), acadErrorStatusText(es));
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



