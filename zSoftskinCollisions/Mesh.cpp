// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
  template<typename T>
  T GetMemPage( const int& index, const int& offset ) {
    uint address = CMemPage::GetPage( 5 )->GetAddress();
    return (T)(address + offset);
  }


  auto posCache = GetMemPage<zVEC3*>( 5, 0 );
  auto nodeCache = GetMemPage<int*>( 5, 0x80000 );


  uint16 GetBestNodeId( const uint16& a, const uint16& b, const uint16& c ) {
    if( a == b || a == c ) return a;
    if( b == a || b == c ) return b;
    if( c == a || c == b ) return b;
    return a;
  }


  zTSimpleMesh* zCProgMeshProto::GetMesh( zVEC3* posCache ) {
    if( posCache == Null )
      posCache = posList.GetArray();
  
    zTSimpleMesh* mesh = new zTSimpleMesh();
    for( int i = 0; i < numSubMeshes; i++ ) {
      auto& sumMesh = subMeshList[i];
      for( int p = 0; p < sumMesh.triList.GetNum(); p++ ) {
        uint16 idA = subMeshList[i].wedgeList[subMeshList[i].triList[p].wedge[VA]].position;
        uint16 idB = subMeshList[i].wedgeList[subMeshList[i].triList[p].wedge[VB]].position;
        uint16 idC = subMeshList[i].wedgeList[subMeshList[i].triList[p].wedge[VC]].position;
        zVEC3 a = posCache[idA];
        zVEC3 b = posCache[idB];
        zVEC3 c = posCache[idC];
        zTSimplePolygon* poly = mesh->AddTriangle( a, b, c );
        poly->NodeId = nodeCache[GetBestNodeId( idA, idB, idC )];
      }
    }
    
    return mesh;
  }
}