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

#include "opennurbs.h"

ON_OBJECT_IMPLEMENT(ON_LineCurve,ON_Curve,"4ED7D4DB-E947-11d3-BFE5-0010830122F0");

ON_LineCurve::ON_LineCurve()
{
  m_line.from.Zero();
  m_line.to.Zero();
  m_t.m_t[0] = 0.0;
  m_t.m_t[0] = 1.0;
  m_dim = 3;
}

ON_LineCurve::ON_LineCurve(const ON_2dPoint& a,const ON_2dPoint& b) : m_line(a,b), m_dim(2)
{
  double len = m_line.Length();
  if ( len <= ON_ZERO_TOLERANCE )
    len = 1.0;
  m_t.Set(0.0,len);
}

ON_LineCurve::ON_LineCurve(const ON_3dPoint& a,const ON_3dPoint& b) : m_line(a,b), m_dim(3)
{
  double len = m_line.Length();
  if ( len <= ON_ZERO_TOLERANCE )
    len = 1.0;
  m_t.Set(0.0,len);
}


ON_LineCurve::ON_LineCurve( const ON_Line& L ) : m_line(L), m_dim(3)
{
  double len = m_line.Length();
  if ( len <= ON_ZERO_TOLERANCE )
    len = 1.0;
  m_t.Set(0.0,len);
}

ON_LineCurve::ON_LineCurve( const ON_Line& L, double t0, double t1 ) : m_line(L), m_t(t0,t1), m_dim(3)
{
}

ON_LineCurve::ON_LineCurve( const ON_LineCurve& src )
{
  *this = src;
}

ON_LineCurve::~ON_LineCurve()
{
}

unsigned int ON_LineCurve::SizeOf() const
{
  unsigned int sz = ON_Curve::SizeOf();
  sz += (sizeof(*this) - sizeof(ON_Curve));
  return sz;
}

ON__UINT32 ON_LineCurve::DataCRC(ON__UINT32 current_remainder) const
{
  current_remainder = ON_CRC32(current_remainder,sizeof(m_line),&m_line);
  current_remainder = ON_CRC32(current_remainder,sizeof(m_t),&m_t);
  current_remainder = ON_CRC32(current_remainder,sizeof(m_dim),&m_dim);

  return current_remainder;
}

ON_LineCurve& ON_LineCurve::operator=( const ON_LineCurve& src )
{
  if ( this != &src ) {
    ON_Curve::operator=(src);
    m_line = src.m_line;
    m_t = src.m_t;
    m_dim  = src.m_dim;
  }
  return *this;
}

ON_LineCurve& ON_LineCurve::operator=( const ON_Line& L )
{
  m_line = L;
  m_t.m_t[0] = 0.0;
  m_t.m_t[1] = L.Length();
  if ( m_t.m_t[1] == 0.0 )
    m_t.m_t[1] = 1.0;
  m_dim = 3;
  return *this;
}

int ON_LineCurve::Dimension() const
{
  return m_dim;
}

BOOL
ON_LineCurve::GetBBox( // returns TRUE if successful
         double* boxmin,    // minimum
         double* boxmax,    // maximum
         BOOL bGrowBox
         ) const
{
  return ON_GetPointListBoundingBox( m_dim, FALSE, 2, 3, m_line.from,
                      boxmin, boxmax, bGrowBox?true:false
                      );
}

BOOL
ON_LineCurve::Transform( const ON_Xform& xform )
{
  TransformUserData(xform);
	DestroyCurveTree();
  return m_line.Transform( xform );
}

bool ON_LineCurve::IsDeformable() const
{
  return true;
}

bool ON_LineCurve::MakeDeformable()
{
  return true;
}


BOOL
ON_LineCurve::SwapCoordinates( int i, int j )
{
  BOOL rc = FALSE;
  if ( i >= 0 && i < 3 && j >= 0 && j < 3 && i != j ) {
    double t = m_line.from[i];
    m_line.from[i] = m_line.from[j];
    m_line.from[j] = t;
    t = m_line.to[i];
    m_line.to[i] = m_line.to[j];
    m_line.to[j] = t;
    rc = TRUE;
  }
  return rc;
}

BOOL ON_LineCurve::IsValid( ON_TextLog* text_log ) const
{
  return ( m_t[0] < m_t[1] && m_line.Length() > 0.0 ) ? TRUE : FALSE;
}

