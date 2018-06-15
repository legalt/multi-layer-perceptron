let Static = require('node-static').Server,
    http  = require('http'),
    mlp = require('./build/Release/mlp.node'),
    
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
            body = body.split(',');

            let result = mlp.recognize(body);

            response.write(JSON.stringify(result));
            response.end();
        });
}).listen(8000);
