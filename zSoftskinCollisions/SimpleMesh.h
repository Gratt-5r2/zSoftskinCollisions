// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	struct zTSimplePolygon {
		zVEC3 Triangle[3];
		zVEC3 Normal;
		float Distance;
		int NodeId;
		void CalcNormal();
		int TraceRay( const zVEC3& start, const zVEC3& ray, zVEC3& intersection, float& distance, const int& flags );
	};

	struct zTSimpleMesh {
		Array<zTSimplePolygon*> Polygons;
		zTSimplePolygon* AddTriangle( const zVEC3& a, const zVEC3& b, const zVEC3& c );
		int TraceRay( const zVEC3& start, const zVEC3& ray, const int& flags, zTTraceRayReport& report );
		~zTSimpleMesh();
	};
}