//
//  ui-state-manager.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 6/10/26.
//

#ifndef ui_state_manager_hpp
#define ui_state_manager_hpp

#include "ui-gui.hpp"

namespace kege::ui{

    class StateMgr : public kege::RefCounter{
    public:

        virtual void cleanup() = 0;
        virtual void update(){ _frame_number += 1; }
        virtual ~StateMgr(){}

    protected:

        static uint32_t _frame_number;
    };


    template<typename T> class StateMgrT : public StateMgr{
    public:

        struct Entry
        {
            T item;
            uint32_t frame_number;
        };
        using Iterator = typename std::unordered_map<uint64_t, Entry>::iterator;

        T* get( uint64_t user_id )
        {
            auto itr = _states.find(user_id);
            if( itr != _states.end() )
            {
                itr->second.frame_number = _frame_number;
                return &itr->second.item;
            }
            return nullptr;
        }

        T* insert( uint64_t user_id )
        {
            Entry& entry = _states[user_id];
            entry.frame_number = _frame_number;
            return &entry.item;
        }

        T* make( uint64_t user_id )
        {
            auto itr = _states.find(user_id);
            if( itr != _states.end() )
            {
                itr->second.frame_number = _frame_number;
                return &itr->second.item;
            }
            Entry& entry = _states[user_id];
            entry.frame_number = _frame_number;
            return &entry.item;
        }

        void cleanup()
        {
            std::vector<Iterator> itrs;
            for (Iterator i = _states.begin(); i != _states.end(); i++)
            {
                if (abs(int(_frame_number - i->second.frame_number)) >= 60)
                {
                    itrs.push_back(i);
                }
            }
            for (auto i : itrs) _states.erase(i);
        }

    private:

        std::unordered_map<uint64_t, Entry> _states;
    };



    class StateManager{
    private:

        template<typename T>
        static kege::Ref< StateMgrT<T> >& instance()
        {
            static kege::Ref< StateMgrT<T> > mgr;
            if (mgr == nullptr)
            {
                mgr = new StateMgrT<T>;
                _mgrs.push_back( mgr.ref() );
            }
            return mgr;
        }

    public:

        template<typename T>
        static T* get( uint64_t user_id )
        {
            return instance< T >()->get( user_id );
        }

        template<typename T>
        static T* insert( uint64_t user_id )
        {
            return instance< T >()->insert( user_id );
        }

        template<typename T>
        static T* make( uint64_t user_id )
        {
            return instance< T >()->make( user_id );
        }

        static void update()
        {
            for(auto* mgr : _mgrs)
            {
                mgr->cleanup();
            }

            for(auto* mgr : _mgrs)
            {
                mgr->update();
            }
        }

    private:

        static std::vector< StateMgr* > _mgrs;
    };
}

#endif /* ui_state_manager_hpp */