void ON_LineCurve::Dump( ON_TextLog& dump ) const
{
  dump.Print( "ON_LineCurve:  domain = [%g,%g]\n",m_t[0],m_t[1]);
  dump.PushIndent();
  dump.Print( "start = ");
  dump.Print( m_line.from );
  dump.Print( "\nend = ");
  dump.Print( m_line.to );
  dump.Print( "\n");
  dump.Print( "length = %g\n",m_line.Length());
  dump.PopIndent();
}

BOOL ON_LineCurve::Write(
       ON_BinaryArchive& file // open binary file
     ) const
{
  BOOL rc = file.Write3dmChunkVersion(1,0);
  if (rc) {
    rc = file.WriteLine( m_line );
    if (rc) rc = file.WriteInterval( m_t );
    if (rc) rc = file.WriteInt(m_dim);
  }
  return rc;
}

BOOL ON_LineCurve::Read(
       ON_BinaryArchive& file // open binary file
     )
{
  int major_version = 0;
  int minor_version = 0;
  BOOL rc = file.Read3dmChunkVersion(&major_version,&minor_version);
  if (rc && major_version==1) {
    // common to all 1.x versions
    rc = file.ReadLine( m_line );
    if (rc) rc = file.ReadInterval( m_t );
    if (rc) rc = file.ReadInt(&m_dim);
  }
  return rc;
}

ON_Interval ON_LineCurve::Domain() const
{
  return m_t;
}

BOOL ON_LineCurve::SetDomain( double t0, double t1)
{
  if (t0 < t1)
  {
    m_t.Set(t0, t1);
		DestroyCurveTree();
    return TRUE;
  }
  return FALSE;
}

bool ON_LineCurve::ChangeDimension( int desired_dimension )
{
  bool rc = (desired_dimension>=2 && desired_dimension<=3);

  if (rc && m_dim != desired_dimension )
  {
    DestroyCurveTree();
    if ( desired_dimension == 2 )
    {
      // 7 April 2003 Dale Lear - zero z coords if x coord are set
      if( ON_UNSET_VALUE != m_line.from.x )
        m_line.from.z = 0.0;
      if( ON_UNSET_VALUE != m_line.to.x )
        m_line.to.z = 0.0;
      m_dim = 2;
    }
    else
    {
      if ( 2 == m_dim  )
      {
        // 7 April 2003 Dale Lear
        // zero z coords if x coords are set and z coords are not set
        if( ON_UNSET_VALUE != m_line.from.x && ON_UNSET_VALUE == m_line.from.z )
          m_line.from.z = 0.0;
        if( ON_UNSET_VALUE != m_line.from.x && ON_UNSET_VALUE == m_line.to.z )
          m_line.from.z = 0.0;
      }
      m_dim = 3;
    }
  }

  return rc;
}


int ON_LineCurve::SpanCount() const
{
  return 1;
}

BOOL ON_LineCurve::GetSpanVector( // span "knots"
       double* s
       ) const
{
  s[0] = m_t[0];
  s[1] = m_t[1];
  return m_t.IsIncreasing();
}

int ON_LineCurve::Degree() const
{
  return 1;
}


BOOL
ON_LineCurve::IsLinear(  // TRUE if curve locus is a line segment
      double tolerance   // tolerance to use when checking linearity
      ) const
{
  return IsValid();
}

int ON_LineCurve::IsPolyline(
      ON_SimpleArray<ON_3dPoint>* pline_points,
      ON_SimpleArray<double>* pline_t
      ) const
{
  int rc = 0;
  if ( pline_points )
    pline_points->SetCount(0);
  if ( pline_t )
    pline_t->SetCount(0);
  if ( IsValid() )
  {
    rc = 2;
    if ( pline_points )
    {
      pline_points->Reserve(2);
      pline_points->Append( m_line.from );
      pline_points->Append( m_line.to );
    }
    if ( pline_t )
    {
      pline_t->Reserve(2);
      pline_t->Append( m_t[0] );
      pline_t->Append( m_t[1] );
    }
  }
  return rc;
}


BOOL
ON_LineCurve::IsArc( // TRUE if curve locus in an arc or circle
      const ON_Plane* plane, // if not NULL, test is performed in this plane
      ON_Arc* arc,         // if not NULL and TRUE is returned, then arc
                              // arc parameters are filled in
      double tolerance // tolerance to use when checking linearity
      ) const
{
  return FALSE;
}

BOOL
ON_LineCurve::IsPlanar(
      ON_Plane* plane, // if not NULL and TRUE is returned, then plane parameters
                         // are filled in
      double tolerance // tolerance to use when checking linearity
      ) const
{
  BOOL rc = IsValid();
  if ( plane != NULL && rc )
  {
    if ( m_dim == 2 )
      rc = ON_Curve::IsPlanar(plane,tolerance);
    else if ( !m_line.InPlane(*plane,tolerance) )
      m_line.InPlane(*plane,0.0);
  }
  return rc;
}

