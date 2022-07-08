// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
  zCMeshSoftSkin::zTWeightEntry* zCMeshSoftSkin::GetIterWeightEntry() {
    byte* prev = iterator;
    iterator += sizeof( zTWeightEntry );
    return (zTWeightEntry*)prev;
  };


  int zCMeshSoftSkin::GetIterNumWeightEntry() {
    byte* prev = iterator;
    iterator += 4;
    return *((int*)prev);
  };


  zTSimpleMesh* zCMeshSoftSkin::GetMesh( Array<zMAT4*>& nodeTrafoList ) {
    ResetVertWeightIterator();
    for( int i = 0; i < posList.GetNum(); i++ ) {
      int numNodes = GetIterNumWeightEntry();
      posCache[i] = 0;
      for( int j = 0; j < numNodes; j++ ) {
        auto weightEntry = GetIterWeightEntry();
        int index = weightEntry->nodeIndex;
        posCache[i] += ((*nodeTrafoList[index]) * weightEntry->vertPosOS) * weightEntry->weight;
        nodeCache[i] = index;
      };
    }

    return zCProgMeshProto::GetMesh( posCache );
  }
}