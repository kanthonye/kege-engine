//
//  main.cpp
//  ui
//
//  Created by Kenneth Esdaile on 3/5/25.
//

#include "../src/editor/editor.hpp"

void test();

int main(int argc, const char * argv[])
{
    kege::Editor editor;
    return editor.run();
}



void print( kege::ResourceManagerT< std::string >& lst)
{
    for (int i=lst.begin(); i >= 0; i = lst.next(i))
    {
        std::cout << lst.get( i ) << ", ";
    }
    std::cout <<"\n";
}

void test()
{
    kege::AssetManager rm;
    rm.initalize();
    rm.add< kege::BufferDefn >( "apple", {});


    rm.add< std::string >("apple", "apple");
    rm.add< std::string >("banana", "banana");
    rm.add< std::string >("grapes", "grapes");
    rm.add< std::string >("plums", "plums");
    rm.add< std::string >("mangos", "mangos");
    rm.add< std::string >("coconut", "coconut");
    rm.add< std::string >("limes", "limes");
    rm.add< std::string >("cherries", "cherries");

    print(*rm.get< std::string >());

    rm.remove< std::string >("apple");
    rm.remove< std::string >("cherries");
    rm.remove< std::string >("plums");

    print(*rm.get< std::string >());

    rm.fetch< std::string >( "" );
    rm.get< std::string >( 0 );


//    lst.pop(8);
//
//    print(lst);
//
//    lst.push("9");
//    lst.push("10");
//    lst.push("11");
//
//    print(lst);

}