BOOL
ON_LineCurve::IsInPlane(
      const ON_Plane& plane, // plane to test
      double tolerance // tolerance to use when checking linearity
      ) const
{
  BOOL rc = FALSE;
  double d = fabs( plane.DistanceTo( PointAtStart() ));
  if ( d <= tolerance ) {
    d = fabs( plane.DistanceTo( PointAtEnd() ));
    if ( d <= tolerance )
      rc = TRUE;
  }
  return rc;
}

BOOL
ON_LineCurve::IsClosed() const
{
  return FALSE;
}

BOOL
ON_LineCurve::IsPeriodic() const
{
  return FALSE;
}

BOOL
ON_LineCurve::Reverse()
{
  const ON_3dPoint p = m_line.from;
  m_line.from = m_line.to;
  m_line.to = p;
  m_t.Reverse();
	DestroyCurveTree();
  return TRUE;
}

BOOL ON_LineCurve::Evaluate( // returns FALSE if unable to evaluate
       double t,       // evaluation parameter
       int der_count,  // number of derivatives (>=0)
       int v_stride,   // v[] array stride (>=Dimension())
       double* v,      // v[] array of length stride*(ndir+1)
       int side,       // optional - determines which side to evaluate from
                       //         0 = default
                       //      <  0 to evaluate from below,
                       //      >  0 to evaluate from above
       int* hint       // optional - evaluation hint (int) used to speed
                       //            repeated evaluations
       ) const
{
  BOOL rc = FALSE;
  if ( m_t[0] < m_t[1] ) {
    double s = (t == m_t[1]) ? 1.0 : (t-m_t[0])/(m_t[1]-m_t[0]);
    const ON_3dPoint p = m_line.PointAt(s);
    v[0] = p.x;
    v[1] = p.y;
    if ( m_dim == 3 )
      v[2] = p.z;
    if ( der_count >= 1 )
    {
      v += v_stride;
      ON_3dVector d = m_line.to - m_line.from;
      double dt = m_t[1] - m_t[0];
      v[0] = d.x/dt;
      v[1] = d.y/dt;
      if ( m_dim == 3 )
        v[2] = d.z/dt;
      for ( int di = 2; di <= der_count; di++ ) {
        v += v_stride;
        v[0] = 0.0;
        v[1] = 0.0;
        if ( m_dim == 3 )
          v[2] = 0.0;
      }
    }
    rc = TRUE;
  }
  return rc;
}

bool ON_LineCurve::GetClosestPoint( const ON_3dPoint& test_point,
        double* t,       // parameter of local closest point returned here
        double maximum_distance,
        const ON_Interval* sub_domain
        ) const
{
  bool rc = false;
  double s, x, d;
  ON_3dVector Q, P1;

  Q.x = test_point.x - m_line.from.x;
  Q.y = test_point.y - m_line.from.y;
  Q.z = test_point.z - m_line.from.z;

  P1.x = m_line.to.x - m_line.from.x;
  P1.y = m_line.to.y - m_line.from.y;
  P1.z = m_line.to.z - m_line.from.z;

  s = P1.x*P1.x + P1.y*P1.y + P1.z*P1.z;
  if ( 0.0 != s )
  {
    s = (Q.x*P1.x + Q.y*P1.y + Q.z*P1.z)/s;
    if ( s <= 0.0 )
      s = 0.0;
    else if ( s > 1.0 )
      s = 1.0;

    x = (1.0-s)*m_t[0] + s*m_t[1];

    if ( sub_domain )
    {
      if ( x < sub_domain->m_t[0]  )
      {
        if ( m_t[1] < sub_domain->m_t[0] )
          return false;
        x = sub_domain->m_t[0];
        s = (x - m_t[0])/(m_t[1] - m_t[0]);
      }
      else if ( x > sub_domain->m_t[1] )
      {
        if ( m_t[0] > sub_domain->m_t[1] )
          return false;
        x = sub_domain->m_t[1];
        s = (x - m_t[0])/(m_t[1] - m_t[0]);
      }
    }

    if ( maximum_distance > 0.0 )
    {
      d = 1.0-s;
      Q.x = d*m_line.from.x + s*m_line.to.x - test_point.x;
      Q.y = d*m_line.from.y + s*m_line.to.y - test_point.y;
      Q.z = d*m_line.from.z + s*m_line.to.z - test_point.z;
      d = Q.Length();
      if ( d > maximum_distance )
        return false;
    }

    *t = x;

    rc = true;
  }
  return rc;
}

