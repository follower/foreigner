#include "foreignbuffer.h"

using namespace godot;

void ForeignBuffer::_register_methods() {
    register_method("_process", &ForeignBuffer::_process);
    register_method("hex_encode_buffer", &ForeignBuffer::hex_encode_buffer);
    register_method("set_data_with_offset", &ForeignBuffer::set_data_with_offset);
    register_method("ptr", &ForeignBuffer::ptr);

    // Note: The following methods accept any pointer--they're not specific
    //       to this buffer. (See note in `foreignbuffer.h` for details.)
}

ForeignBuffer::ForeignBuffer() {
}

ForeignBuffer::~ForeignBuffer() {
    Godot::print("Destroying ForeignBuffer");
}

//void ForeignBuffer::_notification(int64_t what) {
//    Godot::print("Notification %d", what);
//}


//
// NOTE: Initialization is currently made complicated by some
//       intersecting `godot-cpp` bugs/issues that affect:
//
//        * `_init` and/or `new_` not being passed parameters.
//
//        * Incorrect handling of default values/optional/variable args.
//
//       As a result, proper initialization requires the `new_()`,
//       `_init()` & `_init_buffer()` combo of calls--the first two
//       are standard Godot process but the `init_buffer()` call needs
//       to be made manually.
//
//       Due to this, the `new_buffer()` call added to  `Foreigner` class
//       should currently be used to create instances of `ForeignBuffer`.
//

void ForeignBuffer::_init() {
    // See comment about initialization above.
    this->size = 0;
    data = NULL;
}

void ForeignBuffer::_init_buffer(int32_t size_required) {
    // See comment about initialization above.
    this->size = size_required;
    this->data = new uint8_t[this->size]; // TODO: Handle this better?
}


void ForeignBuffer::_process(float delta) {
}


String ForeignBuffer::hex_encode_buffer() {
    return String::hex_encode_buffer(this->data, this->size);
};


#if defined(TARGET_GODOT_CPP_3_2_LATEST)
void ForeignBuffer::set_data_with_offset(PoolByteArray pba, int32_t byte_offset) {
#else
void ForeignBuffer::set_data_with_offset(Array array_pba, int32_t byte_offset) {

    PoolByteArray pba = godot::PoolByteArray(array_pba);
#endif

    // TODO: Improve implementation & make more robust.

    byte_offset = std::max(0, byte_offset);

    std::memcpy((uint8_t *)(this->data+byte_offset), pba.read().ptr(), std::min(std::max(0, this->size-byte_offset), pba.size()));

}


uint64_t ForeignBuffer::ptr() { // TODO: Handle in a different way?
  return (uint64_t) this->data;
}


// NOTE: The following methods manipulate raw pointers so things can
//       break when they are used! (i.e. segfaults & the like.)
//
//       The current naming/implementation is not necessarily ideal
//       but it's what I've got working currently, so that's what
//       we're starting with. It's possible either aspect may change.
//
//       Ideally most buffer/struct usage wouldn't actually use these
//       methods directly but would use some higher level abstraction
//       with more safety/checking. That's what I'm aiming for, at
//       least.
//
//       I have already developed a higher-level abstraction for
//       `struct` access/usage which is built on these methods which
//       will eventually be added to the library. Although, again its
//       API/implementation should be considered in a state of flux.
//
//       In terms of the API approach used, I've looked primarily to
//       Python `ctypes` for inspiration which seems to have 2-3
//       layers of abstraction for similar functionality:
//
//        * "Safe" operation with fully ctypes controlled pointers
//          e.g. Python objects created by ctypes itself.
//
//        * Semi-safe manipulation with "raw" pointers when additional
//          type information is supplied.
//
//        * Raw pointer manipulation when you're entirely on your own.
//          (This is pretty much where Foreigner is currently at.)
//
//       In many cases there will always be the need for raw pointer
//       manipulation but with the help of additional tooling
//       (for parsing header files, generating wrapper classes etc)
//       the hope is that at least some safety can be provided. :)
//
//       Currently I have some library/project specific tooling used
//       to parse header files to an intermediate JSON form & then
//       generate GDScript-based wrapper classes. My hope is to
//       eventually make this more generic and include it with
//       Foreigner.
//


// Note: The following methods accept any pointer--they're not specific
//       to this buffer. (See note in `foreignbuffer.h` for details.)
