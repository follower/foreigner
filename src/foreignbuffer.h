#ifndef FOREIGNBUFFER_H
#define FOREIGNBUFFER_H

#include <Godot.hpp>
#include <Reference.hpp>

#include "foreigner_cfg.h"

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


    /*

       Note: The following methods accept any pointer--they're not
             specific to this buffer.

       For reasons that I don't remember GDNative/`godot-cpp` doesn't
       support static class methods currently.

       So, while these bare pointer manipulating methods *shouldn't* need
       an instance of this class in order to be used, they currently
       do need such an instance.

       The current idiom I'm using is along these lines:

           var _op = foreigner.new_buffer(32); // Arbitrary buffer size. (Reuse `_op` whereever pointer manipulation is required.)
           var some_ptr = _op.deref(_op.deref(_op.offset(my_struct.ptr(), 8))) // Deref a pointer stored in a buffer instance.

    */

    /*static*/ uint64_t offset(uint64_t original_ptr, int32_t byte_offset);
    /*static*/ uint64_t deref(uint64_t original_ptr);

    void _process(float delta);
};

}

#endif
