/* $Header$ */
/* $NoKeywords: $ */
/*
//
// Copyright (c) 1993-2001 Robert McNeel & Associates. All rights reserved.
// Rhinoceros is a registered trademark of Robert McNeel & Assoicates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//
// For complete openNURBS copyright information see <http://www.opennurbs.org>.
//
////////////////////////////////////////////////////////////////
*/

////////////////////////////////////////////////////////////////
//
//   Definition of virtual parametric curve
//
////////////////////////////////////////////////////////////////

#if !defined(OPENNURBS_CURVE_INC_)
#define OPENNURBS_CURVE_INC_


class ON_Curve;
class ON_Plane;
class ON_Arc;
class ON_NurbsCurve;
class ON_CurveTree;


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
class ON_Ray {
public:
  ON_3dPoint m_origin;
  ON_3dVector m_dir;

  ON_Ray(ON_3dPoint& origin, ON_3dVector& dir) : m_origin(origin), m_dir(dir) {}
  ON_Ray(const ON_Ray& r) : m_origin(r.m_origin), m_dir(r.m_dir)  {}
  ON_Ray& operator=(const ON_Ray& r) {
    m_origin = r.m_origin;
    m_dir = r.m_dir;
    return *this;
  }

  ON_3dPoint PointAt(double t) const {
    return m_origin + m_dir * t;
  }

  double DistanceTo(const ON_3dPoint& pt, double* out_t = NULL) const {
    ON_3dVector w = pt - m_origin;
    double c1 = w * m_dir;
    if (c1 <= 0) return pt.DistanceTo(m_origin);
    double c2 = m_dir * m_dir;
    double b = c1 / c2;
    ON_3dPoint p = m_dir * b + m_origin;
    if (out_t != NULL) *out_t = b;
    return p.DistanceTo(pt);
  }
};

class Sample;


typedef int (*ON_MassPropertiesCurve)( const ON_Curve&, void*, int, ON_3dPoint, ON_3dVector, ON_MassProperties&, bool, bool, bool, bool, double, double );

class ON_CLASS ON_Curve : public ON_Geometry
{
  // pure virtual class for curve objects

  // Any object derived from ON_Curve should have a
  //   ON_OBJECT_DECLARE(ON_...);
  // as the last line of its class definition and a
  //   ON_OBJECT_IMPLEMENT( ON_..., ON_baseclass );
  // in a .cpp file.
  //
  // See the definition of ON_Object for details.
  ON_OBJECT_DECLARE(ON_Curve);

public:
  // virtual ON_Object::DestroyRuntimeCache override
  void DestroyRuntimeCache( bool bDelete = true );

public:
  ON_Curve();
  ON_Curve(const ON_Curve&);
  ON_Curve& operator=(const ON_Curve&);
  virtual ~ON_Curve();

  // virtual ON_Object::SizeOf override
  unsigned int SizeOf() const;

  // virtual ON_Geometry override
  bool EvaluatePoint( const class ON_ObjRef& objref, ON_3dPoint& P ) const;

  /*
  Description:
    Get a duplicate of the curve.
  Returns:
    A duplicate of the curve.
  Remarks:
    The caller must delete the returned curve.
    For non-ON_CurveProxy objects, this simply duplicates the curve using
    ON_Object::Duplicate.
    For ON_CurveProxy objects, this duplicates the actual proxy curve
    geometry and, if necessary, trims and reverse the result to that
    the returned curve's parameterization and locus match the proxy curve's.
  */
  virtual
  ON_Curve* DuplicateCurve() const;

  // Description:
  //   overrides virtual ON_Object::ObjectType.
  // Returns:
  //   ON::curve_object
  ON::object_type ObjectType() const;

  /*
	Description:
    Get tight bounding box of the curve.
	Parameters:
		tight_bbox - [in/out] tight bounding box
		bGrowBox -[in]	(default=false)
      If true and the input tight_bbox is valid, then returned
      tight_bbox is the union of the input tight_bbox and the
      curve's tight bounding box.
		xform -[in] (default=NULL)
      If not NULL, the tight bounding box of the transformed
      curve is calculated.  The curve is not modified.
	Returns:
    True if the returned tight_bbox is set to a valid
    bounding box.
  */
	bool GetTightBoundingBox(
			ON_BoundingBox& tight_bbox,
      int bGrowBox = false,
			const ON_Xform* xform = 0
      ) const;

  ////////////////////////////////////////////////////////////////////
  // curve interface

  // Description:
  //   Gets domain of the curve
  // Parameters:
  //   t0 - [out]
  //   t1 - [out] domain is [*t0, *t1]
  // Returns:
  //   TRUE if successful.
  BOOL GetDomain( double* t0, double* t1 ) const;

  // Returns:
  //   domain of the curve.
  virtual
  ON_Interval Domain() const = 0;

  /*
  Description:
    Set the domain of the curve.
  Parameters:
    domain - [in] increasing interval
  Returns:
    true if successful.
  */
  bool SetDomain( ON_Interval domain );

  // Description:
  //   Set the domain of the curve
  // Parameters:
  //   t0 - [in]
  //   t1 - [in] new domain will be [t0,t1]
  // Returns:
  //   TRUE if successful.
  virtual
  BOOL SetDomain(
        double t0,
        double t1
        );


  /*
  Description:
    If this curve is closed, then modify it so that
    the start/end point is at curve parameter t.
  Parameters:
    t - [in] curve parameter of new start/end point.  The
             returned curves domain will start at t.
  Returns:
    TRUE if successful.
  */
  virtual
  BOOL ChangeClosedCurveSeam(
            double t
            );

  /*
  Description:
    Change the dimension of a curve.
  Parameters:
    desired_dimension - [in]
  Returns:
    TRUE if the curve's dimension was already desired_dimension
    or if the curve's dimension was successfully changed to
    desired_dimension.
  */
  virtual
  bool ChangeDimension(
          int desired_dimension
          );


  // Description:
  //   Get number of nonempty smooth (c-infinity) spans in curve
  // Returns:
  //   Number of nonempty smooth (c-infinity) spans.
  virtual
  int SpanCount() const = 0;

  // Description:
  //   Get number of parameters of "knots".
  // Parameters:
  //   knots - [out] an array of length SpanCount()+1 is filled in
  //       with the parameters where the curve is not smooth (C-infinity).
  // Returns:
  //   TRUE if successful
  virtual
  BOOL GetSpanVector(
        double* knots
        ) const = 0; //

