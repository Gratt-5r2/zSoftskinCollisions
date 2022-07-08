// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
  const zSTRING BIP01_HEAD_NAME = "BIP01 HEAD";
  const zSTRING ZS_PREFIX = "ZS_";


  zTSimpleMeshList zCModel::GetMeshList() {
    auto& pair = MeshFrameCache[this];
    if( !pair.IsNull() )
      return pair;

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

    MeshFrameCache.Insert( this, meshList );
    return meshList;
  }


  int zCModel::TraceRaySoftskin( const zVEC3& start, const zVEC3& ray, int flags, zTTraceRayReport& report ) {
    int hitFound = False;
    zTSimpleMeshList meshList = GetMeshList();
    if( HasFlag( flags, zTRACERAY_FIRSTHIT ) ) {
      for( uint i = 0; i < meshList.GetNum(); i++ ) {
        zTSimpleMesh* mesh = meshList[i];
        if( mesh->TraceRay( start, ray, flags, report ) ) {
          hitFound = True;
          break;
        }
      }
    }
    else {
      float nearest = float_MAX;
      for( uint i = 0; i < meshList.GetNum(); i++ ) {
        zTSimpleMesh* mesh = meshList[i];
        zTTraceRayReport reportTmp;
        if( mesh->TraceRay( start, ray, flags, reportTmp ) ) {
          zVEC3 hitPos = reportTmp.foundIntersection;
          float distance = start.Distance( hitPos );
          if( distance < nearest ) {
            nearest = distance;
            report = reportTmp;
            hitFound = True;
          }
        }
      }
    }

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
      for( uint p = 0; p < mesh->Polygons.GetNum(); p++ ) {
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
    for( int i = 0; i < nodeList.GetNum(); i++ ) {
      zCModelNodeInst* node = nodeList[i];
      if( !node->nodeVisual )
        continue;

      if( node->protoNode->nodeName.StartWith( ZS_PREFIX ) )
        continue;

      zMAT4 trafo = GetTrafoNodeToModel( node );
      zTBBox3D bbox3D = node->nodeVisual->GetBBox3D();
      bbox3D.Transform( trafo, bbox3D );
      bbox3D.AddPoint( bbox3D.mins );
      bbox3D.AddPoint( bbox3D.maxs );
    }
  }
}