// Supported with union (c) 2020 Union team

// User API for zCModel
// Add your methods here

Array<struct zTSimpleMesh*> GetMeshList();
int TraceRaySoftskin( const zVEC3& start, const zVEC3& ray, int flags, zTTraceRayReport& report );
int TraceRaySoftskin( const zMAT4& vobTrafo, const zVEC3& start, const zVEC3& ray, int flags, zTTraceRayReport& report );
void ShowPolygons( const zMAT4& trafo, const zCOLOR& color );
void CalcModelBBox3DWorld_Union();