  //////////
  // If t is in the domain of the curve, GetSpanVectorIndex() returns the
  // span vector index "i" such that span_vector[i] <= t <= span_vector[i+1].
  // The "side" parameter determines which span is selected when t is at the
  // end of a span.
  virtual
  BOOL GetSpanVectorIndex(
        double t ,               // [IN] t = evaluation parameter
        int side,                // [IN] side 0 = default, -1 = from below, +1 = from above
        int* span_vector_index,  // [OUT] span vector index
        ON_Interval* span_domain // [OUT] domain of the span containing "t"
        ) const;

  // Description:
  //   Returns maximum algebraic degree of any span
  //   or a good estimate if curve spans are not algebraic.
  // Returns:
  //   degree
  virtual
  int Degree() const = 0;

  // Description:
  //   Returns maximum algebraic degree of any span
  //   or a good estimate if curve spans are not algebraic.
  // Returns:
  //   degree
  virtual
  BOOL GetParameterTolerance( // returns tminus < tplus: parameters tminus <= s <= tplus
         double t,       // [IN] t = parameter in domain
         double* tminus, // [OUT] tminus
         double* tplus   // [OUT] tplus
         ) const;

  // Description:
  //   Test a curve to see if the locus if its points is a line segment.
  // Parameters:
  //   tolerance - [in] // tolerance to use when checking linearity
  // Returns:
  //   TRUE if the ends of the curve are farther than tolerance apart
  //   and the maximum distance from any point on the curve to
  //   the line segment connecting the curve's ends is <= tolerance.
  virtual
  BOOL IsLinear(
        double tolerance = ON_ZERO_TOLERANCE
        ) const;

  /*
  Description:
    Several types of ON_Curve can have the form of a polyline including
    a degree 1 ON_NurbsCurve, an ON_PolylineCurve, and an ON_PolyCurve
    all of whose segments are some form of polyline.  IsPolyline tests
    a curve to see if it can be represented as a polyline.
  Parameters:
    pline_points - [out] if not NULL and TRUE is returned, then the
        points of the polyline form are returned here.
    t - [out] if not NULL and TRUE is returned, then the parameters of
        the polyline points are returned here.
  Returns:
    @untitled table
    0        curve is not some form of a polyline
    >=2      number of points in polyline form
  */
  virtual
  int IsPolyline(
        ON_SimpleArray<ON_3dPoint>* pline_points = NULL,
        ON_SimpleArray<double>* pline_t = NULL
        ) const;

  // Description:
  //   Test a curve to see if the locus if its points is an arc or circle.
  // Parameters:
  //   plane - [in] if not NULL, test is performed in this plane
  //   arc - [out] if not NULL and TRUE is returned, then arc parameters
  //               are filled in
  //   tolerance - [in] tolerance to use when checking
  // Returns:
  //   ON_Arc.m_angle > 0 if curve locus is an arc between
  //   specified points.  If ON_Arc.m_angle is 2.0*ON_PI, then the curve
  //   is a circle.
  virtual
  BOOL IsArc(
        const ON_Plane* plane = NULL,
        ON_Arc* arc = NULL,
        double tolerance = ON_ZERO_TOLERANCE
        ) const;

  virtual
  bool IsEllipse(
      const ON_Plane* plane = NULL,
      ON_Ellipse* ellipse = NULL,
      double tolerance = ON_ZERO_TOLERANCE
      ) const;

  // Description:
  //   Test a curve to see if it is planar.
  // Parameters:
  //   plane - [out] if not NULL and TRUE is returned,
  //                 the plane parameters are filled in.
  //   tolerance - [in] tolerance to use when checking
  // Returns:
  //   TRUE if there is a plane such that the maximum distance from
  //   the curve to the plane is <= tolerance.
  virtual
  BOOL IsPlanar(
        ON_Plane* plane = NULL,
        double tolerance = ON_ZERO_TOLERANCE
        ) const;

  // Description:
  //   Test a curve to see if it lies in a specific plane.
  // Parameters:
  //   test_plane - [in]
  //   tolerance - [in] tolerance to use when checking
  // Returns:
  //   TRUE if the maximum distance from the curve to the
  //   test_plane is <= tolerance.
  virtual
  BOOL IsInPlane(
        const ON_Plane& test_plane,
        double tolerance = ON_ZERO_TOLERANCE
        ) const = 0;

  /*
  Description:
    Decide if it makes sense to close off this curve by moving
    the endpoint to the start based on start-end gap size and length
    of curve as approximated by chord defined by 6 points.
  Parameters:
    tolerance - [in] maximum allowable distance between start and end.
                     if start - end gap is greater than tolerance, returns false
    min_abs_size - [in] if greater than 0.0 and none of the interior sampled
                     points are at least min_abs_size from start, returns false.
    min_rel_size - [in] if greater than 1.0 and chord length is less than
                     min_rel_size*gap, returns false.
  Returns:
    true if start and end points are close enough based on above conditions.
  */

  bool IsClosable(
        double tolerance,
        double min_abs_size = 0.0,
        double min_rel_size = 10.0
        ) const;

  // Description:
  //   Test a curve to see if it is closed.
  // Returns:
  //   TRUE if the curve is closed.
  virtual
  BOOL IsClosed() const;

  // Description:
  //   Test a curve to see if it is periodic.
  // Returns:
  //   TRUE if the curve is closed and at least C2 at the start/end.
  virtual
  BOOL IsPeriodic() const;

  /*
  Description:
    Search for a derivatitive, tangent, or curvature
    discontinuity.
  Parameters:
    c - [in] type of continity to test for.
    t0 - [in] Search begins at t0. If there is a discontinuity
              at t0, it will be ignored.  This makes it
              possible to repeatedly call GetNextDiscontinuity
              and step through the discontinuities.
    t1 - [in] (t0 != t1)  If there is a discontinuity at t1 is
              will be ingored unless c is a locus discontinuity
              type and t1 is at the start or end of the curve.
    t - [out] if a discontinuity is found, then *t reports the
          parameter at the discontinuity.
    hint - [in/out] if GetNextDiscontinuity will be called
       repeatedly, passing a "hint" with initial value *hint=0
       will increase the speed of the search.
    dtype - [out] if not NULL, *dtype reports the kind of
        discontinuity found at *t.  A value of 1 means the first
        derivative or unit tangent was discontinuous.  A value
        of 2 means the second derivative or curvature was
        discontinuous.  A value of 0 means teh curve is not
        closed, a locus discontinuity test was applied, and
        t1 is at the start of end of the curve.
    cos_angle_tolerance - [in] default = cos(1 degree) Used only
        when c is ON::G1_continuous or ON::G2_continuous.  If the
        cosine of the angle between two tangent vectors is
        <= cos_angle_tolerance, then a G1 discontinuity is reported.
    curvature_tolerance - [in] (default = ON_SQRT_EPSILON) Used
        only when c is ON::G2_continuous.  If K0 and K1 are
        curvatures evaluated from above and below and
        |K0 - K1| > curvature_tolerance, then a curvature
        discontinuity is reported.
  Returns:
    Parametric continuity tests c = (C0_continuous, ..., G2_continuous):

      TRUE if a parametric discontinuity was found strictly
      between t0 and t1. Note well that all curves are
      parametrically continuous at the ends of their domains.

    Locus continuity tests c = (C0_locus_continuous, ...,G2_locus_continuous):

      TRUE if a locus discontinuity was found strictly between
      t0 and t1 or at t1 is the at the end of a curve.
      Note well that all open curves (IsClosed()=false) are locus
      discontinuous at the ends of their domains.  All closed
      curves (IsClosed()=true) are at least C0_locus_continuous at
      the ends of their domains.
  */
  virtual
  bool GetNextDiscontinuity(
                  ON::continuity c,
                  double t0,
                  double t1,
                  double* t,
                  int* hint=NULL,
                  int* dtype=NULL,
                  double cos_angle_tolerance=0.99984769515639123915701155881391,
                  double curvature_tolerance=ON_SQRT_EPSILON
                  ) const;

