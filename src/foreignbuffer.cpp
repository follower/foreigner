#include "foreignbuffer.h"

using namespace godot;

void ForeignBuffer::_register_methods() {
    register_method("_process", &ForeignBuffer::_process);
}

ForeignBuffer::ForeignBuffer() {
}

ForeignBuffer::~ForeignBuffer() {
    Godot::print("Destroying ForeignBuffer");
}

//void ForeignBuffer::_notification(int64_t what) {
//    Godot::print("Notification %d", what);
//}

void ForeignBuffer::_init() {
}

void ForeignBuffer::_process(float delta) {
}
