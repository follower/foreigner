#ifndef FOREIGNBUFFER_H
#define FOREIGNBUFFER_H

#include <Godot.hpp>
#include <Reference.hpp>

namespace godot {

class ForeignBuffer : public Reference {
    GODOT_CLASS(ForeignBuffer, Reference)

private:

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

    void set_data_with_offset(PoolByteArray pba, int32_t byte_offset);

    void _process(float delta);
};

}

#endif
