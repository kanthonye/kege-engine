//
//  scene.hpp
//  game
//
//  Created by Kenneth Esdaile on 3/14/25.
//

#ifndef scene_hpp
#define scene_hpp

#include "../memory/ref.hpp"
#include "../math/algebra/vectors.hpp"
#include "../ecs/entity.hpp"
#include "../ecs/entity-registry.hpp"
#include "../utils/asset-system.hpp"
#include "entity-tag.hpp"

namespace kege{

    class Scene : public kege::RefCounter
    {
    public:

        /**
         * @fn getResourceManager
         *
         * @return resource-manager of this scene
         */
        kege::AssetSystem& getAssetSystem();

        /**
         * @fn getEntityRegistry
         *
         * @return entity registry of this scene
         */
        kege::EntityRegistry& getEntityRegistry();

        /**
         * @fn setSceneRay
         *
         * @param ray the ray to set the ray member to.
         */
        void setSceneRay( const kege::vec3& ray );

        /**
         * @fn getSceneRay
         *
         * @return The ray cast into this scene
         */
        const kege::vec3& getSceneRay()const;

        /**
         * @fn setCameraEntity
         * @brief Set the camera entit
         * @param entity The value to set the camera entity to.
         */
        void setCameraEntity( const Entity& entity );

        /**
         * @fn getCameraEntity
         * @brief Get the camera entity
         * @return the camera entity
         */
        const Entity& getCameraEntity()const;

        /**
         * @fn setPlayer
         * @brief Set the player entit
         * @param entity The value to set the player to
         */
        void setPlayer( const Entity& entity );

        /**
         * @fn getPlayer
         * @brief Get the player entity
         * @return the player entity
         */
        const Entity& getPlayer()const;

        /**
         * @fn get
         * @brief Get an entity from the scene by name
         * @param name The name of the entity
         * @return the entity associated with the name or a invalid entity
         */
        Entity get( const std::string& name );

        /**
         * @fn remove
         * @brief Remove an entity from the scene by name
         * @param name The name of the entity to remove
         */
        bool remove( const std::string& name );

        /**
         * @fn remove
         * @brief Remove an entity from the scene
         * @param entity the entity to remove
         */
        bool remove( Entity& entity );

        /**
         * @fn init
         * @brief Integrate an entity into a set based on its component bitmask
         * @param entity The ID of the entity to integrate into the set.
         */
        void insert( Entity entity );

        /**
         * @fn initialize
         *
         * @brief initialize this scene.
         *
         * @return True if initialization was successful false otherwise.
         */
        virtual bool initialize();

        /**
         * @fn shutdown
         *
         * @brief shutdown this scene.
         */
        virtual void shutdown();

        kege::Entity root();
        
        /**
         * @fn ready
         *
         * @return Trus is scene is initialized, false otherwise.
         */
        bool ready()const;

        ~Scene();
        Scene();

    protected:

        kege::AssetSystem _asset_system;

        /**
         * The scene entity set group manager
         */
        kege::EntityRegistry _registry;

        /**
         * The camera entity
         */
        kege::Entity _camera;

        /**
         * The player entity
         */
        kege::Entity _player;

        /**
         * The root scene node
         */
        kege::Entity _root;

        /**
         * The from the camera into the scene
         */
        kege::vec3 _ray;

        /**
         * indicate weather the scene is ready for uses
         */
        bool _ready;
    };

}
#endif /* scene_hpp */
