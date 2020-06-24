#ifndef FOREIGNER_H
#define FOREIGNER_H

#include <Godot.hpp>
#include <Reference.hpp>
#include <String.hpp>

#include "crossplatform.h"
#include "foreignlibrary.h"

#include "foreignbuffer.h"

#define TARGET_GODOT_CPP_3_2_LATEST // 9eceb16f0553884094d0f659461649be5d333866

namespace godot {

class Foreigner : public Reference {
    GODOT_CLASS(Foreigner, Reference)

public:
    static void _register_methods();

    static inline const char *___get_class_name() {
        return (const char *) "Foreigner";
    }

    Foreigner();
    ~Foreigner();

    void _init();

    Ref<ForeignLibrary> open(String path);

    // TODO: Decide if this would be better elsewhere.
    Ref<ForeignBuffer> new_buffer(uint32_t size_in_bytes);

    void _process(float delta);
};

}

#endif