  /*
  Description:
    Test continuity at a curve parameter value.
  Parameters:
    c - [in] type of continuity to test for. Read ON::continuity
             comments for details.
    t - [in] parameter to test
    hint - [in] evaluation hint
    point_tolerance - [in] if the distance between two points is
        greater than point_tolerance, then the curve is not C0.
    d1_tolerance - [in] if the difference between two first derivatives is
        greater than d1_tolerance, then the curve is not C1.
    d2_tolerance - [in] if the difference between two second derivatives is
        greater than d2_tolerance, then the curve is not C2.
    cos_angle_tolerance - [in] default = cos(1 degree) Used only when
        c is ON::G1_continuous or ON::G2_continuous.  If the cosine
        of the angle between two tangent vectors
        is <= cos_angle_tolerance, then a G1 discontinuity is reported.
    curvature_tolerance - [in] (default = ON_SQRT_EPSILON) Used only when
        c is ON::G2_continuous.  If K0 and K1 are curvatures evaluated
        from above and below and |K0 - K1| > curvature_tolerance,
        then a curvature discontinuity is reported.
  Returns:
    TRUE if the curve has at least the c type continuity at
    the parameter t.
  */
  virtual
  bool IsContinuous(
    ON::continuity c,
    double t,
    int* hint = NULL,
    double point_tolerance=ON_ZERO_TOLERANCE,
    double d1_tolerance=ON_ZERO_TOLERANCE,
    double d2_tolerance=ON_ZERO_TOLERANCE,
    double cos_angle_tolerance=0.99984769515639123915701155881391,
    double curvature_tolerance=ON_SQRT_EPSILON
    ) const;


  // Description:
  //   Reverse the direction of the curve.
  // Returns:
  //   TRUE if curve was reversed.
  // Remarks:
  //   If reveresed, the domain changes from [a,b] to [-b,-a]
  virtual
  BOOL Reverse()=0;


  /*
  Description:
    Force the curve to start at a specified point.
  Parameters:
    start_point - [in]
  Returns:
    TRUE if successful.
  Remarks:
    Some end points cannot be moved.  Be sure to check return
    code.
  See Also:
    ON_Curve::SetEndPoint
    ON_Curve::PointAtStart
    ON_Curve::PointAtEnd
  */
  virtual
  BOOL SetStartPoint(
          ON_3dPoint start_point
          );

  /*
  Description:
    Force the curve to end at a specified point.
  Parameters:
    end_point - [in]
  Returns:
    TRUE if successful.
  Remarks:
    Some end points cannot be moved.  Be sure to check return
    code.
  See Also:
    ON_Curve::SetStartPoint
    ON_Curve::PointAtStart
    ON_Curve::PointAtEnd
  */
  virtual
  BOOL SetEndPoint(
          ON_3dPoint end_point
          );

  // Description:
  //   Evaluate point at a parameter.
  // Parameters:
  //   t - [in] evaluation parameter
  // Returns:
  //   Point (location of curve at the parameter t).
  // Remarks:
  //   No error handling.
  // See Also:
  //   ON_Curve::EvPoint
  //   ON_Curve::PointAtStart
  //   ON_Curve::PointAtEnd
  ON_3dPoint  PointAt(
                double t
                ) const;

  // Description:
  //   Evaluate point at the start of the curve.
  // Parameters:
  //   t - [in] evaluation parameter
  // Returns:
  //   Point (location of the start of the curve.)
  // Remarks:
  //   No error handling.
  // See Also:
  //   ON_Curve::PointAt
  ON_3dPoint  PointAtStart() const;

  // Description:
  //   Evaluate point at the end of the curve.
  // Parameters:
  //   t - [in] evaluation parameter
  // Returns:
  //   Point (location of the end of the curve.)
  // Remarks:
  //   No error handling.
  // See Also:
  //   ON_Curve::PointAt
  ON_3dPoint  PointAtEnd() const;

  // Description:
  //   Evaluate first derivative at a parameter.
  // Parameters:
  //   t - [in] evaluation parameter
  // Returns:
  //   First derivative of the curve at the parameter t.
  // Remarks:
  //   No error handling.
  // See Also:
  //   ON_Curve::Ev1Der
  ON_3dVector DerivativeAt(
                double t
                ) const;

  // Description:
  //   Evaluate unit tangent vector at a parameter.
  // Parameters:
  //   t - [in] evaluation parameter
  // Returns:
  //   Unit tangent vector of the curve at the parameter t.
  // Remarks:
  //   No error handling.
  // See Also:
  //   ON_Curve::EvTangent
  ON_3dVector TangentAt(
                double t
                ) const;

  // Description:
  //   Evaluate the curvature vector at a parameter.
  // Parameters:
  //   t - [in] evaluation parameter
  // Returns:
  //   curvature vector of the curve at the parameter t.
  // Remarks:
  //   No error handling.
  // See Also:
  //   ON_Curve::EvCurvature
  ON_3dVector CurvatureAt(
                double t
                ) const;

  // Description:
  //   Return a 3d frame at a parameter.
  // Parameters:
  //   t - [in] evaluation parameter
  //   plane - [out] the frame is returned here
  // Returns:
  //   TRUE if successful
  // See Also:
  //   ON_Curve::PointAt, ON_Curve::TangentAt,
  //   ON_Curve::Ev1Der, Ev2Der
  BOOL FrameAt( double t, ON_Plane& plane) const;

