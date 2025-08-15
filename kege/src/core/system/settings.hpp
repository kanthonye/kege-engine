//
//  settings.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef settings_hpp
#define settings_hpp

#include "ref.hpp"
#include <vector>
#include <map>

#include "system-factory.hpp"

namespace kege{

    class Setting : public kege::RefCounter
    {
    public:

        template< typename T > kege::Ref< Setting > add(const std::string& name, T* data );
        template< typename T > const T* get()const;
        template< typename T > T* get();

        void insert( const std::string& name, kege::Ref< Setting > setting );
        Setting* operator[]( const std::string& name );

        void remove(const std::string& name );

        static kege::Setting& singleton()
        {
            static Setting settings(nullptr);
            return settings;
        }


        Setting( void* data );

    public:

        std::map< std::string, kege::Ref< Setting > > _settings;
        void* _data;
    };

    template< typename T > kege::Ref< Setting > Setting::add(const std::string& name, T* data )
    {
        Setting* setting = new Setting( data );
        insert( name, setting );
        return setting;
    }
    template< typename T > const T* Setting::get()const
    {
        return reinterpret_cast< T* >( _data );
    }
    template< typename T > T* Setting::get()
    {
        return reinterpret_cast< T* >( _data );
    }
}
#endif /* settings_hpp */
