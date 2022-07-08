# zSoftskinCollisions
This plugin allow to get intersection from ray and composite models.
You can trace the NPCs from any plugins or patches. For that you need to define a new traceray flag.
```cpp
enum {
  zTRACERAY_NPC_SOFTSKIN = 1 << 26
};
```

For example, get the intersection point of 5 meters from camera view.
```cpp
  void DrawCross3D( const zMAT4& trafo, const zVEC3& location, const zCOLOR& color, const float& size ) {
    zVEC3 up = trafo.GetUpVector();
    zVEC3 right = trafo.GetRightVector();
    zlineCache->Line3D(
      location + up * -size + right * -size,
      location + up * +size + right * +size,
      color, 0 );

    zlineCache->Line3D(
      location + up * +size + right * -size,
      location + up * -size + right * +size,
      color, 0 );
  }

  void Game_Loop() {
    oCWorld* world   = ogame->GetGameWorld();
    zCVob* cameraVob = ogame->GetCameraVob();
    zVEC3 rayStart   = cameraVob->GetPositionWorld();
    zVEC3 ray        = cameraVob->GetAtVectorWorld() * 500.0f;

    int ok = world->TraceRayNearestHit( rayStart, ray, player, zTRACERAY_NPC_SOFTSKIN );
    if( ok ) {
      zTTraceRayReport& report = world->traceRayReport;
      DrawCross3D( cameraVob->trafoObjToWorld, report.foundIntersection, GFX_WHITE, 1.5f );

      zCVob* foundVob = report.foundVob;
      if( foundVob && foundVob->type == zVOB_TYPE_NSC ) {
        oCNpc* npc = reinterpret_cast<oCNpc*>(foundVob);
        DrawCross3D( cameraVob->trafoObjToWorld, report.foundIntersection, GFX_RED, 5.0f );

        zCModel* model = npc->GetModel();
        int nodeId = reinterpret_cast<int&>( report.foundVertex );
        if( nodeId != Invalid && nodeId < model->nodeList.GetNum() ) {
          zCModelNodeInst* nodeInst = model->nodeList[nodeId];
          screen->PrintCX( 2500, nodeInst->protoNode->nodeName );
        }
      }
    }
  }
```


![image](https://user-images.githubusercontent.com/55413597/178073836-0cfd9a3e-a5aa-454c-aebf-4394a7da2f43.png)
