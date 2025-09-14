//
//  main.cpp
//  ui
//
//  Created by Kenneth Esdaile on 3/5/25.
//

#include "../src/editor/editor.hpp"

int main(int argc, const char * argv[])
{
    kege::Editor editor;
    return editor.run();
}



struct UniformElement
{
    int index;
    kege::Uniform uniform;
};

struct UniformBindings
{
    inline const UniformElement& operator[](int i) const { return elements[i]; }
    inline UniformElement& operator[](int i){ return elements[i]; }

    uint32_t size()const{ return static_cast< uint32_t >( elements.size() ); }

    UniformBindings& operator =( const UniformBindings& u ){
        elements = u.elements;
        return *this;
    }
    UniformBindings& operator =( UniformBindings&& u ) noexcept {
        u.elements.swap( elements );
        return *this;
    }

    explicit UniformBindings( const std::vector< UniformElement >& v ): elements(v) {}
    UniformBindings( const UniformBindings& u ): elements( u.elements ){}
    UniformBindings( UniformBindings&& u ) { u.elements.swap( elements ); }
    UniformBindings(){}

    std::vector< UniformElement > elements;
};

struct UniformSet
{
    inline const UniformBindings& operator[](int i) const { return bindings[i]; }
    inline UniformBindings& operator[](int i){ return bindings[i]; }

    uint32_t size()const{ return static_cast< uint32_t >( bindings.size() ); }

    UniformSet& operator =( const UniformSet& u ){
        bindings = u.bindings;
        return *this;
    }
    UniformSet& operator =( UniformSet&& u ) noexcept {
        u.bindings.swap( bindings );
        return *this;
    }

    explicit UniformSet( const std::vector< UniformBindings >& v ): bindings(v) {}
    UniformSet( const UniformSet& u ): bindings( u.bindings ){}
    UniformSet( UniformSet&& u ) { u.bindings.swap( bindings ); }
    UniformSet(){}

    std::vector< UniformBindings > bindings;
};


class ShaderResource // ShaderResourceLayout
{
public:

    /**
     * @brief Enables access to this shader resource handles
     * @return A collection of resource handle associated with this shader resource.
     */
    const std::vector< int >* operator ->()const;

    /**
     * @brief Enables access to each uniform set associated with this shader resource
     * @return A reference to the uniform set at the given index
     */
    const UniformSet& operator[](int i) const;

    /**
     * @brief Enables access to each uniform set associated with this shader resource
     * @return A reference to the uniform set at the given index
     */
    UniformSet& operator[](int i);

    /**
     * @brief Boolean conversion operator.
     * @return True if the resource is valid, false otherwise.
     */
    operator bool() const;

    /**
     * @brief Copy assignment operator
     */
    ShaderResource& operator =( const ShaderResource& other );

    /**
     * @brief Move assignment operator
     */
    ShaderResource& operator =( ShaderResource&& other) noexcept;

    /**
     * @brief Copy constructor
     */
    ShaderResource( const ShaderResource& other );

    /**
     * @brief Move constructor
     */
    ShaderResource( ShaderResource&& other )noexcept;

    /**
     * @brief Default constructor creates an invalid/null resource.
     */
    ShaderResource();

    /**
     * @brief deconstructor
     */
    ~ShaderResource();

private:

    /**
     * @brief Constructs a resource handle tied to a specific manager and index.
     * @param mngr The resource manager that owns this resource.
     * @param index The index of the resource in the manager.
     */
    ShaderResource( kege::ShaderResourceManager* mngr, uint32_t index );

private:

    friend kege::ShaderResourceManager;

    /** @brief Pointer to the resource manager that owns this resource. */
    kege::ShaderResourceManager* _mngr;

    /** @brief Index of the resource within the manager. */
    uint32_t _index;
};




struct UniformDesc
{
   /**
    * @brief The descriptor set layout binding name in the shader.
    */
   std::string name = "";

   /**
    * @brief Binding number matching shader declaration.
    * Example: `layout(binding = 0) uniform UniformBufferObject`
    */
   uint32_t binding = 0;

   /**
    * @brief Number of array elements (for array bindings).
    * Use 1 for non-array resources.
    */
   uint32_t count = 1;
};


typedef std::vector< UniformDesc > UniformSetDesc;
typedef std::vector< UniformSetDesc > UniformSetsDesc;


void Main()
{
    UniformBindings
    {{
        UniformElement{},
        UniformElement{},
        UniformElement{}
    }};

    ShaderResource sr;


    UniformSetsDesc
    {
        UniformSetDesc
        {
            UniformDesc
            {
                .binding = 0,
                .count = 0,
            },
            UniformDesc
            {
                .binding = 0,
                .count = 0,
            },
            UniformDesc
            {
                .binding = 0,
                .count = 0,
            },
        },
        UniformSetDesc
        {
            UniformDesc
            {
                .binding = 0,
                .count = 0,
            },
            UniformDesc
            {
                .binding = 0,
                .count = 0,
            },
        }
    };

}