  // Description:
  //   Evaluate point at a parameter with error checking.
  // Parameters:
  //   t - [in] evaluation parameter
  //   point - [out] value of curve at t
  //   side - [in] optional - determines which side to evaluate from
  //               =0   default
  //               <0   to evaluate from below,
  //               >0   to evaluate from above
  //   hint - [in/out] optional evaluation hint used to speed repeated evaluations
  // Returns:
  //   FALSE if unable to evaluate.
  // See Also:
  //   ON_Curve::PointAt
  //   ON_Curve::EvTangent
  //   ON_Curve::Evaluate
  BOOL EvPoint(
         double t,
         ON_3dPoint& point,
         int side = 0,
         int* hint = 0
         ) const;

  // Description:
  //   Evaluate first derivative at a parameter with error checking.
  // Parameters:
  //   t - [in] evaluation parameter
  //   point - [out] value of curve at t
  //   first_derivative - [out] value of first derivative at t
  //   side - [in] optional - determines which side to evaluate from
  //               =0   default
  //               <0   to evaluate from below,
  //               >0   to evaluate from above
  //   hint - [in/out] optional evaluation hint used to speed repeated evaluations
  // Returns:
  //   FALSE if unable to evaluate.
  // See Also:
  //   ON_Curve::EvPoint
  //   ON_Curve::Ev2Der
  //   ON_Curve::EvTangent
  //   ON_Curve::Evaluate
  BOOL Ev1Der(
         double t,
         ON_3dPoint& point,
         ON_3dVector& first_derivative,
         int side = 0,
         int* hint = 0
         ) const;

  // Description:
  //   Evaluate second derivative at a parameter with error checking.
  // Parameters:
  //   t - [in] evaluation parameter
  //   point - [out] value of curve at t
  //   first_derivative - [out] value of first derivative at t
  //   second_derivative - [out] value of second derivative at t
  //   side - [in] optional - determines which side to evaluate from
  //               =0   default
  //               <0   to evaluate from below,
  //               >0   to evaluate from above
  //   hint - [in/out] optional evaluation hint used to speed repeated evaluations
  // Returns:
  //   FALSE if unable to evaluate.
  // See Also:
  //   ON_Curve::Ev1Der
  //   ON_Curve::EvCurvature
  //   ON_Curve::Evaluate
  BOOL Ev2Der(
         double t,
         ON_3dPoint& point,
         ON_3dVector& first_derivative,
         ON_3dVector& second_derivative,
         int side = 0,
         int* hint = 0
         ) const;

  /*
  Description:
    Evaluate unit tangent at a parameter with error checking.
  Parameters:
    t - [in] evaluation parameter
    point - [out] value of curve at t
    tangent - [out] value of unit tangent
    side - [in] optional - determines which side to evaluate from
                =0   default
                <0   to evaluate from below,
                >0   to evaluate from above
    hint - [in/out] optional evaluation hint used to speed repeated evaluations
  Returns:
    FALSE if unable to evaluate.
  See Also:
    ON_Curve::TangentAt
    ON_Curve::Ev1Der
  */
  BOOL EvTangent(
         double t,
         ON_3dPoint& point,
         ON_3dVector& tangent,
         int side = 0,
         int* hint = 0
         ) const;

  /*
  Description:
    Evaluate unit tangent and curvature at a parameter with error checking.
  Parameters:
    t - [in] evaluation parameter
    point - [out] value of curve at t
    tangent - [out] value of unit tangent
    kappa - [out] value of curvature vector
    side - [in] optional - determines which side to evaluate from
                =0   default
                <0   to evaluate from below,
                >0   to evaluate from above
    hint - [in/out] optional evaluation hint used to speed repeated evaluations
  Returns:
    FALSE if unable to evaluate.
  See Also:
    ON_Curve::CurvatureAt
    ON_Curve::Ev2Der
    ON_EvCurvature
  */
  BOOL EvCurvature(
         double t,
         ON_3dPoint& point,
         ON_3dVector& tangent,
         ON_3dVector& kappa,
         int side = 0,
         int* hint = 0
         ) const;

  /*
  Description:
    This evaluator actually does all the work.  The other ON_Curve
    evaluation tools call this virtual function.
  Parameters:
    t - [in] evaluation parameter ( usually in Domain() ).
    der_count - [in] (>=0) number of derivatives to evaluate
    v_stride - [in] (>=Dimension()) stride to use for the v[] array
    v - [out] array of length (der_count+1)*v_stride
        curve(t) is returned in (v[0],...,v[m_dim-1]),
        curve'(t) is retuned in (v[v_stride],...,v[v_stride+m_dim-1]),
        curve"(t) is retuned in (v[2*v_stride],...,v[2*v_stride+m_dim-1]),
        etc.
    side - [in] optional - determines which side to evaluate from
                =0   default
                <0   to evaluate from below,
                >0   to evaluate from above
    hint - [in/out] optional evaluation hint used to speed repeated evaluations
  Returns:
    FALSE if unable to evaluate.
  See Also:
    ON_Curve::EvPoint
    ON_Curve::Ev1Der
    ON_Curve::Ev2Der
  */
  virtual
  BOOL Evaluate(
         double t,
         int der_count,
         int v_stride,
         double* v,
         int side = 0,
         int* hint = 0
         ) const = 0;

  //////////
  // Find parameter of the point on a curve that is closest to test_point.
  // If the maximum_distance parameter is > 0, then only points whose distance
  // to the given point is <= maximum_distance will be returned.  Using a
  // positive value of maximum_distance can substantially speed up the search.
  // If the sub_domain parameter is not NULL, then the search is restricted
  // to the specified portion of the curve.
  //
  // TRUE if returned if the search is successful.  FALSE is returned if
  // the search fails.
  virtual
  bool GetClosestPoint(
          const ON_3dPoint&, // test_point
          double* t,       // parameter of local closest point returned here
          double maximum_distance = 0.0,  // maximum_distance
          const ON_Interval* sub_domain = NULL // sub_domain
          ) const;

  //////////
  // Find parameter of the point on a curve that is locally closest to
  // the test_point.  The search for a local close point starts at
  // seed_parameter. If the sub_domain parameter is not NULL, then
  // the search is restricted to the specified portion of the curve.
  //
  // TRUE if returned if the search is successful.  FALSE is returned if
  // the search fails.
  virtual
  BOOL GetLocalClosestPoint(
            const ON_3dPoint& test_point,
            double seed_parameter,
            double* t,
            const ON_Interval* sub_domain = 0
            ) const;

