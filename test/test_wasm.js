XeusModuleFactory = require('./emscripten_wasm_test.js')

function test_start_kernel(Module) {
    raw_xkernel = new Module.xkernel();
    var raw_xserver = raw_xkernel.get_server();
    raw_xkernel.start()
}

function test_xmessage(Module) {
    message = new Module.xmessage();
}


 
async function test() {
    XeusModuleFactory().then((Module) => {
        test_xmessage(Module)
        test_start_kernel(Module)
    })
}

test();