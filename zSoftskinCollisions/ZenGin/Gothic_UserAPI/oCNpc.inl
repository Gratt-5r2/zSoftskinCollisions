// Supported with union (c) 2020 Union team

// User API for oCNpc
// Add your methods here

int TraceRay_Union( zVEC3 const& start, zVEC3 const& ray, int flags, zTTraceRayReport& report );
int TraceRaySoftskin( const zVEC3& start, const zVEC3& ray, int flags, zTTraceRayReport& report );
int TraceRayNode( zCModelNodeInst* node, const zVEC3& start, const zVEC3& ray, int flags, zTTraceRayReport& report );
int TraceRayNodes( const zVEC3& start, const zVEC3& ray, int flags, zTTraceRayReport& report );