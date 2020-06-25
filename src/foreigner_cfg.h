#ifndef FOREIGNER_CFG__H
#define FOREIGNER_CFG__H

// If enabled this will probably lead to crashes
// when Foreigner is compiled against earlier `godot-cpp`
// versions (e.g. 3.1 a.k.a e4ad265339f17042a86227bfb44f9d5d7dee5ba4).
// This results from the fix for `Ref<>` which results
// in the refcount being correct but means that previous
// work arounds based on `__internal_constructor()` will
// now result in an incorrect refcount that will mean
// instances will be freed too early.
#define TARGET_GODOT_CPP_3_2_LATEST // 9eceb16f0553884094d0f659461649be5d333866

#endif
