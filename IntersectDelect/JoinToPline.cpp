#include "stdafx.h"
#include "JoinToPline.h"


CJoinToPline::CJoinToPline()
{
}


CJoinToPline::~CJoinToPline()
{
}


/// <summary>
/// AcDbObjectIdArray from SelectionSet
/// </summary>
/// <param name="sset">SelectionSet</param>
/// <param name="ids">AcDbObjectIdArray</param>
/// <returns></returns>
Acad::ErrorStatus CJoinToPline::ObjectIdArrayFromSelSet(ads_name sset, AcDbObjectIdArray &ids)
{
	Acad::ErrorStatus es = Acad::eOk;
	int32_t nset = -1;
	if (acedSSLength(sset, &nset) != RTNORM) 
		return Acad::eAmbiguousInput;
	ids.setLogicalLength(nset);
	ads_name en;
	AcDbObjectId id;
	for (long i = 0; i < nset; i++) 
	{
		if (acedSSName(sset, i, en) == RTNORM) {
			if ((es = acdbGetObjectId(id, en)) != Acad::eOk) 
				return es;
			ids[i] = id;
		}
	}
	return Acad::eOk;
}

/// <summary>
/// Create polyline from lines and arcs.
/// Those ids will remove from input array
/// </summary>
/// <param name="ids">Array of id's</param>
/// <param name="FUZZ">The accuracy of determining the distance between points</param>
/// <returns></returns>
AcDbPolyline* CJoinToPline::MakeJonedPoly(AcDbObjectIdArray &ids,double FUZZ)
{
	AcDbPolyline *p = new AcDbPolyline();
	p->setDatabaseDefaults();
	AcDbObjectId idFirst = ids[0];
	AcGePoint3d nextPt = AcGePoint3d::kOrigin;
	AcGePoint3d prevPt = AcGePoint3d::kOrigin;

	AcDbObjectPointer<AcDbCurve> c(idFirst, AcDb::kForRead);
	if (c.openStatus() == Acad::eOk) 
	{
		AcGePoint3d ptStart, ptEnd;
		c->getStartPoint(ptStart);
		c->getEndPoint(ptEnd);
		p->addVertexAt(0, asPnt2d(asDblArray(ptStart)), BulgeFromArc(c, false), 0, 0);
		p->addVertexAt(1, asPnt2d(asDblArray(ptEnd)), 0, 0, 0);
		nextPt = ptEnd;
		prevPt = ptStart;
	}

	ids.remove(idFirst);
	int prevCnt = ids.length() + 1;

	while (ids.length() > 0 && ids.length() < prevCnt)
	{
		prevCnt = ids.length();
		for (int i = 0; i < ids.length(); i++) {
			AcDbObjectId id = ids[i];
			AcDbObjectPointer<AcDbCurve> cv(id, AcDb::kForRead);
			if (cv.openStatus() == Acad::eOk) {
				AcGePoint3d ptStart, ptEnd;
				cv->getStartPoint(ptStart);
				cv->getEndPoint(ptEnd);
				if (ptStart.distanceTo(nextPt) < FUZZ || ptEnd.distanceTo(nextPt) < FUZZ) 
				{
					double bulge = BulgeFromArc(cv, ptEnd.distanceTo(nextPt) < FUZZ);
					p->setBulgeAt(p->numVerts() - 1, bulge);
					if (ptStart.distanceTo(nextPt) < FUZZ)
						nextPt = ptEnd;
					else
						nextPt = ptStart;
					p->addVertexAt(p->numVerts(), asPnt2d(asDblArray(nextPt)), 0, 0, 0);
					ids.remove(id);
					break;
				}
				else if (ptStart.distanceTo(prevPt) < FUZZ || ptEnd.distanceTo(prevPt) < FUZZ)
				{
					double bulge = BulgeFromArc(cv, ptStart.distanceTo(prevPt) < FUZZ);
					if (ptStart.distanceTo(prevPt) < FUZZ)
						prevPt = ptEnd;
					else
						prevPt = ptStart;
					p->addVertexAt(0, asPnt2d(asDblArray(prevPt)), bulge, 0, 0);
					ids.remove(id);
					break;
				}
			}
		}
	}
	if (p->numVerts() == 0)
	{
		delete p; 
		return NULL;
	}
	else 
	{
		return p;
	}
}

/// <summary>
/// Getting bulge of curve
/// </summary>
/// <param name="c">AcDbCurve pointer</param>
/// <param name="clockwise">direction.</param>
/// <returns></returns>
double CJoinToPline::BulgeFromArc(AcDbCurve *c, bool clockwise)
{
	double bulge = 0.0;
	AcDbArc *a = AcDbArc::cast(c);
	if (a == NULL) 
		return bulge;

	double newStart =
		(a->startAngle() > a->endAngle()) ?
		(a->startAngle() - 8 * atan(1.0)) :
		(a->startAngle());

	bulge = tan((a->endAngle() - newStart) / 4.0);
	if (clockwise) 
		bulge = -bulge;
	return bulge;
}
