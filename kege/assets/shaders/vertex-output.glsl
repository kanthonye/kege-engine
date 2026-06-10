


@set(name: Global, index: 0)
{
    @bind(block: Camera, index: 0)
    {
        usage:  UNIFORM_BUFFER
        stages: [VERT]
        count:  1
    }
    @bind(block: Font, index: 1)
    {
        usage:  COMBINED_IMAGE_SAMPLER
        stages: [FRAG]
        count:  1
    }
    @bind(block: Theme, index: 2)
    {
        usage:  COMBINED_IMAGE_SAMPLER
        stages: [FRAG]
        count:  1
    }
}