  /*
  Description:
    Find curve's self intersection points.
  Parameters:
    x - [out]
       Intersection events are appended to this array.
    intersection_tolerance - [in]
    curve_domain - [in] optional restriction
  Returns:
    Number of intersection events appended to x.
  */
  virtual
  int IntersectSelf(
          ON_SimpleArray<ON_X_EVENT>& x,
          double intersection_tolerance = 0.0,
          const ON_Interval* curve_domain = 0
          ) const;

  /*
  Description:
    Intersect this curve with curveB.
  Parameters:
    curveB - [in]
    x - [out] Intersection events are appended to this array.
    intersection_tolerance - [in]  If the distance from a point
      on this curve to curveB is <= intersection tolerance,
      then the point will be part of an intersection event.
      If the input intersection_tolerance <= 0.0, then 0.001 is used.
    overlap_tolerance - [in] If t1 and t2 are parameters of this
      curve's intersection events and the distance from curve(t) to
      curveB is <= overlap_tolerance for every t1 <= t <= t2,
      then the event will be returened as an overlap event.
      If the input overlap_tolerance <= 0.0, then
      intersection_tolerance*2.0 is used.
    curveA_domain - [in] optional restriction on this curve's domain
    curveB_domain - [in] optional restriction on curveB domain
  Returns:
    Number of intersection events appended to x.
  */
  int IntersectCurve(
          const ON_Curve* curveB,
          ON_SimpleArray<ON_X_EVENT>& x,
          double intersection_tolerance = 0.0,
          double overlap_tolerance = 0.0,
          const ON_Interval* curveA_domain = 0,
          const ON_Interval* curveB_domain = 0
          ) const;

  /**
     Description:

       Returns the number of 2D intersections of this curve
       with the line defined by segment. Does not calculate actual
       intersection points, so is faster than a general intersection
       algorithm.

     Parameters:

       segment - [in] the segment to intersect with

     Return:

       number of intersections
   */
  virtual int NumIntersectionsWith(const ON_Line& segment) const;


  /**
     Description:

     Return whether the given ray is close to the curve. The "close
     to" relation is defined in terms of the distance between the
     closest points on the curve and the ray... if this distance is
     less than epsilon, then the ray is close to the edge.

  */
  virtual bool CloseTo(const ON_Ray& ray, double epsilon, Sample& closest) const;
  virtual bool CloseTo(const ON_3dPoint& pt, double epsilon, Sample& closest) const;

  /*
  Description:
    Intersect this curve with surfaceB.
  Parameters:
    surfaceB - [in]
    x - [out] Intersection events are appended to this array.
    intersection_tolerance - [in]  If the distance from a point
      on this curve to the surface is <= intersection tolerance,
      then the point will be part of an intersection event.
      If the input intersection_tolerance <= 0.0, then 0.001 is used.
    overlap_tolerance - [in] If t1 and t2 are curve parameters of
      intersection events and the distance from curve(t) to the
      surface is <= overlap_tolerance for every t1 <= t <= t2,
      then the event will be returened as an overlap event.
      If the input overlap_tolerance <= 0.0, then
      intersection_tolerance*2.0 is used.
    curveA_domain - [in] optional restriction on this curve's domain
    surfaceB_udomain - [in] optional restriction on surfaceB u domain
    surfaceB_vdomain - [in] optional restriction on surfaceB v domain
  Returns:
    Number of intersection events appended to x.
  */
  int IntersectSurface(
          const ON_Surface* surfaceB,
          ON_SimpleArray<ON_X_EVENT>& x,
          double intersection_tolerance = 0.0,
          double overlap_tolerance = 0.0,
          const ON_Interval* curveA_domain = 0,
          const ON_Interval* surfaceB_udomain = 0,
          const ON_Interval* surfaceB_vdomain = 0
          ) const;


  /*
  Description:
    Get the length of the curve.
  Parameters:
    length - [out] length returned here.
    fractional_tolerance - [in] desired fractional precision.
        fabs(("exact" length from start to t) - arc_length)/arc_length <= fractional_tolerance
    sub_domain - [in] If not NULL, the calculation is performed on
        the specified sub-domain of the curve (must be non-decreasing)
  Returns:
    TRUE if returned if the length calculation is successful.
    FALSE is returned if the length is not calculated.
  Remarks:
    The arc length will be computed so that
    (returned length - real length)/(real length) <= fractional_tolerance
    More simply, if you want N significant figures in the answer, set the
    fractional_tolerance to 1.0e-N.  For "nice" curves, 1.0e-8 works
    fine.  For very high degree NURBS and NURBS with bad parametrizations,
    use larger values of fractional_tolerance.
  */
  virtual
  BOOL GetLength(
          double* length,
          double fractional_tolerance = 1.0e-8,
          const ON_Interval* sub_domain = NULL
          ) const;

  // obsolete - use ON_Curve::GetLength
  //__declspec(deprecated) BOOL Length(
  //        double* length,
  //        double fractional_tolerance = 1.0e-8,
  //        const ON_Interval* sub_domain = NULL
  //        ) const;

  /*
  Description:
    Used to quickly find short curves.
  Parameters:
    tolerance - [in] (>=0)
    sub_domain - [in] If not NULL, the test is performed
      on the interval that is the intersection of
      sub_domain with Domain().
  Returns:
    True if the length of the curve is <= tolerance.
  Remarks:
    Faster than calling Length() and testing the
    result.
  */
  bool IsShort(
    double tolerance,
    const ON_Interval* sub_domain = NULL
    ) const;

  /*
  Description:
    Looks for segments that are shorter than tolerance
    that can be removed. If bRemoveShortSegments is true,
    then the short segments are removed. Does not change the
    domain, but it will change the relative parameterization.
  Parameters:
    tolerance - [in]
    bRemoveShortSegments - [in] If true, then short segments
                                are removed.
  Returns:
    True if removable short segments can were found.
    False if no removable short segments can were found.
  */
  bool RemoveShortSegments(
    double tolerance,
    bool bRemoveShortSegments = true
    );

  /*
  Description:
    Get the parameter of the point on the curve that is a
    prescribed arc length from the start of the curve.
  Parameters:
    s - [in] normalized arc length parameter.  E.g., 0 = start
         of curve, 1/2 = midpoint of curve, 1 = end of curve.
    t - [out] parameter such that the length of the curve
       from its start to t is arc_length.
    fractional_tolerance - [in] desired fractional precision.
        fabs(("exact" length from start to t) - arc_length)/arc_length <= fractional_tolerance
    sub_domain - [in] If not NULL, the calculation is performed on
        the specified sub-domain of the curve.
  Returns:
    TRUE if successful
  */
  virtual
  BOOL GetNormalizedArcLengthPoint(
          double s,
          double* t,
          double fractional_tolerance = 1.0e-8,
          const ON_Interval* sub_domain = NULL
          ) const;

