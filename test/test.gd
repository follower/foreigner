extends SceneTree

func ASSERT(condition, message='Assertion failed.'):
    if not condition:
        push_error(message)
        quit()

func _init():
    print('Starting test')

    var foreigner = load('res://foreigner.gdns').new()
    ASSERT(foreigner)
    prints('Foreigner:', foreigner)

    var test_lib_filename: String = './testlib.so'

    match OS.get_name():
        "Windows":
            test_lib_filename = './testlib.dll'

        "OSX":
            test_lib_filename = './testlib.dylib'

    var lib = foreigner.open(test_lib_filename)

    ASSERT(lib)
    print('Library:', lib)

    var result

    print('##### General tests')

    print('* Testing int getNumber(void)')
    lib.define('getNumber', 'sint32', [])
    result = lib.invoke('getNumber')
    print(result)
    ASSERT(result == 42)

    print('* Testing int sqr(int)')
    # Previously, the following line resulted in the error:
    #
    #   "ERROR: cleanup: There are still MemoryPool allocs in use at exit! At: core/pool_vector.cpp:69."
    #
    # This was due to the `['sint32']` being turned into a `PoolStringArray`
    # which leaked due to errors in `godot-cpp`. A workaround
    # is to changed the `define()` method to receive an Array
    # instead & add some extra handling code.
    lib.define('sqr', 'sint32', ['sint32'])
    result = lib.invoke('sqr', [9])
    print(result)
    ASSERT(result == 81)
    result = lib.invoke('sqr', [16])
    print(result)
    ASSERT(result == 256)

    print('* Testing int add2i(int, int)')
    lib.define('add2i', 'sint32', ['sint32', 'sint32'])
    result = lib.invoke('add2i', [6, 8])
    print(result)
    ASSERT(result == 14)

    print('* Testing double add3d(double, double)')
    lib.define('add3d', 'double', ['double', 'double', 'double'])
    result = lib.invoke('add3d', [3.0, 4.0, 5.0])
    print(result)
    ASSERT(result == 12.0)

    print('##### Pointers')

    print('* Testing int retrieveInt(allocateInt(int))')
    lib.define('allocateInt', 'pointer', ['sint32'])
    lib.define('retrieveInt', 'sint32', ['pointer'])
    result = lib.invoke('retrieveInt', [lib.invoke('allocateInt', [1337])])
    print(result)
    ASSERT(result == 1337)

    print('##### Strings')

    print('* Testing const char* getConstString()')
    lib.define('getMessage', 'string', [])
    result = lib.invoke('getMessage', [])
    print(result)
    ASSERT(result == 'Hello world!')

    print('* Testing char* joinStrings(char*, char*)')
    lib.define('joinStrings', 'string', ['string', 'string'])
    result = lib.invoke('joinStrings', ['Foo', 'bar'])
    print(result)
    ASSERT(result == 'Foobar')



    print('##### ForeignBuffer')

    var my_buffer = foreigner.new_buffer(32)
    print(my_buffer)
    print(my_buffer.hex_encode_buffer())


    # Note: The `PoolByteArray()` here will leak with `godot-cpp` 3.1.
    #       (If `define()` isn't re-defined to take Array() instead.)
    my_buffer.set_data_with_offset(PoolByteArray([0x42, 0x00]), 0)

    print(my_buffer.hex_encode_buffer())

    result = lib.invoke('joinStrings', ['Foo', my_buffer])
    print(result)

    ASSERT(result == 'FooB')

    # TODO: Add an assert to verify my_buffer hasn't been modified/freed?
    print("my_buffer: ", my_buffer.hex_encode_buffer())


    result = lib.invoke('joinStrings', ['Foo', my_buffer.ptr()])
    print(result)

    ASSERT(result == 'FooB')
    print("my_buffer: ", my_buffer.hex_encode_buffer())


    result = lib.invoke('retrieveInt', [my_buffer.ptr()])
    ASSERT((result & 0xffff) == 0x42)


    var spb: StreamPeerBuffer = StreamPeerBuffer.new()
    spb.seek(0)
    spb.put_u64(0x0000000012345678)

    # Note: I would use ClassDB to check for the method
    #       but apparently `PoolByteArray` isn't in it.
    #       And, PBA doesn't have `call()`, so there's
    #       no way to make this optional on 3.1 AFAICT.
#    if Engine.get_version_info().hex >= 0x030200:
#      prints("spb: ", spb.data_array.hex_encode())

    var my_buffer2 = foreigner.new_buffer(32)
    my_buffer2.set_data_with_offset(spb.data_array, 0)
    prints("my_buffer2:", my_buffer2.hex_encode_buffer())

    result = lib.invoke('retrieveInt', [my_buffer2])
    ASSERT(result == 0x12345678)
    print("my_buffer2 retrieveint: 0x%08x" % result)

    result = lib.invoke('retrieveInt', [my_buffer2.ptr()])
    ASSERT(result == 0x12345678)
    print("my_buffer2.ptr retrieveint: 0x%08x" % result)

    # One last manual check...
    print("my_buffer: ", my_buffer.hex_encode_buffer())


    print('Testing finished')
    quit()

