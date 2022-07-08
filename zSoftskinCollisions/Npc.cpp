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


  int oCNpc::TraceRay_Union( zVEC3 const& start, zVEC3 const& ray, int flags, zTTraceRayReport& report ) {
    if( (flags & zTRACERAY_NPC_SOFTSKIN) == zTRACERAY_NPC_SOFTSKIN ) {
      int hitFound = False;
      if( TraceRayHead( start, ray, flags, report ) ) {
        hitFound = True;
        if( (flags & zTRACERAY_FIRSTHIT) == zTRACERAY_FIRSTHIT )
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


  int oCNpc::TraceRayHead( const zVEC3& start, const zVEC3& ray, int flags, zTTraceRayReport& report ) {
    zCModel* model = GetModel();
    if( !model )
      return False;

    zCModelNodeInst* headNode = model->SearchNode( BIP01_HEAD_NAME );
    if( headNode ) {
      zCVisual* headVisual = headNode->nodeVisual;
      if( !headVisual )
        return False;
      
      zMAT4 headTrafo       = GetTrafoModelNodeToWorld( headNode );
      zVEC3 startLocal      = headTrafo.InverseLinTrafo() * start;
      zVEC3 rayLocal        = headTrafo.Transpose() * ray;
      zCMorphMesh* headMesh = headVisual->CastTo<zCMorphMesh>();
      if( headMesh && headMesh->morphMesh ) {
        zTBBox3D& bbox = headMesh->GetBBox3D();
        zVEC3 intersection;
        if( !bbox.TraceRay( startLocal, rayLocal, intersection ) )
          return False;
        
        zTSimpleMesh* mesh = headMesh->morphMesh->GetMesh( Null );
        if( !mesh )
          return False;

        int ok = mesh->TraceRay( startLocal, rayLocal, flags, report );
        if( ok ) {
          report.foundVob = this;
          report.foundPoly = Null;
          report.foundIntersection = headTrafo * report.foundIntersection;
          (int&)report.foundVertex = GetModelNodeId( GetModel(), BIP01_HEAD_NAME );
        }

        delete mesh;
        return ok;
      }

      return headVisual->TraceRay( startLocal, rayLocal, flags, report );
    }

    return False;
  }
}