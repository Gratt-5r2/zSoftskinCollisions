// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
  void zCPolygon::SetTriangleData( const zVEC3* triData ) {
    vertex[VA]->position = triData[VA];
    vertex[VB]->position = triData[VB];
    vertex[VC]->position = triData[VC];
  }


  void zCPolygon::GetTriangleData( zVEC3* triData ) {
    triData[VA] = vertex[VA]->position;
    triData[VB] = vertex[VB]->position;
    triData[VC] = vertex[VC]->position;
  }


  void zCPolygon::SetPlaneData( const zVEC3& normal, const float& distance ) {
    polyPlane.normal = normal;
    polyPlane.distance = distance;
  }


  void zCPolygon::GetPlaneData( zVEC3& normal, float& distance ) {
    normal = polyPlane.normal;
    distance = polyPlane.distance;
  }


  inline zCPolygon* CreateTriangle() {
    zCPolygon* poly = new zCPolygon();
    poly->AllocVerts( 3 );
    poly->vertex[VA] = new zCVertex();
    poly->vertex[VB] = new zCVertex();
    poly->vertex[VC] = new zCVertex();
    return poly;
  }

  static zCPolygon* TriangleTmp = CreateTriangle();


  void zTSimplePolygon::CalcNormal() {
    TriangleTmp->SetTriangleData( Triangle );
    TriangleTmp->CalcNormal();
    TriangleTmp->GetPlaneData( Normal, Distance );
  }


  int zTSimplePolygon::TraceRay( const zVEC3& start, const zVEC3& ray, zVEC3& intersection, float& distance, const int& flags ) {
    TriangleTmp->SetTriangleData( Triangle );
    TriangleTmp->SetPlaneData( Normal, Distance );

    int hitFound = (flags & zTRACERAY_POLY_2SIDED) == zTRACERAY_POLY_2SIDED ?
      TriangleTmp->CheckRayPolyIntersection2Sided( start, ray, intersection, distance ) :
      TriangleTmp->CheckRayPolyIntersection      ( start, ray, intersection, distance );

    return hitFound;
  }


  zTSimplePolygon* zTSimpleMesh::AddTriangle( const zVEC3& a, const zVEC3& b, const zVEC3& c ) {
		zTSimplePolygon* poly = new zTSimplePolygon();
		poly->Triangle[VA] = a;
		poly->Triangle[VB] = b;
		poly->Triangle[VC] = c;
    poly->CalcNormal();
    Polygons.Insert( poly );
    return poly;
	}


	int zTSimpleMesh::TraceRay( const zVEC3& start, const zVEC3& ray, const int& flags, zTTraceRayReport& report ) {
    report.foundHit = False;
    if( (flags & zTRACERAY_FIRSTHIT) == zTRACERAY_FIRSTHIT ) {
      zVEC3 intersection;
      for( uint i = 0; i < Polygons.GetNum(); i++ ) {
        float distance;
        zTSimplePolygon* poly = Polygons[i];
        if( poly->TraceRay( start, ray, intersection, distance, flags ) ) {
          report.foundHit = True;
          report.foundIntersection = intersection;
          report.foundPolyNormal = poly->Normal;
          (int&)report.foundVertex = poly->NodeId;
          return True;
        }
      }
    }
    else {
      float bestDistance = float_MAX;
      zVEC3 intersection;
      for( uint i = 0; i < Polygons.GetNum(); i++ ) {
        float distance;
        zTSimplePolygon* poly = Polygons[i];
        if( poly->TraceRay( start, ray, intersection, distance, flags ) ) {
          if( distance < bestDistance ) {
            bestDistance = distance;
            report.foundHit = True;
            report.foundIntersection = intersection;
            report.foundPolyNormal = poly->Normal;
            (int&)report.foundVertex = poly->NodeId;
          }
        }
      }
    }

    return report.foundHit;
	}


	zTSimpleMesh::~zTSimpleMesh() {
		Polygons.DeleteData();
	}
}