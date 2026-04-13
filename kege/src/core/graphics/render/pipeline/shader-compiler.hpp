//
//  shader-compiler.hpp
//  assets
//
//  Created by Kenneth Esdaile on 2/11/26.
//

#ifndef kege_shader_compiler_hpp
#define kege_shader_compiler_hpp

#include <string>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <variant>
#include <memory>
#include <optional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>

#include "../core/graphics-core.hpp"
#include "pipeline-key.hpp"
#include "shader.hpp"
#include "shader-io.hpp"
#include "shader-reflection.hpp"

namespace kege{

    class ShaderCompiler
    {
    public:

        void compileVariant(const kege::PipelineKey& key);
        void getVariant(const kege::PipelineKey& key);
        ShaderCompiler(kege::Graphics* graphics);

    private:

        struct VertexInfo
        {
            kege::VertexBit semantic;
            int location;
        };

        struct Define
        {
            std::string name;
            int value;
        };

        struct Defines
        {
            std::map< std::string, size_t > table;
            std::vector< Define > defs;
        };

        struct Pair
        {
            uint32_t set, binding;
        };

        struct Stage
        {
            std::map< std::string, int > defines;
            std::vector< ShaderIO > output;
            std::vector< ShaderIO > input;
        };

        void addAttribute(kege::VertexBit semantic, int location, kege::ShaderVar type, const char* name );

        void beginVertexShader(std::stringstream& source);
        void endVertexShader(std::stringstream& source);
        void beginFragmentShader(std::stringstream& source);
        void endFragmentShader(std::stringstream& source);

        void writeResource(std::stringstream& source, const std::string& fname);
        void writeOutputs(std::stringstream& source, const std::string& fname);
        void writeInputs(std::stringstream& source, const std::string& fname);

        void writeVersion(std::stringstream& source);
        void writeExtensions(std::stringstream& source);
        //void writeMacros(std::stringstream& source);
        void processOutput(std::stringstream& source);
        void processInput(std::stringstream& source);
        void processResources(std::stringstream& source);
        void writeFunctions(std::stringstream& source);
        void writeMainFn(std::stringstream& source);

        void writeCameraData(std::stringstream& source);
        void writeObjectData(std::stringstream& source);

        std::string writeVertexShader();
        std::string writeFragmentShader();

        void compileShader(const kege::PipelineKey& key, const std::string& vs, const std::string& fs);

        std::string include(const std::string& fname );
        std::string process(const std::string& fname );

        void define(std::stringstream& source, const std::string& name, int index = 1);
        std::string parseDefineName(const std::string& line);
        void addDefine(Defines& defines, const std::string& name, int index = 1);

        bool checkFeatures(uint64_t signature) { return (_key.features & signature); }
        bool hasInput(int signature);
        void writeFeatures();

        void processShadingModel(std::stringstream& source, const kege::ShadingModel& key);
        void processMeshType(std::stringstream& source, const kege::PipelineKey& key);

    private:
//        std::map<std::string, kege::BindSetDesc> _sets;


        std::map< kege::ShaderStageFlag, Stage > _stages;
        Stage* _curr_stage;

        std::vector< std::string > _output_fnames;

        std::map< kege::ShaderStageFlag, Defines > _defines;
        Defines _global_defines;

        std::map< std::string, Pair > _defs;

        ShaderStageFlag _current_stage;

        kege::PipelineKey _key;
        //std::vector<VertexInfo> _inputs;
        //std::vector<VertexInfo> _outputs;

//        kege::PipelineRendering _pipeline_rendering;
//        kege::VertexLayout _vertex_layout;


        //std::map< std::string, size_t > _shader_set_index_map;
        kege::BindSetDescs _shader_sets;
        kege::PushBlockDescs _push_blocks;

        kege::Graphics* _graphics;
        int _binding_index = 0;
        int _set_index = 0;

        int _output_location = 0;
        int _input_location = 0;

        bool _has_lights;
        bool _has_material;
        bool _has_light_funct;
        bool _has_normal_mapping;
        int _input_signature;

    };
}
#endif /* kege_shader_compiler_hpp */
