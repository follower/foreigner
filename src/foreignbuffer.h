#ifndef FOREIGNBUFFER_H
#define FOREIGNBUFFER_H

#include <Godot.hpp>
#include <Reference.hpp>

namespace godot {

class ForeignBuffer : public Reference {
    GODOT_CLASS(ForeignBuffer, Reference)

private:

    friend class ForeignLibrary;

    uint8_t *data;
    int32_t size;

public:
    static void _register_methods();

    static inline const char *___get_class_name() {
        return (const char *) "ForeignBuffer";
    }

    ForeignBuffer();
    ~ForeignBuffer();

    void _init();
    void _init_buffer(int32_t size_required);

    String hex_encode_buffer();

#if defined(TARGET_GODOT_CPP_3_2_LATEST)
    void set_data_with_offset(PoolByteArray pba, int32_t byte_offset);
#else
    void set_data_with_offset(Array array__pba, int32_t byte_offset);
#endif

    uint64_t ptr();

    void _process(float delta);
};

}

#endif
