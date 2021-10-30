XeusModule = require('./emscripten_wasm_test.js')
console.log(XeusModule)


async function test() {
    Module = XeusModule().then((Module) => {
        raw_xkernel = new Module.xkernel();
        var raw_xserver = raw_xkernel.get_server();
        raw_xkernel.start()
    })
}

test();