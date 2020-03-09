#include "foreignbuffer.h"

using namespace godot;

void ForeignBuffer::_register_methods() {
    register_method("_process", &ForeignBuffer::_process);
    register_method("hex_encode_buffer", &ForeignBuffer::hex_encode_buffer);
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
