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
#include "../resource/asset-manager.hpp"
#include "ecs-entity-registry.hpp"

namespace kege{

    class Scene : public kege::ecs::EntityRegistry
    {
    public:

        ecs::Entity getEntityChild( ecs::Entity entity, const std::string& name );

        struct Changed{ kege::Ref< kege::Scene > scene; };

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
        void setCameraEntity( const ecs::Entity& entity );

        /**
         * @fn getCameraEntity
         * @brief Get the camera entity
         * @return the camera entity
         */
        const ecs::Entity& getCameraEntity()const;

        /**
         * @fn setPlayer
         * @brief Set the player entit
         * @param entity The value to set the player to
         */
        void setPlayer( const ecs::Entity& entity );

        /**
         * @fn getPlayer
         * @brief Get the player entity
         * @return the player entity
         */
        const ecs::Entity& getPlayer()const;

        /**
         * @fn get
         * @brief Get an entity from the scene by name
         * @param name The name of the entity
         * @return the entity associated with the name or a invalid entity
         */
        ecs::Entity get( const std::string& name );

        /**
         * @fn remove
         * @brief Remove an entity from the scene by name
         * @param name The name of the entity to remove
         */
        void remove( const std::string& name );

        /**
         * @fn remove
         * @brief Remove an entity from the scene
         * @param entity the entity to remove
         */
        void remove( ecs::Entity& entity );

        /**
         * @fn init
         * @brief Integrate an entity into a set based on its component bitmask
         * @param entity The ID of the entity to integrate into the set.
         */
        void insert( ecs::Entity& entity );

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


        const std::string& name()const;
    
        ecs::Entity root();

        /**
         * @fn ready
         *
         * @return Trus is scene is initialized, false otherwise.
         */
        bool ready()const;

        Scene( const std::string& name, ecs::EntityManager &ecs );
        ~Scene();

    protected:

        void registerEntities( const ecs::Entity& entity );

        /**
         * The camera entity
         */
        ecs::Entity _camera;

        /**
         * The player entity
         */
        ecs::Entity _player;

        /**
         * The root scene node
         */
        ecs::Entity _root;

        /**
         * The from the camera into the scene
         */
        kege::vec3 _ray;

        /**
         * indicate weather the scene is ready for uses
         */
        bool _ready;

        std::string _name;
    };

    using Tag = std::string;
}

namespace kege::ref{
    typedef kege::Ref< kege::Scene > Scene;
}
#endif /* scene_hpp */