BOOL ON_LineCurve::GetLocalClosestPoint( const ON_3dPoint& test_point,
        double seed_parameter,
        double* t,
        const ON_Interval* sub_domain
        ) const
{
  if ( sub_domain )
  {
    if ( seed_parameter < sub_domain->Min() )
      seed_parameter = sub_domain->Min();
    else if ( seed_parameter > sub_domain->Max() )
      seed_parameter = sub_domain->Max();
  }
  BOOL rc = GetClosestPoint( test_point, t, 0.0, sub_domain );
  if ( rc
       && t
       && seed_parameter != *t
       && test_point.DistanceTo(PointAt(seed_parameter)) <= test_point.DistanceTo(PointAt(*t))
       )
  {
    *t = seed_parameter;
  }
  return rc;
}

int ON_LineCurve::NumIntersectionsWith( const ON_Line& segment ) const
{
  // segment is assumed to be horizontal at this point
  // and first point is the "left" point
  if (((m_line[0].y < segment[0].y) && (m_line[1].y < segment[0].y)) ||
      ((m_line[0].y > segment[0].y) && (m_line[1].y > segment[0].y)) ||
      (m_line[0].x < segment[0].x && m_line[1].x < segment[0].x)) {
    return 0;
  } else if (m_line[0].x > segment[0].x && m_line[1].x > segment[0].x) {
    return 1;
  } else {
    // use the two point form of a line to solve for x at the y of the segment
    double x =
      (segment[0].y-m_line[0].y)*(m_line[1].x-m_line[0].x) /
      (m_line[1].y-m_line[0].y) +
      m_line[0].x;
    if (x < segment[0].x) return 0;
    else return 1;
  }
}

BOOL ON_LineCurve::GetLength(
        double* length,               // length returned here
        double fractional_tolerance,  // default = 1.0e-8
        const ON_Interval* sub_domain // default = NULL
        ) const
{
  // override when possible
  // TODO: add simple integration routine that works with C1 curves
	if ( sub_domain && sub_domain->IsDecreasing())
		return FALSE;
  else if ( sub_domain ) {
		ON_Interval scratch_domain= m_t;
		if(!scratch_domain.Intersection(*sub_domain))
			return FALSE;
		else
			sub_domain= &scratch_domain;
    *length = PointAt(sub_domain->Min()).DistanceTo( PointAt(sub_domain->Max()) );
  }
  else {
    *length = m_line.Length();
  }
  return TRUE;
}

BOOL ON_LineCurve::GetNormalizedArcLengthPoint(
        double s,
        double* t,
        double fractional_tolerance,
        const ON_Interval* sub_domain
        ) const
{
  ON_Interval domain = (sub_domain) ? *sub_domain : Domain();
  if ( t )
    *t = domain.ParameterAt(s);
  return TRUE;
}

BOOL ON_LineCurve::GetNormalizedArcLengthPoints(
        int count,
        const double* s,
        double* t,
        double absolute_tolerance,
        double fractional_tolerance,
        const ON_Interval* sub_domain
        ) const
{
  if ( count > 0 || s != NULL && t != NULL )
  {
    if ( !sub_domain )
      sub_domain = &m_t;
    int i;
    for ( i = 0; i < count; i++ )
    {
      t[i] = sub_domain->ParameterAt( s[i] );
    }
  }
  return TRUE;
}

BOOL ON_LineCurve::SetStartPoint(ON_3dPoint start_point)
{
  m_line.from = start_point;
	DestroyCurveTree();
  return true;
}

BOOL ON_LineCurve::SetEndPoint(ON_3dPoint end_point)
{
  m_line.to = end_point;
	DestroyCurveTree();
  return true;
}


