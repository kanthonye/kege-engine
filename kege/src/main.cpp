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


void test()
{
    kege::ShaderResrc resource = kege::UniformResourceLayout
    {
        .descriptors = kege::UniformDescriptorSets
        {
            kege::UniformDescriptorSet
            {
                .set = 0,
                .descriptors =
                {
                    kege::UniformDescriptor
                    {
                        .descriptor_type = kege::DescriptorType::CombinedImageSampler,
                        .binding = 0,
                        .count = 1,
                        .name = ""
                    },
                    kege::UniformDescriptor
                    {
                        .descriptor_type = kege::DescriptorType::CombinedImageSampler,
                        .binding = 1,
                        .count = 1,
                        .name = ""
                    }
                }
            },
            kege::UniformDescriptorSet
            {
                .set = 1,
                .descriptors =
                {
                    kege::UniformDescriptor
                    {
                        .descriptor_type = kege::DescriptorType::CombinedImageSampler,
                        .binding = 0,
                        .count = 1,
                        .name = ""
                    }
                }
            },
            kege::UniformDescriptorSet
            {
                .set = 2,
                .descriptors =
                {
                    kege::UniformDescriptor
                    {
                        .descriptor_type = kege::DescriptorType::UniformBuffer,
                        .binding = 0,
                        .count = 1,
                        .name = ""
                    }
                }
            },
        },
        .resources = kege::UniformResourceSets
        {
            kege::UniformResourceSet
            {
                kege::UniformResource
                {
                    .binding = 0,
                    .uniform = kege::ImageBindings
                    {
                        kege::ImageInfo{ .image = {}, .sampler = {}, .layout  = kege::ImageLayout::ShaderReadOnly }
                    }
                },
                kege::UniformResource
                {
                    .binding = 1,
                    .uniform = kege::ImageBindings
                    {
                        kege::ImageInfo{ .image = {}, .sampler = {}, .layout  = kege::ImageLayout::ShaderReadOnly }
                    }
                }
            },
            kege::UniformResourceSet
            {
                kege::UniformResource
                {
                    .binding = 0,
                    .uniform = kege::ImageBindings
                    {
                        kege::ImageInfo{ .image = {}, .sampler = {}, .layout  = kege::ImageLayout::ShaderReadOnly }
                    }
                }
            },
            kege::UniformResourceSet
            {
                kege::UniformResource
                {
                    .binding = 0,
                    .uniform = kege::BufferBindings
                    {
                        kege::BufferInfo{}
                    }
                }
            },
        },
        .graphics = nullptr
    };


    resource.getShaderBindings();


//    kege::AssetManager rm;
//    rm.initalize();
//    rm.add< kege::BufferDefn >( "apple", {});
//
//
//    rm.add< std::string >("apple", "apple");
//    rm.add< std::string >("banana", "banana");
//    rm.add< std::string >("grapes", "grapes");
//    rm.add< std::string >("plums", "plums");
//    rm.add< std::string >("mangos", "mangos");
//    rm.add< std::string >("coconut", "coconut");
//    rm.add< std::string >("limes", "limes");
//    rm.add< std::string >("cherries", "cherries");
//
//    print(*rm.get< std::string >());
//
//    rm.remove< std::string >("apple");
//    rm.remove< std::string >("cherries");
//    rm.remove< std::string >("plums");
//
//    print(*rm.get< std::string >());
//
//    rm.fetch< std::string >( "" );
//    rm.get< std::string >( 0 );
//
}





void print( kege::ResourceManagerT< std::string >& lst)
{
    for (int i=lst.begin(); i >= 0; i = lst.next(i))
    {
        std::cout << lst.get( i ) << ", ";
    }
    std::cout <<"\n";
}
