// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
  const zSTRING BIP01_HEAD_NAME = "BIP01 HEAD";


  Array<zTSimpleMesh*> zCModel::GetMeshList() {
    Array<zMAT4*> trafoList;
    for( int i = 0; i < nodeList.GetNum(); i++ ) {
      nodeList[i]->trafoObjToCam = GetTrafoNodeToModel( nodeList[i] );
      trafoList.Insert( &nodeList[i]->trafoObjToCam );
    }

    Array<zTSimpleMesh*> meshList;
    for( int i = 0; i < meshSoftSkinList.GetNum(); i++ ) {
      auto& softSkin = meshSoftSkinList[i];
      meshList.Insert( softSkin->GetMesh( trafoList ) );
    }

    return meshList;
  }


  int zCModel::TraceRaySoftskin( const zVEC3& start, const zVEC3& ray, int flags, zTTraceRayReport& report ) {
    int hitFound = False;
    auto meshList = GetMeshList();
    if( (flags & zTRACERAY_FIRSTHIT) == zTRACERAY_FIRSTHIT ) {
      for( uint i = 0; i < meshList.GetNum(); i++ ) {
        auto mesh = meshList[i];
        if( mesh->TraceRay( start, ray, flags, report ) ) {
          if( report.foundPoly ) {
            (int&)report.foundVertex = report.foundPoly->flags.sectorIndex;
            report.foundPoly = Null;
          }
          hitFound = True;
          break;
        }
      }
    }
    else {
      float nearest = float_MAX;
      for( uint i = 0; i < meshList.GetNum(); i++ ) {
        auto mesh = meshList[i];
        zTTraceRayReport reportTmp;
        if( mesh->TraceRay( start, ray, flags, reportTmp ) ) {
          zVEC3 hitPos = reportTmp.foundIntersection;
          float distance = start.Distance( hitPos );
          if( i == 0 || distance < nearest ) {
            nearest = distance;
            report = reportTmp;
            if( report.foundPoly ) {
              (int&)report.foundVertex = report.foundPoly->flags.sectorIndex;
              report.foundPoly = Null;
            }
            hitFound = True;
          }
        }
      }
    }

    meshList.DeleteData();
    return hitFound;
  }


  int zCModel::TraceRaySoftskin( const zMAT4& vobTrafo, const zVEC3& start, const zVEC3& ray, int flags, zTTraceRayReport& report ) {
    zVEC3 startLocal = vobTrafo.InverseLinTrafo() * start;
    zVEC3 rayLocal = vobTrafo.Transpose() * ray;
    int ok = TraceRaySoftskin( startLocal, rayLocal, flags, report );
    if( ok )
      report.foundIntersection = vobTrafo * report.foundIntersection;

    return ok;
  }


  void zCModel::ShowPolygons( const zMAT4& trafo, const zCOLOR& color ) {
    auto meshList = GetMeshList();
    for( uint i = 0; i < meshList.GetNum(); i++ ) {
      auto mesh = meshList[i];
      for( int p = 0; p < mesh->Polygons.GetNum(); p++ ) {
        zTSimplePolygon* poly = mesh->Polygons[p];
        zVEC3 vertA = trafo * poly->Triangle[VA];
        zVEC3 vertB = trafo * poly->Triangle[VB];
        zVEC3 vertC = trafo * poly->Triangle[VC];
        zlineCache->Line3D( vertA, vertB, color, 0 );
        zlineCache->Line3D( vertB, vertC, color, 0 );
        zlineCache->Line3D( vertC, vertA, color, 0 );
      }

      delete mesh;
    }
  }


  HOOK Hook_zCModel_CalcModelBBox3DWorld PATCH( &zCModel::CalcModelBBox3DWorld, &zCModel::CalcModelBBox3DWorld_Union );

  void zCModel::CalcModelBBox3DWorld_Union() {
    THISCALL( Hook_zCModel_CalcModelBBox3DWorld )();
    zCModelNodeInst* node = SearchNode( BIP01_HEAD_NAME );
    if( !node || !node->nodeVisual )
      return;

    zMAT4 trafo = GetTrafoNodeToModel( node );
    zTBBox3D bboxHead = node->nodeVisual->GetBBox3D();
    bboxHead.Transform( trafo, bboxHead );
    bbox3D.AddPoint( bboxHead.mins );
    bbox3D.AddPoint( bboxHead.maxs );
  }
}