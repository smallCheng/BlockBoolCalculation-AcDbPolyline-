#pragma once
class CJoinToPline
{
public:
	CJoinToPline();
	~CJoinToPline();

	/// <summary>
	/// AcDbObjectIdArray from SelectionSet
	/// </summary>
	/// <param name="sset">SelectionSet</param>
	/// <param name="ids">AcDbObjectIdArray</param>
	/// <returns></returns>
	static Acad::ErrorStatus ObjectIdArrayFromSelSet(ads_name sset, AcDbObjectIdArray &ids);

	/// <summary>
	/// Create polyline from lines and arcs.
	/// Those ids will remove from input array
	/// </summary>
	/// <param name="ids">Array of id's</param>
	/// <param name="FUZZ">The accuracy of determining the distance between points</param>
	/// <returns></returns>
	static AcDbPolyline* MakeJonedPoly(AcDbObjectIdArray &ids,double FUZZ = AcGeContext::gTol.equalPoint());

	/// <summary>
	/// Getting bulge of curve
	/// </summary>
	/// <param name="c">AcDbCurve pointer</param>
	/// <param name="clockwise">direction.</param>
	/// <returns></returns>
	static double BulgeFromArc(AcDbCurve *c, bool clockwise);
};