  /*
  Description:
    Get the parameter of the point on the curve that is a
    prescribed arc length from the start of the curve.
  Parameters:
    count - [in] number of parameters in s.
    s - [in] array of normalized arc length parameters. E.g., 0 = start
         of curve, 1/2 = midpoint of curve, 1 = end of curve.
    t - [out] array of curve parameters such that the length of the
       curve from its start to t[i] is s[i]*curve_length.
    absolute_tolerance - [in] if absolute_tolerance > 0, then the difference
        between (s[i+1]-s[i])*curve_length and the length of the curve
        segment from t[i] to t[i+1] will be <= absolute_tolerance.
    fractional_tolerance - [in] desired fractional precision for each segment.
        fabs("true" length - actual length)/(actual length) <= fractional_tolerance
    sub_domain - [in] If not NULL, the calculation is performed on
        the specified sub-domain of the curve.  A 0.0 s value corresponds to
        sub_domain->Min() and a 1.0 s value corresponds to sub_domain->Max().
  Returns:
    TRUE if successful
  */
  virtual
  BOOL GetNormalizedArcLengthPoints(
          int count,
          const double* s,
          double* t,
          double absolute_tolerance = 0.0,
          double fractional_tolerance = 1.0e-8,
          const ON_Interval* sub_domain = NULL
          ) const;

  // Description:
  //   Removes portions of the curve outside the specified interval.
  // Parameters:
  //   domain - [in] interval of the curve to keep.  Portions of the
  //      curve before curve(domain[0]) and after curve(domain[1]) are
  //      removed.
  // Returns:
  //   TRUE if successful.
  virtual
  BOOL Trim(
    const ON_Interval& domain
    );

  // Description:
  //   Pure virtual function. Default returns false.
  //   Where possible, analytically extends curve to include domain.
  // Parameters:
  //   domain - [in] if domain is not included in curve domain,
  //   curve will be extended so that its domain includes domain.
  //   Will not work if curve is closed. Original curve is identical
  //   to the restriction of the resulting curve to the original curve domain,
  // Returns:
  //   true if successful.
  virtual
  bool Extend(
    const ON_Interval& domain
    );

  /*
  Description:
    Splits (divides) the curve at the specified parameter.
    The parameter must be in the interior of the curve's domain.
    The pointers passed to Split must either be NULL or point to
    an ON_Curve object of the same type.  If the pointer is NULL,
    then a curve will be created in Split().  You may pass "this"
    as left_side or right_side.
  Parameters:
    t - [in] parameter to split the curve at in the
             interval returned by Domain().
    left_side - [out] left portion of curve returned here
    right_side - [out] right portion of curve returned here
	Returns:
		TRUE	- The curve was split into two pieces.
		FALSE - The curve could not be split.  For example if the parameter is
						too close to an endpoint.

  Example:
    For example, if crv were an ON_NurbsCurve, then

          ON_NurbsCurve right_side;
          crv.Split( crv.Domain().Mid() &crv, &right_side );

    would split crv at the parametric midpoint, put the left side
    in crv, and return the right side in right_side.
  */
  virtual
  BOOL Split(
      double t,
      ON_Curve*& left_side,
      ON_Curve*& right_side
    ) const;

  /*
  Description:
    Get a NURBS curve representation of this curve.
  Parameters:
    nurbs_curve - [out] NURBS representation returned here
    tolerance - [in] tolerance to use when creating NURBS
        representation.
    subdomain - [in] if not NULL, then the NURBS representation
        for this portion of the curve is returned.
  Returns:
    0   unable to create NURBS representation
        with desired accuracy.
    1   success - returned NURBS parameterization
        matches the curve's to wthe desired accuracy
    2   success - returned NURBS point locus matches
        the curve's to the desired accuracy and the
        domain of the NURBS curve is correct.  On
        However, This curve's parameterization and
        the NURBS curve parameterization may not
        match to the desired accuracy.  This situation
        happens when getting NURBS representations of
        curves that have a transendental parameterization
        like circles
  Remarks:
    This is a low-level virtual function.  If you do not need
    the parameterization information provided by the return code,
    then ON_Curve::NurbsCurve may be easier to use.
  See Also:
    ON_Curve::NurbsCurve
  */
  virtual
  int GetNurbForm(
        ON_NurbsCurve& nurbs_curve,
        double tolerance = 0.0,
        const ON_Interval* subdomain = NULL
        ) const;
  /*
  Description:
    Does a NURBS curve representation of this curve.
  Parameters:
  Returns:
    0   unable to create NURBS representation
        with desired accuracy.
    1   success - NURBS parameterization
        matches the curve's to wthe desired accuracy
    2   success - NURBS point locus matches
        the curve's and the
        domain of the NURBS curve is correct.
        However, This curve's parameterization and
        the NURBS curve parameterization may not
        match.  This situation
        happens when getting NURBS representations of
        curves that have a transendental parameterization
        like circles
  Remarks:
    This is a low-level virtual function.
  See Also:
    ON_Curve::GetNurbForm
    ON_Curve::NurbsCurve
  */
  virtual
  int HasNurbForm() const;

  /*
  Description:
    Get a NURBS curve representation of this curve.
  Parameters:
    pNurbsCurve - [in/out] if not NULL, this ON_NurbsCurve
    will be used to store the NURBS representation
    of the curve will be returned.
    tolerance - [in] tolerance to use when creating NURBS
        representation.
    subdomain - [in] if not NULL, then the NURBS representation
        for this portion of the curve is returned.
  Returns:
    NULL or a NURBS representation of the curve.
  Remarks:
    See ON_Surface::GetNurbForm for important details about
    the NURBS surface parameterization.
  See Also:
    ON_Curve::GetNurbForm
  */
  ON_NurbsCurve* NurbsCurve(
        ON_NurbsCurve* pNurbsCurve = NULL,
        double tolerance = 0.0,
        const ON_Interval* subdomain = NULL
        ) const;

  // Description:
  //   Convert a NURBS curve parameter to a curve parameter
  //
  // Parameters:
  //   nurbs_t - [in] nurbs form parameter
  //   curve_t - [out] curve parameter
  //
  // Remarks:
  //   If GetNurbForm returns 2, this function converts the curve
  //   parameter to the NURBS curve parameter.
  //
  // See Also:
  //   ON_Curve::GetNurbForm, ON_Curve::GetNurbFormParameterFromCurveParameter
  virtual
  BOOL GetCurveParameterFromNurbFormParameter(
        double nurbs_t,
        double* curve_t
        ) const;

