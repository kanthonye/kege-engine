//
//  collision-detector.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/28/25.
//

#include "../../../physics/3d/collision/rayhit/rayhit.hpp"
#include "../../../physics/3d/collision/algorithms/box-vs-box.hpp"
#include "../../../physics/3d/collision/algorithms/box-vs-cone.hpp"
#include "../../../physics/3d/collision/algorithms/box-vs-sphere.hpp"

#include "../../../physics/3d/collision/algorithms/plane-vs-box.hpp"
#include "../../../physics/3d/collision/algorithms/plane-vs-cone.hpp"
#include "../../../physics/3d/collision/algorithms/plane-vs-sphere.hpp"
#include "../../../physics/3d/collision/algorithms/plane-vs-cone.hpp"
#include "../../../physics/3d/collision/algorithms/plane-vs-capsule.hpp"
#include "../../../physics/3d/collision/algorithms/plane-vs-capsule.hpp"
#include "../../../physics/3d/collision/algorithms/plane-vs-cylinder.hpp"
#include "../../../physics/3d/collision/algorithms/plane-vs-mesh.hpp"
#include "../../../physics/3d/collision/algorithms/plane-vs-plane.hpp"

#include "../../../physics/3d/collision/algorithms/capsule-vs-cone.hpp"
#include "../../../physics/3d/collision/algorithms/capsule-vs-box.hpp"
#include "../../../physics/3d/collision/algorithms/capsule-vs-sphere.hpp"
#include "../../../physics/3d/collision/algorithms/capsule-vs-capsule.hpp"
#include "../../../physics/3d/collision/algorithms/capsule-vs-cylinder.hpp"

#include "../../../physics/3d/collision/algorithms/cylinder-vs-box.hpp"
#include "../../../physics/3d/collision/algorithms/cylinder-vs-cone.hpp"
#include "../../../physics/3d/collision/algorithms/cylinder-vs-sphere.hpp"
#include "../../../physics/3d/collision/algorithms/cylinder-vs-cylinder.hpp"

#include "../../../physics/3d/collision/algorithms/mesh-vs-box.hpp"
#include "../../../physics/3d/collision/algorithms/mesh-vs-cone.hpp"
#include "../../../physics/3d/collision/algorithms/mesh-vs-sphere.hpp"
#include "../../../physics/3d/collision/algorithms/mesh-vs-cylinder.hpp"
#include "../../../physics/3d/collision/algorithms/mesh-vs-capsule.hpp"
#include "../../../physics/3d/collision/algorithms/mesh-vs-mesh.hpp"

#include "../../../physics/3d/collision/algorithms/sphere-vs-cone.hpp"
#include "../../../physics/3d/collision/algorithms/sphere-vs-sphere.hpp"

#include "../../../physics/3d/collision/algorithms/cone-vs-cone.hpp"
#include "../../../physics/3d/collision/algorithms/circle-vs-circle.hpp"
#include "../../../physics/3d/collision/algorithms/plane-vs-circle.hpp"

#include "../simulation/physics-simulation.hpp"
#include "collision-detector.hpp"

namespace kege::physics{

    void CollisionDetector::simulate( double time_step )
    {
        ComponentCacheT< Rigidbody >::Iterator body[2];
        for ( body[0] = _simulator->rigidbodies().begin(); body[0] != _simulator->rigidbodies().end(); body[0]++ )
        {
            if ( !body[0]->collider )
                continue;

            for ( body[1] = body[0] + 1; body[1] != _simulator->rigidbodies().end(); body[1]++ )
            {
                if ( !body[1]->collider )
                    continue;

                if ( body[0]->immovable && body[1]->immovable )
                    continue;

                if ( !body[0]->is_awake && !body[1]->is_awake )
                    continue;

                const RigidShape& shape1 = body[ 0 ]->collider->shape_type;
                const RigidShape& shape2 = body[ 1 ]->collider->shape_type;
                if( _collision_function_table[ shape1 ][ shape1 ]( *body[ 0 ], *body[ 1 ], _simulator->getCollisionRegistry() ) )
                {}
            }
        }
    }

