#include "foreigner.h"

using namespace godot;

void Foreigner::_register_methods() {
    register_method("open", &Foreigner::open);
    register_method("_process", &Foreigner::_process);
    register_method("new_buffer", &Foreigner::new_buffer);
}

Foreigner::Foreigner() {
}

Foreigner::~Foreigner() {
    Godot::print("Destroying Foreigner");
}

void Foreigner::_init() {
}

Ref<ForeignBuffer> Foreigner::new_buffer(uint32_t size_in_bytes) {

    // Mostly copy pasta...
    ForeignBuffer *the_buffer = ForeignBuffer::_new();
    the_buffer->_init_buffer(size_in_bytes);

    Ref<ForeignBuffer> ref = Ref<ForeignBuffer>::__internal_constructor(the_buffer);
    return ref;

}

Ref<ForeignLibrary> Foreigner::open(String path) {
    //if (this->handle) {
    //    // Another shared library is already open, close it
    //    dlclose(this->handle);
    //    Godot::print("Foreigner: Unloading shared library " + loadedPath);
    //    this->handle = 0;
    //}

    // Attempt to open shared library
    Godot::print("Foreigner: Loading shared library " + path);
    // TODO: Windows/Linux/Mac
    HANDLE handle = open_library(path.alloc_c_string());
    char *error = open_library_error();
    if (error) {
        // Opening failed
        Godot::print_error(
                String("Foreigner: Failed to load " + path + ": " + String(error)),
                __FUNCTION__, __FILE__, __LINE__
        );
        return 0;
    }

    ForeignLibrary *library = ForeignLibrary::_new();
    Ref<ForeignLibrary> ref = Ref<ForeignLibrary>::__internal_constructor(library);
    library->setHandle(handle);
    return ref;
}

void Foreigner::_process(float delta) {
}