int ON_LineCurve::GetNurbForm(
      ON_NurbsCurve& c,
      double tolerance,
      const ON_Interval* subdomain
      ) const
{
  int rc = 0;
  if ( c.Create( m_dim==2?2:3, FALSE, 2, 2 ) )
  {
    rc = 1;
    double t0 = m_t[0];
    double t1 = m_t[1];
    if (subdomain )
    {
      if ( t0 < t1 )
      {
        const ON_Interval& sd = *subdomain;
        double s0 = sd[0];
        double s1 = sd[1];
        if (s0 < t0) s0 = t0;
        if (s1 > t1) s1 = t1;
        if (s0 < s1)
        {
          t0 = s0;
          t1 = s1;
        }
        else
          rc = 0;
      }
      else
      {
        rc = 0;
      }
    }
    if ( t0 < t1 )
    {
      c.m_knot[0] = t0;
      c.m_knot[1] = t1;
      c.SetCV( 0, PointAt(t0));
      c.SetCV( 1, PointAt(t1));
    }
    else if ( t0 > t1 )
    {
      rc = 0;
      c.m_knot[0] = t1;
      c.m_knot[1] = t0;
      c.SetCV( 0, PointAt(t1));
      c.SetCV( 1, PointAt(t0));
    }
    else
    {
      rc = 0;
      c.m_knot[0] = 0.0;
      c.m_knot[1] = 1.0;
      c.SetCV( 0, m_line.from );
      c.SetCV( 1, m_line.to );
    }
  }
  return rc;
}

int ON_LineCurve::HasNurbForm() const

{
  if (!IsValid())
    return 0;
  return 1;
}



BOOL ON_LineCurve::Trim( const ON_Interval& domain )
{
  BOOL rc = FALSE;
  if ( domain.IsIncreasing() )
  {
    DestroyCurveTree();
    ON_3dPoint p = PointAt( domain[0] );
    ON_3dPoint q = PointAt( domain[1] );
		if( p.DistanceTo(q)>0){								// 2 April 2003 Greg Arden A successfull trim
																					// should return an IsValid ON_LineCurve .
			m_line.from = p;
			m_line.to = q;
			m_t = domain;
			rc = TRUE;
		}
  }
	DestroyCurveTree();
  return rc;
}


bool ON_LineCurve::Extend(
  const ON_Interval& domain
  )

{
  double len = Domain().Length();
  ON_3dVector V = m_line.Direction();
  ON_3dPoint Q0 = m_line.from;
  ON_3dPoint Q1 = m_line.to;
  double t0 = Domain()[0];
  double t1 = Domain()[1];
  bool do_it = false;
  if (domain[1] > Domain()[1]) {
    Q1 += (domain[1]-Domain()[1])/len*V;
    t1 = domain[1];
    do_it = true;
  }
  if (domain[0] < Domain()[0]) {
    Q0 += (domain[0]-Domain()[0])/len*V;
    t0 = domain[0];
    do_it = true;
  }

  if (do_it){
    m_line = ON_Line(Q0, Q1);
    SetDomain(t0, t1);
    DestroyCurveTree();
  }
  return do_it;
}

BOOL ON_LineCurve::Split(
      double t,
      ON_Curve*& left_side,
      ON_Curve*& right_side
    ) const

{
  BOOL rc = FALSE;
  if ( m_t.Includes(t,TRUE) )
  {
    const int dim = m_dim;
    double t0 = m_t[0];
    double t1 = m_t[1];
    ON_Line left, right;
    left.from = m_line.from;
    left.to = m_line.PointAt(m_t.NormalizedParameterAt(t));
    right.from = left.to;
    right.to = m_line.to;

		// 27 March 2003, Greg Arden.  Result must pass IsValid()
		if( left.Length()==0 || right.Length()==0)
			return FALSE;

    ON_LineCurve* left_line = ON_LineCurve::Cast(left_side);
    ON_LineCurve* right_line = ON_LineCurve::Cast(right_side);
    if ( left_side && !left_line )
    {
      ON_ERROR("ON_LineCurve::Split - input left_side not an ON_LineCurve*");
      return FALSE;
    }
    if ( right_side && !right_line )
    {
      ON_ERROR("ON_LineCurve::Split - input right_side not an ON_LineCurve*");
      return FALSE;
    }
    if ( !left_line )
    {
      left_line = new ON_LineCurve();
      left_side = left_line;
    }
    if ( !right_line )
    {
      right_line = new ON_LineCurve();
      right_side = right_line;
    }

    left_line->DestroyCurveTree();
    left_line->m_line = left;
    left_line->m_t.Set( t0, t );
    left_line->m_dim = dim;

    right_line->DestroyCurveTree();
    right_line->m_line = right;
    right_line->m_t.Set( t, t1 );
    right_line->m_dim = dim;

    rc = TRUE;
  }
  return rc;
}

BOOL ON_LineCurve::GetCurveParameterFromNurbFormParameter(
      double nurbs_t,
      double* curve_t
      ) const
{
  *curve_t = nurbs_t;
  return TRUE;
}

BOOL ON_LineCurve::GetNurbFormParameterFromCurveParameter(
      double curve_t,
      double* nurbs_t
      ) const
{
  *nurbs_t = curve_t;
  return TRUE;
}
