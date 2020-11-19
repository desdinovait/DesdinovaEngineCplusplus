################################################################
### Core Dump from Novodex Physics SDK
### Core Dump Generated on Monday, March 07, 2005 at 09:37 PM
### Contains 2 assets.
PsReset
PsVersion 1.4
PsNameSpace PhysicsSDK__3d2900


### Begin Material Definition ####
PsMatBegin mat1
PsMatDynamicFriction 0
PsMatStaticFriction 0
PsMatSpinFriction 0
PsMatRollFriction 0
PsMatRestitution 0
PsMatDynamicFrictionV 0
PsMatStaticFrictionV 0
PsMatDirOfAnisotropy 1 0 0
PsMatDirOfMotion 1 0 0
PsMatSpeedOfMotion 0
PsMatAnisotropic false
PsMatMovingSurface false
PsMatEnd
### End Material Definition ####


### Begin Material Definition ####
PsMatBegin mat2
PsMatDynamicFriction 0.200000003
PsMatStaticFriction 0.200000003
PsMatSpinFriction 0
PsMatRollFriction 0
PsMatRestitution 0.899999976
PsMatDynamicFrictionV 0.5
PsMatStaticFrictionV 0.5
PsMatDirOfAnisotropy 0 0 0
PsMatDirOfMotion 1 0 0
PsMatSpeedOfMotion 0
PsMatAnisotropic false
PsMatMovingSurface false
PsMatEnd
### End Material Definition ####

PsAssetBegin Asset__3d4ff0
################################################################
### Novodex Specific Physics Simulation Constants
################################################################
PsSetConstant NX_PENALTY_FORCE 0.800000012
PsSetConstant NX_MIN_SEPARATION_FOR_PENALTY -0.050000001
PsSetConstant NX_DEFAULT_SLEEP_LIN_VEL_SQUARED 0.022500001
PsSetConstant NX_DEFAULT_SLEEP_ANG_VEL_SQUARED 0.0196
PsSetConstant NX_BOUNCE_TRESHOLD -2
PsSetConstant NX_DYN_FRICT_SCALING 1
PsSetConstant NX_STA_FRICT_SCALING 1
PsSetConstant NX_MAX_ANGULAR_VELOCITY 7
PsSetConstant NX_MESH_MESH_LEVEL 4
PsSetConstant NX_COLL_INFINITY fltmax
PsSetConstant NX_CONTINUOUS_CD 0
PsSetConstant NX_MESH_HINT_SPEED 1
################################################################

###########################################################
#### Asset Information
###########################################################
## Scene in initial configuration.
###########################################################
PsGravity 0 -9.800000191 0
PsPlane  plane(0,1,0,0) localposition(0,0,0) localorientation(0,0,0,1) group(0) material(mat1) name($__3d2d80) awake(false) position(0,0,0) orientation(0,0,0,1) static(true) 
PsDefaultSettings orientation(0,0,0,1)
PsDefaultSettings localposition(0,0,0)
PsDefaultSettings localorientation(0,0,0,1)
PsDefaultSettings group(0)
PsBox sides(12.922327042,6.604532242,4.810034275) material(mat2) name($__3d74b0) position(0,25,0) density(10) com(0,0,0) comrot(0,0,0,1) inertia(22837.14453125,65040.56640625,72047.8828125) mass(4105.16796875) solvercount(4) velocity(0,0,0) angularvelocity(41,51,70) wakeupcounter(0.399999976) lineardamping(0) angulardamping(0.720000029) maxangularvelocity(7) 
PsAssetEnd
##################################################################################

PsAssetBegin Asset__3d8ed0
################################################################
### Novodex Specific Physics Simulation Constants
################################################################
PsSetConstant NX_PENALTY_FORCE 0.800000012
PsSetConstant NX_MIN_SEPARATION_FOR_PENALTY -0.050000001
PsSetConstant NX_DEFAULT_SLEEP_LIN_VEL_SQUARED 0.022500001
PsSetConstant NX_DEFAULT_SLEEP_ANG_VEL_SQUARED 0.0196
PsSetConstant NX_BOUNCE_TRESHOLD -2
PsSetConstant NX_DYN_FRICT_SCALING 1
PsSetConstant NX_STA_FRICT_SCALING 1
PsSetConstant NX_MAX_ANGULAR_VELOCITY 7
PsSetConstant NX_MESH_MESH_LEVEL 4
PsSetConstant NX_COLL_INFINITY fltmax
PsSetConstant NX_CONTINUOUS_CD 0
PsSetConstant NX_MESH_HINT_SPEED 1
################################################################

###########################################################
#### Asset Information
###########################################################
## Total Elapsed Time: 3.083330631
## Elapsed Time Last Frame: 0.001953125
## MaxTimeStep: 0.016666668
## MaxIter: 8
## TimeStep = FIXED
###########################################################
PsGravity 0 -9.800000191 0
PsPlane  plane(0,1,0,0) localposition(0,0,0) localorientation(0,0,0,1) group(0) material(mat1) name($__3d9810) awake(false) position(0,0,0) orientation(0,0,0,1) static(true) 
PsAssetEnd
##################################################################################


PsSetScene 0
PsAsset Asset__3d4ff0 position(0,0,0) orientation(0,0,0,1)

PsSetScene 1
PsAsset Asset__3d8ed0 position(0,0,0) orientation(0,0,0,1)

PsSetScene 0
