let Static = require('node-static').Server,
    http  = require('http'),
    mlp = require('./build/Release/mlp.node'),
    sharp = require('sharp'),
    
    staticClient = new Static('./public/', {cache: false});


mlp.init('../../asd');


http
    .createServer(( request, response ) => {
        request.addListener('end', () => staticClient.serve(request, response)).resume();
    })
    .listen(3333).on('listening', () => {
        console.log('serve client static files on port 3333');
    });

let arr = [];

http.createServer((request, response) => {
    let body = [];
    
    response.setHeader('Access-Control-Allow-Origin', '*');    
    response.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS, PUT, PATCH, DELETE');

    request
        .on('data', (chunk) => {
            body.push(chunk);
        })
        .on('end', () => {            
            body = Buffer.concat(body).toString();
            body = body.replace('data:image/png;base64,', '');
            body = Buffer.from(body, 'base64');

            sharp(body)
                .resize(28, 28, {
                    kernel: sharp.kernel.cubic
                })
                .grayscale(true)
                .raw()
                .toBuffer(function(err, data, info) {
                    let pixels = [],
                        index;

                    for ( index = 0; index < data.length; index++ ) {
                        pixels.push(data[index] / 255);
                    }

                    let result = mlp.recognize(pixels);

                    response.write(JSON.stringify(result));
                    response.end();
                });
            // console.log(body)
            
            
        });
}).listen(8000);
