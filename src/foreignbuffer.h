#ifndef FOREIGNBUFFER_H
#define FOREIGNBUFFER_H

#include <Godot.hpp>
#include <Reference.hpp>

namespace godot {

class ForeignBuffer : public Reference {
    GODOT_CLASS(ForeignBuffer, Reference)

private:

public:
    static void _register_methods();

    static inline const char *___get_class_name() {
        return (const char *) "ForeignBuffer";
    }

    ForeignBuffer();
    ~ForeignBuffer();

    void _init();

    void _process(float delta);
};

}

#endif
