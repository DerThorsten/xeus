XeusModuleFactory = require('./emscripten_wasm_test.js')

function test_start_kernel(Module) {
    raw_xkernel = new Module.xkernel();
    var raw_xserver = raw_xkernel.get_server();
    raw_xkernel.start()
}

function test_xmessage(Module) {
    message = new Module.xmessage();
}

function test_buffer_sequence_js_to_cpp(XeusModule) {
    
    // create a binary buffer sequence on js side 
    // with various types
    buffers = [
        new Int8Array([-1,2,-3,4,-5]),
        new Uint8Array([10,20,30,40,50]),
        new Float32Array([1.5, 2.5, 3.5, 4.5])
    ]

    // from js to c++
    buffer_sequence = new XeusModule.buffer_sequence() 
    buffer_sequence.from_js(buffers)

    // And for the fun of it we go back to js:
    // c++ to js without a copy!
    int8_restored_buffers = buffer_sequence.view()

    // recover convert to original type
    restored_buffers = [
        new Int8Array(   int8_restored_buffers[0].buffer, int8_restored_buffers[0].byteOffset, int8_restored_buffers[0].length),
        new Uint8Array(  int8_restored_buffers[1].buffer, int8_restored_buffers[1].byteOffset, int8_restored_buffers[1].length),
        new Float32Array(int8_restored_buffers[2].buffer, int8_restored_buffers[2].byteOffset, int8_restored_buffers[2].length / 4)
    ]
}

async function test() {
    XeusModuleFactory().then((XeusModule) => {
        test_buffer_sequence_js_to_cpp(XeusModule)
        test_xmessage(XeusModule)
        test_start_kernel(XeusModule)
    })
}

test();