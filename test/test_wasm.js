XeusModuleFactory = require('./emscripten_wasm_test.js')

function test_start_kernel(Module) {
    raw_xkernel = new Module.xkernel();
    var raw_xserver = raw_xkernel.get_server();
    raw_xkernel.start()
}

function test_xmessage(Module) {
    message = new Module.xmessage();
}

function test_binary_buffer(Module) {
    binary_buffer = Module.binary_buffer_from_string("buffer")
    mem_view = binary_buffer.get_mem_view()
    var mem_copy = new Int8Array(mem_view);
}


function test_buffer_sequence(Module) {
    buffer_sequence = new Module.buffer_sequence()
    buffer_sequence.push_back(Module.binary_buffer_from_string("foo"))
    buffer_sequence.push_back(Module.binary_buffer_from_string("bar"))

    js_buffers = []
    for (let i = 0; i < buffer_sequence.size(); i++) {
      js_buffers.push(new Int8Array(buffer_sequence.get_mem_view_at(i)))
    }

    console.log(js_buffers)
}

 
async function test() {
    XeusModuleFactory().then((Module) => {
        test_buffer_sequence(Module)
        test_binary_buffer(Module)
        test_xmessage(Module)
        test_start_kernel(Module)
    })
}

test();