  // Description:
  //   Convert a curve parameter to a NURBS curve parameter.
  //
  // Parameters:
  //   curve_t - [in] curve parameter
  //   nurbs_t - [out] nurbs form parameter
  //
  // Remarks:
  //   If GetNurbForm returns 2, this function converts the curve
  //   parameter to the NURBS curve parameter.
  //
  // See Also:
  //   ON_Curve::GetNurbForm, ON_Curve::GetCurveParameterFromNurbFormParameter
  virtual
  BOOL GetNurbFormParameterFromCurveParameter(
        double curve_t,
        double* nurbs_t
        ) const;


  // Description:
  //   Destroys the runtime curve tree used to speed closest
  //   point and intersection calcuations.
  // Remarks:
  //   If the geometry of the curve is modified in any way,
  //   then call DestroyCurveTree();  The curve tree is
  //   created as needed.
  void DestroyCurveTree();

  // Description:
  //   Get the runtime curve tree used to speed closest point
  //   and intersection calcuations.
  // Returns:
  //   Pointer to the curve tree.
  const ON_CurveTree* CurveTree() const;

  virtual
  ON_CurveTree* CreateCurveTree() const;

  /*
  Description:
    Calculate length mass properties of the curve.
  Parameters:
    mp - [out]
    bLength - [in] true to calculate length
    bFirstMoments - [in] true to calculate volume first moments,
                         length, and length centroid.
    bSecondMoments - [in] true to calculate length second moments.
    bProductMoments - [in] true to calculate length product moments.
  Returns:
    True if successful.
  */
  bool LengthMassProperties(
    ON_MassProperties& mp,
    bool bLength = true,
    bool bFirstMoments = true,
    bool bSecondMoments = true,
    bool bProductMoments = true,
    double rel_tol = 1.0e-6,
    double abs_tol = 1.0e-6
    ) const;

  /*
  Description:
    Calculate area mass properties of a curve.  The curve should
    be planar.
  Parameters:
    base_point - [in]
      A point on the plane that contians the curve.  To get
      the best results, the point should be in the near the
      curve's centroid.

      When computing the area, area centroid, or area first
      moments of a planar area whose boundary is defined by
      several curves, pass the same base_point and plane_normal
      to each call to AreaMassProperties.  The base_point must
      be in the plane of the curves.

      When computing the area second moments or area product
      moments of a planar area whose boundary is defined by several
      curves, you MUST pass the entire area's centroid as the
      base_point and the input mp parameter must contain the
      results of a previous call to
      AreaMassProperties(mp,true,true,false,false,base_point).
      In particular, in this case, you need to make two sets of
      calls; use first set to calculate the area centroid and
      the second set calculate the second moments and product
      moments.
    plane_normal - [in]
      nonzero unit normal to the plane of integration.  If a closed
      curve has counter clock-wise orientation with respect to
      this normal, the area will be positive.  If the a closed curve
      has clock-wise orientation with respect to this normal, the
      area will be negative.
    mp - [out]
    bArea - [in] true to calculate volume
    bFirstMoments - [in] true to calculate area first moments,
                         area, and area centroid.
    bSecondMoments - [in] true to calculate area second moments.
    bProductMoments - [in] true to calculate area product moments.
  Returns:
    True if successful.
  */
  bool AreaMassProperties(
    ON_3dPoint base_point,
    ON_3dVector plane_normal,
    ON_MassProperties& mp,
    bool bArea = true,
    bool bFirstMoments = true,
    bool bSecondMoments = true,
    bool bProductMoments = true,
    double rel_tol = 1.0e-6,
    double abs_tol = 1.0e-6
    ) const;



/*
	Description:
		Lookup a parameter in the m_t array, optionally using a built in snap tolerance to
		snap a parameter value to an element of m_t.
		This function is used by some types derived from ON_Curve to snap parameter values
	Parameters:
		t			- [in]	parameter
		index -[out]	index into m_t such that
					  			if function returns false then

									 @table
									 value                  condition
						  			-1									 t<m_t[0] or m_t is empty
										0<=i<=m_t.Count()-2		m_t[i] < t < m_t[i+1]
										m_t.Count()-1					t>m_t[ m_t.Count()-1]

									if the function returns true then t is equal to, or is closest to and
									within  tolerance of m_t[index].

		bEnableSnap-[in] enable snapping
		m_t				-[in]	Array of parameter values to snap to
		RelTol		-[in] tolerance used in snapping

	Returns:
		true if the t is exactly equal to (bEnableSnap==false), or within tolerance of
		(bEnableSnap==true) m_t[index].
*/
protected:
bool ParameterSearch( double t, int& index, bool bEnableSnap, const ON_SimpleArray<double>& m_t,
															double RelTol=ON_SQRT_EPSILON) const;

protected:
  // Runtime only - ignored by Read()/Write()
  ON_CurveTree* CurveTreeHelper();
  ON_CurveTree* m_ctree;
public:
  static ON_MassPropertiesCurve _MassPropertiesCurve;
};

#if defined(ON_DLL_TEMPLATE)
// This stuff is here because of a limitation in the way Microsoft
// handles templates and DLLs.  See Microsoft's knowledge base
// article ID Q168958 for details.
#pragma warning( push )
#pragma warning( disable : 4231 )
ON_DLL_TEMPLATE template class ON_CLASS ON_SimpleArray<ON_Curve*>;
#pragma warning( pop )
#endif

class ON_CLASS ON_CurveArray : public ON_SimpleArray<ON_Curve*>
{
public:
  ON_CurveArray( int = 0 );
  ~ON_CurveArray(); // deletes any non-NULL curves

  bool Write( ON_BinaryArchive& ) const;
  bool Read( ON_BinaryArchive& );

  void Destroy(); // deletes curves, sets pointers to NULL, sets count to zero

  bool Duplicate( ON_CurveArray& ) const; // operator= copies the pointer values
                                          // duplicate copies the curves themselves

  /*
	Description:
    Get tight bounding box of the bezier.
	Parameters:
		tight_bbox - [in/out] tight bounding box
		bGrowBox -[in]	(default=false)
      If true and the input tight_bbox is valid, then returned
      tight_bbox is the union of the input tight_bbox and the
      tight bounding box of the bezier curve.
		xform -[in] (default=NULL)
      If not NULL, the tight bounding box of the transformed
      bezier is calculated.  The bezier curve is not modified.
	Returns:
    True if the returned tight_bbox is set to a valid
    bounding box.
  */
	bool GetTightBoundingBox(
			ON_BoundingBox& tight_bbox,
      int bGrowBox = false,
			const ON_Xform* xform = 0
      ) const;
};

