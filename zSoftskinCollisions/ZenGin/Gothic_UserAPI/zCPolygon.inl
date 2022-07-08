// Supported with union (c) 2020 Union team

// User API for zCPolygon
// Add your methods here

void SetTriangleData( const zVEC3* triData );
void GetTriangleData( zVEC3* triData );
void SetPlaneData( const zVEC3& normal, const float& distance );
void GetPlaneData( zVEC3& normal, float& distance );