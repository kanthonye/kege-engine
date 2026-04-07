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

namespace kege{

    class ShaderProgram;


    class ShaderCompiler
    {
    public:

        ShaderProgram* compileVariant(const kege::PipelineKey& key);
        ShaderProgram* getVariant(const kege::PipelineKey& key);

    private:
        enum ShaderStge
        {
            VERTEX,
            FRAGMENT,
        };

        struct VertexInfo
        {
            enum Semantic
            {
                POSITION,
                TEXCOORD,
                NORMAL,
                TANGENT,
                BITANGENT,
                COLOR,
                JOINTS,
                WEIGHTS,

                ALBEDO,
                EMISSIVE,
            };

            Semantic semantic;
            int location;
        };


        void beginVertexShader(std::stringstream& source);
        void endVertexShader(std::stringstream& source);
        void beginFragmentShader(std::stringstream& source);
        void endFragmentShader(std::stringstream& source);

        void writeVersion(std::stringstream& source);
        void writeExtensions(std::stringstream& source);
        void writeMacros(std::stringstream& source);
        void writeInputs(std::stringstream& source);
        void writeOutputs(std::stringstream& source);
        void writeResources(std::stringstream& source);
        void writeFunctions(std::stringstream& source);
        void writeMainFn(std::stringstream& source);

        void writeCameraData(std::stringstream& source);
        void writeObjectData(std::stringstream& source);

        std::string writeVertexShader();
        std::string writeFragmentShader();

        ShaderProgram* compileShader(const kege::PipelineKey& key, const std::string& vs, const std::string& fs);

        std::string include(const std::string& fname );

//        void defineVertexAttributeIO(const kege::PipelineKey& key, const std::vector<VertexInfo>& attributes, std::vector<std::string>& defines);
//
//        void buildVertexLayout(const kege::PipelineKey& key, std::vector<VertexInfo>& attributes);
//
//        void writeFragmentShaderInput(std::stringstream& source, const std::vector<VertexInfo>& inputs);
//
//        void writeFragmentShaderIO(std::stringstream& source, const kege::PipelineKey& key);
//
//        void writeVertexShaderIO(std::stringstream& source, const kege::PipelineKey& key);
//
//        void writeDefines(std::stringstream& source, const std::vector<std::string>& defines);
//
//        void writeVertexProcessingSelection(std::stringstream& source, const kege::PipelineKey& key);
//
//        void writeLightingSelection(std::stringstream& source, const kege::PipelineKey& key);
//
//        void writeVertexShaderMainFunction(std::stringstream& source, const kege::PipelineKey& key);
//
//        void writeFragmentForwardOpaque(std::stringstream& source, const kege::PipelineKey& key);
//
//        void writeFragmentShaderMainFunction(std::stringstream& source, const kege::PipelineKey& key);

        bool checkFeatures(uint64_t signature) { return (_key.features & signature); }
        bool hasInput(int signature);
        void writeFeatures();

        void processShadingModel(std::stringstream& source, const kege::ShadingModel& key);
        void processMeshType(std::stringstream& source, const kege::PipelineKey& key);

    private:

        ShaderStge _current_stage;

        kege::PipelineKey _key;
        std::vector<VertexInfo> _inputs;
        std::vector<VertexInfo> _outputs;

        std::vector<std::string> _defines;
        kege::Graphics* _graphics;
        int binding_index = 0;
        int set_index = 0;

        bool _has_lights;
        bool _has_material;
        bool _has_light_funct;
        bool _has_normal_mapping;
        int _input_signature;

    };
}
#endif /* kege_shader_compiler_hpp */
