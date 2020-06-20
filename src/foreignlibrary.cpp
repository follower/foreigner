#include "foreignlibrary.h"
#include "foreignbuffer.h"

using namespace godot;

ffi_type* ForeignLibrary::get_ffi_type(String name) {
    if (name == "uchar") {
        return &ffi_type_uchar;
    } else if (name == "schar") {
        return &ffi_type_schar;
    } else if (name == "uint16") {
        return &ffi_type_uint16;
    } else if (name == "sint16") {
        return &ffi_type_sint16;
    } else if (name == "uint32") {
        return &ffi_type_uint32;
    } else if (name == "sint32") {
        return &ffi_type_sint32;
    } else if (name == "uint64") {
        return &ffi_type_uint64;
    } else if (name == "sint64") {
        return &ffi_type_sint64;
    } else if (name == "float") {
        return &ffi_type_float;
    } else if (name == "double") {
        return &ffi_type_double;
    } else if (name == "void") {
        return &ffi_type_void;
    } else if (name == "pointer" || name == "string") {
        return &ffi_type_pointer;
    } else {
        Godot::print_error(
                "ForeignLibrary: unknown arg type - " + name,
                __FUNCTION__, __FILE__, __LINE__
        );
        return &ffi_type_void;
    }
}

void ForeignLibrary::_register_methods() {
    //register_method("_notification", &ForeignLibrary::_notification);
    register_method("define", &ForeignLibrary::define);
    register_method("invoke", &ForeignLibrary::invoke);
    register_method("_process", &ForeignLibrary::_process);
}

ForeignLibrary::ForeignLibrary() {
}

ForeignLibrary::~ForeignLibrary() {
    Godot::print("Destroying ForeignLibrary");
    for (signature_map_t::iterator it = signature_map.begin(); it != signature_map.end(); it++) {
        signature_t *signature = it->second;
        delete signature->cif;
        delete signature;
    }
    if (this->handle) {
        close_library(this->handle);
    }
}

//void ForeignLibrary::_notification(int64_t what) {
//    Godot::print("Notification %d", what);
//}

void ForeignLibrary::_init() {
}

void ForeignLibrary::setHandle(void *handle) {
    this->handle = handle;
}

void ForeignLibrary::define(String method, String retType, PoolStringArray argTypes) {
    // TODO: Memory leaks
    ffi_cif *cif = new ffi_cif();
    ffi_type **arg_types = new ffi_type*[argTypes.size()];
    ffi_status status;

    signature_t *signature = new signature_t();

    String argString = "";

    for (int i = 0; i < argTypes.size(); i++) {
        arg_types[i] = this->get_ffi_type(argTypes[i]);
        signature->argtypes.push_back(std::string(argTypes[i].alloc_c_string()));
        argString += (argString.length() ? ", " : "") + argTypes[i];
    }
    signature->restype = std::string(retType.alloc_c_string());
    ffi_prep_cif(cif, FFI_DEFAULT_ABI, argTypes.size(), this->get_ffi_type(retType), arg_types);
    signature->cif = cif;
    this->signature_map[method.hash()] = signature;

    Godot::print("Defined function " + method + "(" + argString + ") -> " + retType);
}

String variant_to_string(String a) { return a; }