/*
Description:
  Trim a curve.
Parameters:
  curve - [in] curve to trim (not modified)
  trim_parameters - [in] trimming parameters
    If curve is open, then  trim_parameters must be an increasing
    interval.If curve is closed, and trim_parameters ins a
    decreasing interval, then the portion of the curve across the
    start/end is returned.
Returns:
  trimmed curve or NULL if input is invalid.
*/
ON_DECL
ON_Curve* ON_TrimCurve(
            const ON_Curve& curve,
            ON_Interval trim_parameters
            );


/*
Description:
  Join all contiguous curves of an array of ON_Curves.
Parameters:
  InCurves - [in] Array of curves to be joined (not modified)
  OutCurves - [out] Resulting joined curves and copies of curves that were not joined to anything
                    are appended.
  join_tol - [in] Distance tolerance used to decide if endpoints are close enough
  bPreserveDirection - [in] If true, curve endpoints will be compared to curve startpoints.
                            If false, all start and endpoints will be compared, and copies of input
                            curves may be reversed in output.
  key     -  [out] if key is not null, InCurves[i] was joined into OutCurves[key[i]].
Returns:
  Number of curves added to Outcurves
Remarks:
  Closed curves are copied to OutCurves.
  Curves that cannot be joined to others are copied to OutCurves.  When curves are joined, the results
  are ON_PolyCurves. All members of InCurves must have same dimension, at most 3.
  */
ON_DECL
int ON_JoinCurves(const ON_SimpleArray<const ON_Curve*>& InCurves,
                  ON_SimpleArray<ON_Curve*>& OutCurves,
                  double join_tol,
                  bool bPreserveDirection = false,
                  ON_SimpleArray<int>* key = 0
                 );


/*
Description:
  Sort a list of lines so they are geometrically continuous.
Parameters:
  line_count - [in] number of lines
  line_list  - [in] array of lines
  index       - [out] The input index[] is an array of line_count unused integers.
                      The returned index[] is a permutation of {0,1,...,line_count-1}
                      so that the list of lines is in end-to-end order.
  bReverse    - [out] The input bReverse[] is an array of line_count unused bools.
                      If the returned value of bReverse[j] is true, then
                      line_list[index[j]] needs to be reversed.
Returns:
  True if successful, false if not.
*/
ON_DECL
bool ON_SortLines(
        int line_count,
        const ON_Line* line_list,
        int* index,
        bool* bReverse
        );

/*
Description:
  Sort a list of lines so they are geometrically continuous.
Parameters:
  line_list  - [in] array of lines
  index       - [out] The input index[] is an array of line_count unused integers.
                      The returned index[] is a permutation of {0,1,...,line_count-1}
                      so that the list of lines is in end-to-end order.
  bReverse    - [out] The input bReverse[] is an array of line_count unused bools.
                      If the returned value of bReverse[j] is true, then
                      line_list[index[j]] needs to be reversed.
Returns:
  True if successful, false if not.
*/
ON_DECL
bool ON_SortLines(
        const ON_SimpleArray<ON_Line>& line_list,
        int* index,
        bool* bReverse
        );

/*
Description:
  Sort a list of open curves so end of a curve matches the start of the next curve.
Parameters:
  curve_count - [in] number of curves
  curve_list  - [in] array of curve pointers
  index       - [out] The input index[] is an array of curve_count unused integers.
                      The returned index[] is a permutation of {0,1,...,curve_count-1}
                      so that the list of curves is in end-to-end order.
  bReverse    - [out] The input bReverse[] is an array of curve_count unused bools.
                      If the returned value of bReverse[j] is true, then
                      curve_list[index[j]] needs to be reversed.
Returns:
  True if successful, false if not.
*/
ON_DECL
bool ON_SortCurves(
          int curve_count,
          const ON_Curve* const* curve_list,
          int* index,
          bool* bReverse
          );

/*
Description:
  Sort a list of curves so end of a curve matches the start of the next curve.
Parameters:
  curve       - [in] array of curves to sort.  The curves themselves are not modified.
  index       - [out] The input index[] is an array of curve_count unused integers.
                      The returned index[] is a permutation of {0,1,...,curve_count-1}
                      so that the list of curves is in end-to-end order.
  bReverse    - [out] The input bReverse[] is an array of curve_count unused bools.
                      If the returned value of bReverse[j] is true, then
                      curve[index[j]] needs to be reversed.
Returns:
  True if successful, false if not.
*/
ON_DECL
bool ON_SortCurves(
                   const ON_SimpleArray<const ON_Curve*>& curves,
                   ON_SimpleArray<int>& index,
                   ON_SimpleArray<bool>& bReverse
                   );

/*
Description:
  Sort a list of curves so end of a curve matches the start of the next curve.
Parameters:
  curve_count - [in] number of curves
  curve       - [in] array of curve pointers
  index       - [out] The input index[] is an array of curve_count unused integers.
                      The returned index[] is a permutation of {0,1,...,curve_count-1}
                      so that the list of curves is in end-to-end order.
  bReverse    - [out] The input bReverse[] is an array of curve_count unused bools.
                      If the returned value of bReverse[j] is true, then
                      curve[index[j]] needs to be reversed.
Returns:
  True if successful, false if not.
*/
ON_DECL
bool ON_SortCurves(
          const ON_SimpleArray<ON_Curve*>& curves,
          ON_SimpleArray<int>& index,
          ON_SimpleArray<bool>& bReverse
          );


#define CLOSETO_CHORD_TOL 1e-1
#define CLOSETO_DER_TOL 0.75 // the min value of the product of the dot products

class Sample {
public:
  const ON_Curve* c;
  ON_3dPoint pt;
  ON_3dVector tangent;
  double t;
  double dist;
  double ray_t;

  Sample() {}

  Sample(const ON_Curve* curve, double param) : c(curve), t(param), dist(0.0) {
    c->Ev1Der(t, pt, tangent);
    assert(tangent.Unitize());
  }
  Sample(const Sample& s) :
    c(s.c), pt(s.pt), tangent(s.tangent), t(s.t), dist(s.dist) {}
  Sample& operator=(const Sample& s) {
    c = s.c;
    pt = s.pt;
    tangent = s.tangent;
    t = s.t;
    dist = s.dist;
    return *this;
  }

  bool operator<(const Sample& s) {
    //    return (ON_NearZero(dist-s.dist,ON_ZERO_TOLERANCE)) ? t < s.t : dist < s.dist;
    return dist < s.dist;
  }
};


#endif
