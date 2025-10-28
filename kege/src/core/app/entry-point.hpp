//
//  entry-point.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/22/25.
//

#pragma once

#include "app.hpp"

int main(int argc, const char * argv[])
{
    kege::ref::Application app = kege::createApplication();
    app->run();
    return 0;
}