    CollisionDetector::CollisionDetector()
    {
        kege::algo::initializeRayHitFunctionTable();
        _collision_function_table[ RIGID_SHAPE_OBB      ][ RIGID_SHAPE_OBB         ] = algo::boxBoxCollision;
        _collision_function_table[ RIGID_SHAPE_OBB      ][ RIGID_SHAPE_SPHERE      ] = algo::boxSphereCollision;
        _collision_function_table[ RIGID_SHAPE_OBB      ][ RIGID_SHAPE_PLANE       ] = algo::boxPlaneCollision;
        _collision_function_table[ RIGID_SHAPE_OBB      ][ RIGID_SHAPE_CAPSULE     ] = algo::boxCapsuleCollision;
        _collision_function_table[ RIGID_SHAPE_OBB      ][ RIGID_SHAPE_CYLINDER    ] = algo::boxCylinderCollision;
        _collision_function_table[ RIGID_SHAPE_OBB      ][ RIGID_SHAPE_MESH        ] = algo::boxMeshCollision;
        _collision_function_table[ RIGID_SHAPE_OBB      ][ RIGID_SHAPE_CONE        ] = algo::boxConeCollision;
        _collision_function_table[ RIGID_SHAPE_OBB      ][ RIGID_SHAPE_CIRCLE      ] = algo::boxCircleCollision;

        _collision_function_table[ RIGID_SHAPE_SPHERE   ][ RIGID_SHAPE_OBB         ] = algo::sphereBoxCollision;
        _collision_function_table[ RIGID_SHAPE_SPHERE   ][ RIGID_SHAPE_SPHERE      ] = algo::sphereSphereCollision;
        _collision_function_table[ RIGID_SHAPE_SPHERE   ][ RIGID_SHAPE_PLANE       ] = algo::spherePlaneCollision;
        _collision_function_table[ RIGID_SHAPE_SPHERE   ][ RIGID_SHAPE_CAPSULE     ] = algo::sphereCapsuleCollision;
        _collision_function_table[ RIGID_SHAPE_SPHERE   ][ RIGID_SHAPE_CYLINDER    ] = algo::sphereCylinderCollision;
        _collision_function_table[ RIGID_SHAPE_SPHERE   ][ RIGID_SHAPE_MESH        ] = algo::sphereMeshCollision;
        _collision_function_table[ RIGID_SHAPE_SPHERE   ][ RIGID_SHAPE_CONE        ] = algo::sphereConeCollision;
        _collision_function_table[ RIGID_SHAPE_SPHERE   ][ RIGID_SHAPE_CIRCLE      ] = algo::sphereCircleCollision;

        _collision_function_table[ RIGID_SHAPE_PLANE    ][ RIGID_SHAPE_OBB         ] = algo::planeBoxCollision;
        _collision_function_table[ RIGID_SHAPE_PLANE    ][ RIGID_SHAPE_SPHERE      ] = algo::planeSphereCollision;
        _collision_function_table[ RIGID_SHAPE_PLANE    ][ RIGID_SHAPE_PLANE       ] = algo::planePlaneCollision;
        _collision_function_table[ RIGID_SHAPE_PLANE    ][ RIGID_SHAPE_CAPSULE     ] = algo::planeCapsuleCollision;
        _collision_function_table[ RIGID_SHAPE_PLANE    ][ RIGID_SHAPE_CYLINDER    ] = algo::planeCylinderCollision;
        _collision_function_table[ RIGID_SHAPE_PLANE    ][ RIGID_SHAPE_MESH        ] = algo::planeMeshCollision;
        _collision_function_table[ RIGID_SHAPE_PLANE    ][ RIGID_SHAPE_CONE        ] = algo::planeConeCollision;
        _collision_function_table[ RIGID_SHAPE_PLANE    ][ RIGID_SHAPE_CIRCLE      ] = algo::planeCircleCollision;

        _collision_function_table[ RIGID_SHAPE_MESH     ][ RIGID_SHAPE_OBB         ] = algo::meshBoxCollision;
        _collision_function_table[ RIGID_SHAPE_MESH     ][ RIGID_SHAPE_SPHERE      ] = algo::meshSphereCollision;
        _collision_function_table[ RIGID_SHAPE_MESH     ][ RIGID_SHAPE_PLANE       ] = algo::meshPlaneCollision;
        _collision_function_table[ RIGID_SHAPE_MESH     ][ RIGID_SHAPE_CAPSULE     ] = algo::meshCapsuleCollision;
        _collision_function_table[ RIGID_SHAPE_MESH     ][ RIGID_SHAPE_CYLINDER    ] = algo::meshCylinderCollision;
        _collision_function_table[ RIGID_SHAPE_MESH     ][ RIGID_SHAPE_MESH        ] = algo::meshMeshCollision;
        _collision_function_table[ RIGID_SHAPE_MESH     ][ RIGID_SHAPE_CONE        ] = algo::meshConeCollision;
        _collision_function_table[ RIGID_SHAPE_MESH     ][ RIGID_SHAPE_CIRCLE      ] = algo::meshCircleCollision;

        _collision_function_table[ RIGID_SHAPE_CAPSULE  ][ RIGID_SHAPE_OBB         ] = algo::capsuleBoxCollision;
        _collision_function_table[ RIGID_SHAPE_CAPSULE  ][ RIGID_SHAPE_SPHERE      ] = algo::capsuleSphereCollision;
        _collision_function_table[ RIGID_SHAPE_CAPSULE  ][ RIGID_SHAPE_PLANE       ] = algo::capsulePlaneCollision;
        _collision_function_table[ RIGID_SHAPE_CAPSULE  ][ RIGID_SHAPE_CAPSULE     ] = algo::capsuleCapsuleCollision;
        _collision_function_table[ RIGID_SHAPE_CAPSULE  ][ RIGID_SHAPE_CYLINDER    ] = algo::capsuleCylinderCollision;
        _collision_function_table[ RIGID_SHAPE_CAPSULE  ][ RIGID_SHAPE_MESH        ] = algo::capsuleMeshCollision;
        _collision_function_table[ RIGID_SHAPE_CAPSULE  ][ RIGID_SHAPE_CONE        ] = algo::capsuleConeCollision;
        _collision_function_table[ RIGID_SHAPE_CAPSULE  ][ RIGID_SHAPE_CIRCLE      ] = algo::capsuleCircleCollision;

        _collision_function_table[ RIGID_SHAPE_CYLINDER ][ RIGID_SHAPE_OBB         ] = algo::cylinderBoxCollision;
        _collision_function_table[ RIGID_SHAPE_CYLINDER ][ RIGID_SHAPE_SPHERE      ] = algo::cylinderSphereCollision;
        _collision_function_table[ RIGID_SHAPE_CYLINDER ][ RIGID_SHAPE_PLANE       ] = algo::cylinderPlaneCollision;
        _collision_function_table[ RIGID_SHAPE_CYLINDER ][ RIGID_SHAPE_CAPSULE     ] = algo::cylinderCapsuleCollision;
        _collision_function_table[ RIGID_SHAPE_CYLINDER ][ RIGID_SHAPE_CYLINDER    ] = algo::cylinderCylinderCollision;
        _collision_function_table[ RIGID_SHAPE_CYLINDER ][ RIGID_SHAPE_MESH        ] = algo::cylinderMeshCollision;
        _collision_function_table[ RIGID_SHAPE_CYLINDER ][ RIGID_SHAPE_CONE        ] = algo::cylinderConeCollision;
        _collision_function_table[ RIGID_SHAPE_CYLINDER ][ RIGID_SHAPE_CIRCLE      ] = algo::cylinderCircleCollision;

        _collision_function_table[ RIGID_SHAPE_CONE     ][ RIGID_SHAPE_OBB         ] = algo::coneBoxCollision;
        _collision_function_table[ RIGID_SHAPE_CONE     ][ RIGID_SHAPE_SPHERE      ] = algo::coneSphereCollision;
        _collision_function_table[ RIGID_SHAPE_CONE     ][ RIGID_SHAPE_PLANE       ] = algo::conePlaneCollision;
        _collision_function_table[ RIGID_SHAPE_CONE     ][ RIGID_SHAPE_CAPSULE     ] = algo::coneCapsuleCollision;
        _collision_function_table[ RIGID_SHAPE_CONE     ][ RIGID_SHAPE_CYLINDER    ] = algo::coneCylinderCollision;
        _collision_function_table[ RIGID_SHAPE_CONE     ][ RIGID_SHAPE_MESH        ] = algo::coneMeshCollision;
        _collision_function_table[ RIGID_SHAPE_CONE     ][ RIGID_SHAPE_CONE        ] = algo::coneConeCollision;
        _collision_function_table[ RIGID_SHAPE_CONE     ][ RIGID_SHAPE_CIRCLE      ] = algo::coneCircleCollision;

        _collision_function_table[ RIGID_SHAPE_CIRCLE   ][ RIGID_SHAPE_OBB         ] = algo::circleBoxCollision;
        _collision_function_table[ RIGID_SHAPE_CIRCLE   ][ RIGID_SHAPE_SPHERE      ] = algo::circleSphereCollision;
        _collision_function_table[ RIGID_SHAPE_CIRCLE   ][ RIGID_SHAPE_PLANE       ] = algo::circlePlaneCollision;
        _collision_function_table[ RIGID_SHAPE_CIRCLE   ][ RIGID_SHAPE_CAPSULE     ] = algo::circleCapsuleCollision;
        _collision_function_table[ RIGID_SHAPE_CIRCLE   ][ RIGID_SHAPE_CYLINDER    ] = algo::circleCylinderCollision;
        _collision_function_table[ RIGID_SHAPE_CIRCLE   ][ RIGID_SHAPE_MESH        ] = algo::circleMeshCollision;
        _collision_function_table[ RIGID_SHAPE_CIRCLE   ][ RIGID_SHAPE_CONE        ] = algo::circleConeCollision;
        _collision_function_table[ RIGID_SHAPE_CIRCLE   ][ RIGID_SHAPE_CIRCLE      ] = algo::circleCircleCollision;
    }
}
