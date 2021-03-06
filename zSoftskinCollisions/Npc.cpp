// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
  enum {
    zTRACERAY_NPC_SOFTSKIN = 1 << 26
  };


  int GetModelNodeId( zCModel* model, const zSTRING& nodeName ) {
    for( int i = 0; i < model->nodeList.GetNum(); i++ )
      if( model->nodeList[i]->protoNode->nodeName.Compare( nodeName ) )
        return i;
    
    return Invalid;
  }


  int GetModelNodeId( zCModel* model, zCModelNodeInst* node ) {
    for( int i = 0; i < model->nodeList.GetNum(); i++ )
      if( model->nodeList[i] == node )
        return i;
    
    return Invalid;
  }


  int oCNpc::TraceRay_Union( zVEC3 const& start, zVEC3 const& ray, int flags, zTTraceRayReport& report ) {
    if( HasFlag( flags, zTRACERAY_NPC_SOFTSKIN ) ) {
      int hitFound = False;
      if( TraceRayNodes( start, ray, flags, report ) ) {
        hitFound = True;
        if( HasFlag( flags, zTRACERAY_FIRSTHIT ) )
          return True;
      }

      zTTraceRayReport reportTmp;
      if( TraceRaySoftskin( start, ray, flags, reportTmp ) ) {
        if( !hitFound || start.Distance( reportTmp.foundIntersection ) < start.Distance( report.foundIntersection ) ) {
          report = reportTmp;
          return True;
        }
      }

      return hitFound;
    }

    return oCVob::TraceRay(start, ray, flags, report);
  }


  int oCNpc::TraceRaySoftskin( const zVEC3& start, const zVEC3& ray, int flags, zTTraceRayReport& report ) {
    if( !zCVob::TraceRay( start, ray, zTRACERAY_VOB_BBOX, report ) )
      return False;

    zCModel* model = GetModel();
    if( !model )
      return False;

    int ok = model->TraceRaySoftskin( trafoObjToWorld, start, ray, flags, report );
    if( ok )
      report.foundVob = this;

    return ok;
  }


  inline zTSimpleMeshList GetMeshListFromVisual( zCVisual* visual ) {
    zCMorphMesh* mesh = visual->CastTo<zCMorphMesh>();
    if( mesh )
      return mesh->morphMesh->GetMeshList( Null );

    zCProgMeshProto* meshProto = visual->CastTo<zCProgMeshProto>();
    if( meshProto )
      return meshProto->GetMeshList( Null );

    return zTSimpleMeshList();
  }


  int oCNpc::TraceRayNode( zCModelNodeInst* node, const zVEC3& start, const zVEC3& ray, int flags, zTTraceRayReport& report ) {
    zCVisual* visual = node->nodeVisual;
    if( !visual )
      return False;

    if( node->protoNode->nodeName.StartWith( ZS_PREFIX ) )
      return False;

    zMAT4 trafo      = GetTrafoModelNodeToWorld( node );
    zVEC3 startLocal = trafo.InverseLinTrafo() * start;
    zVEC3 rayLocal   = trafo.Transpose() * ray;
    zTBBox3D& bbox   = visual->GetBBox3D();
    zVEC3 intersection;
    if( !bbox.TraceRay( startLocal, rayLocal, intersection ) )
      return False;

    zTSimpleMeshList meshList = GetMeshListFromVisual( visual );
    if( meshList.IsEmpty() ) {
      int ok = visual->TraceRay( startLocal, rayLocal, flags, report );
      if( ok )
        report.foundVob = this;

      return ok;
    }

    zTSimpleMesh* mesh = meshList.GetFirst();
    if( !mesh )
      return False;

    int ok = mesh->TraceRay( startLocal, rayLocal, flags, report );
    if( ok ) {
      report.foundVob = this;
      report.foundPoly = Null;
      report.foundIntersection = trafo * report.foundIntersection;
      (int&)report.foundVertex = GetModelNodeId( GetModel(), node );
    }

    return ok;
  }


  int oCNpc::TraceRayNodes( const zVEC3& start, const zVEC3& ray, int flags, zTTraceRayReport& report ) {
    zCModel* model = GetModel();
    if( !model )
      return False;

    int hitFound = False;
    float bestDistance = float_MAX;
    zTTraceRayReport reportTmp;
    for( int i = 0; i < model->nodeList.GetNum(); i++ ) {
      zCModelNodeInst* node = model->nodeList[i];
      int ok = TraceRayNode( node, start, ray, flags, reportTmp );
      if( !ok )
        continue;

      if( HasFlag( flags, zTRACERAY_FIRSTHIT ) ) {
        report = reportTmp;
        return True;
      }

      float distance = start.Distance( reportTmp.foundIntersection );
      if( distance >= bestDistance )
        continue;

      bestDistance = distance;
      report = reportTmp;
      hitFound = True;
    }

    return hitFound;
  }
}