Variant ForeignLibrary::invoke(String method, Array args) {
    if (!this->handle) {
        Godot::print_error(
                "ForeignLibrary: no library loaded, cannot invoke method " + method,
                __FUNCTION__, __FILE__, __LINE__
        );
        return 0;
    }

    void *sym;
    if (!(sym = this->symbol_map[method.hash()])) {
        sym = get_symbol(this->handle, method.alloc_c_string());

        if (!sym) {
            Godot::print_error(
                    "ForeignLibrary: unresolved symbol - " + method,
                    __FUNCTION__, __FILE__, __LINE__
            );
            return 0;
        }
        this->symbol_map[method.hash()] = sym;
    }

    signature_t *signature;
    if (!(signature = this->signature_map[method.hash()])) {
        Godot::print_error(
                "ForeignLibrary: method " + method + " not prepared yet, cannot call",
                __FUNCTION__, __FILE__, __LINE__
        );
        return 0;
    }

    void *arg_values[signature->cif->nargs];
    uint64_t *arg_values_data[signature->cif->nargs];
    // TODO: Suport more arg types
    String str;
    char* pStr;
    ForeignBuffer *the_buffer;
    for (int i = 0; i < args.size(); i++) {
        switch(args[i].get_type()) {
            case Variant::Type::NIL:
                arg_values[i] = 0;
                break;
            case Variant::Type::INT:
                arg_values[i] = new uint64_t(args[i]);
                break;
            case Variant::Type::REAL:
                arg_values[i] = new double(args[i]);
                break;
            case Variant::Type::BOOL:
                arg_values[i] = new bool(&args[i]);
                break;
            case Variant::Type::STRING:
                // There must be a better way.
                str = args[i];
                pStr = new char[str.length() + 1];
                memcpy(pStr, str.alloc_c_string(), str.length());
                pStr[str.length()] = 0;
                arg_values[i] = new char*(pStr);
                break;
            case Variant::Type::OBJECT:
	      {
                Variant v = args[i]; // Necessary intermediate step for the following to work.

		// My understanding is that there's a simpler approach to use
		// but when I last tried it didn't work for...reasons?
		//
		// One of these, perhaps:
		//
		//   ForeignBuffer *the_buffer = Object::cast_to<ForeignBuffer>(args[i].operator Object*());
		//   ForeignBuffer *the_buffer = as<ForeignBuffer>(args[i].operator godot_object *());
		//   ForeignBuffer *the_buffer = as<ForeignBuffer>(v.operator Object*());

		// So, while inelegant, the following approach did at least work...
		//
		// static_cast<T*>(T::___get_from_variant(*this))
		//
		// via <https://github.com/GodotNativeTools/godot-cpp/pull/270/files>
		/* ForeignBuffer * */ the_buffer = static_cast<ForeignBuffer*>(ForeignBuffer::___get_from_variant(v));

		// TODO: Don't continue if argument is some other Object type.

		// We pass along a pointer to the internal buffer here so
		// that any modifications to the buffer by the invoked
		// function are reflected by what is seen within Godot.
		//
		// Note: In Godot 3.x we can't use `PoolByteArray` or
		//       similar because they are copy-on-write.
		//       This is to be changed in Godot 4.0 but we're
		//       not currently living in the future.
		//
		// Note: It turns out that the...ah..."unintuitively"-named[0]
		//       `StreamPeerBuffer` *may* have been an option
		//       if we created it internally & restricted
		//       access to only the `.put*()` & `.get*()`
		//       methods but that wasn't clear at the time.
		//
		//       My plan is to revisit `StreamPeerBuffer`
		//       in relation to (re-)implementing `struct`
		//       support because it has built-in data type/size
		//       handling.
		//
		//       [0] It's unintuitive because it has *no* Peer :)
		//           and it's *not* a Stream. It's just a
		//           Buffer...with some useful helper methods.
		//
		//           The naming is because it's based on a
		//           class that *is* used by the family of
		//           classes that *do* have Peer(s) & *are*
		//           Streams.
		//
		//           And even knowing this, it's still not
		//           instantly obvious how to avoid Copy-on-Write
		//           aspect--which requires being the creator
		//           of the instance, restricting access
		//           to methods *and* (probably) requires
		//           temporarily duplicating the underlying
		//           buffer when passing it via FFI anyway.
		//
                arg_values[i] = &the_buffer->data;
	      }
                break;
            default:
                // Variant::___get_type_name(args[i].get_type())
                Godot::print_error(
                        String("ForeignLibrary: argument of type ") + \
                            String::num(args[i].get_type()) + \
                            " not yet supported",
                        __FUNCTION__, __FILE__, __LINE__
                );
                arg_values[i] = 0;
        }
    }
    //uint64_t result;
    unsigned char result[8];

    ffi_call(signature->cif, FFI_FN(sym), result, arg_values);

    // Release memory of allocated variables
    for (int i = 0; i < signature->cif->nargs; i++) {
        // TODO: Fix this
        if (signature->argtypes[i] == "string") {
            delete (char*)(*(char**)arg_values[i]);
        } else if ((signature->argtypes[i] == "pointer") && (args[i].get_type() == Variant::Type::OBJECT) ) {

	  // We didn't duplicate anything in this case, so don't try to delete it.

	  // (?) TODO: Revert to handling properly.

        } else {
            delete (uint64_t*) arg_values[i];
        }
    }

    if (signature->restype == "uchar") {
        return Variant(*(uint8_t*) result);
    } else if (signature->restype == "schar") {
        return Variant(*(int8_t*) result);
    } else if (signature->restype == "uint16") {
        return Variant(*(uint16_t*) result);
    } else if (signature->restype == "sint16") {
        return Variant(*(int16_t*) result);
    } else if (signature->restype == "uint32") {
        return Variant(*(uint32_t*) result);
    } else if (signature->restype == "sint32") {
        return Variant(*(int32_t*) result);
    } else if (signature->restype == "uint64") {
        return Variant(*(uint64_t*) result);
    } else if (signature->restype == "sint64") {
        return Variant(*(int64_t*) result);
    } else if (signature->restype == "float") {
        return Variant(*(float*) result);
    } else if (signature->restype == "double") {
        return Variant(*(double*)result);
    } else if (signature->restype == "void") {
        return Variant(*(uint64_t*) result);
    } else if (signature->restype == "pointer") {
        return Variant(*(uint64_t*) result);
    } else if (signature->restype == "string") {
        return Variant(*(const char**) result);
    } else {
        return 0;
    }
}

void ForeignLibrary::_process(float delta) {
}
