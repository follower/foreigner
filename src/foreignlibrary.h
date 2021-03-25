#ifndef FOREIGNLIBRARY_H
#define FOREIGNLIBRARY_H

#include <ffi.h>
#include <unordered_map>
#include <vector>

#include <Godot.hpp>
#include <Reference.hpp>
#include <Array.hpp>
#include <Variant.hpp>
#include <String.hpp>

#include "foreigner_cfg.h"
#include "crossplatform.h"

namespace godot {

typedef struct {
    ffi_cif* cif;
    std::vector<std::string> argtypes;
    std::string restype;
} signature_t;

typedef std::unordered_map<uint64_t, signature_t*> signature_map_t;
typedef std::unordered_map<uint64_t, SYMBOL> symbol_map_t;

class ForeignLibrary : public Reference {
    GODOT_CLASS(ForeignLibrary, Reference)

private:
    HANDLE handle = 0;
    //std::unordered_map<uint64_t, ffi_cif*> cif_map;
    //std::unordered_map<uint64_t,
    signature_map_t signature_map;
    symbol_map_t symbol_map;
    ffi_type* get_ffi_type(String name);

public:
    static void _register_methods();

    ForeignLibrary();
    ~ForeignLibrary();

    void _init();
    //void _notification(int64_t what);

    void setHandle(void *handle);
#if defined(TARGET_GODOT_CPP_3_2_LATEST)
    void define(String method, String retType, PoolStringArray argTypes);
#else
    void define(String method, String retType, Array argTypes);
#endif
    Variant invoke(String method, Array args);

    void _process(float delta);
};

}

#endif
