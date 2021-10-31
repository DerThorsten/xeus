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


function test_buffer_sequence_cpp_to_js(Module) {
    buffer_sequence = new Module.buffer_sequence()
    buffer_sequence.push_back(Module.binary_buffer_from_string("foo"))
    buffer_sequence.push_back(Module.binary_buffer_from_string("bar"))

    buffers_view = buffer_sequence.view()
    buffers_copy = buffer_sequence.copy()

    // for(let i=0; i<buffers_copy.length; ++i)
    // {
    //     console.log("AAA",buffers_copy[i].buffer)
    // }
}



// function test_buffer_sequence_js_to_cpp(Module) {
//     buffer_sequence = new Module.buffer_sequence()
//     buffers = [
//         new Int8Array([1,2,3.5]),
//         new Float32Array([1, 2.5, 3.5, 16.0])
//     ]

//     buffers_int8 = []
//     buffers.forEach(function(item){
//       buffers_int8.push(new Int8Array(item.buffer));
//     });

//     console.log(buffers)
//     console.log(buffers_int8)

//     console.log(new Float32Array(buffers_int8[1].buffer))

//     console.log("from js")
//     buffer_sequence.from_js(buffers)
//     console.log("from js done")
// }




function test_buffer_sequence_js_to_cpp(Module) {
    
    buffers = [
        new Int8Array([-1,2,-3,4,-5]),
        new Uint8Array([10,20,30,40,50]),
        new Float32Array([1.5, 2.5, 3.5, 4.5])
    ]
    // from js to c++
    buffer_sequence = new Module.buffer_sequence()
    buffer_sequence.from_js(buffers)

    // from c++ to js 
    int8_buffers_restored = buffer_sequence.view()
    restored_buffers = [
        new Int8Array(int8_buffers_restored[0].buffer, int8_buffers_restored[0].byteOffset,    int8_buffers_restored[0].length),
        new Uint8Array(int8_buffers_restored[1].buffer, int8_buffers_restored[1].byteOffset,   int8_buffers_restored[1].length),
        new Float32Array(int8_buffers_restored[2].buffer, int8_buffers_restored[2].byteOffset, int8_buffers_restored[2].length / 4)
    ]

    console.log("buffers",buffers)
    console.log("int8_buffers_restored",int8_buffers_restored)
    console.log("restored_buffers", restored_buffers)


    // from js to c++ again
    buffer_sequence_2 = new Module.buffer_sequence()
    buffer_sequence_2.from_js(restored_buffers)

    // from c++ to js again
    int8_buffers_restored = buffer_sequence_2.view()
    restored_buffers = [
        new Int8Array(int8_buffers_restored[0].buffer, int8_buffers_restored[0].byteOffset,    int8_buffers_restored[0].length),
        new Uint8Array(int8_buffers_restored[1].buffer, int8_buffers_restored[1].byteOffset,   int8_buffers_restored[1].length),
        new Float32Array(int8_buffers_restored[2].buffer, int8_buffers_restored[2].byteOffset, int8_buffers_restored[2].length / 4)
    ]


    console.log("buffers",buffers)
    console.log("int8_buffers_restored",int8_buffers_restored)
    console.log("restored_buffers", restored_buffers)
}

async function test() {
    XeusModuleFactory().then((Module) => {
        test_buffer_sequence_js_to_cpp(Module)
        test_buffer_sequence_cpp_to_js(Module)
        test_binary_buffer(Module)
        test_xmessage(Module)
        test_start_kernel(Module)
    })
}

test();