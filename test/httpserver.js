//
// Simple HTTP server that for every request: 
//  - logs the request to the console
//  - returns 200 with body "OK"
//

// see https://nodejs.org/dist/latest-v4.x/docs/api/http.html
var http = require('http');

http.createServer(function (req, res) {
    console.log(new Date().toISOString() + ': ' + req.url);
    res.writeHead(200, {
        'Content-Type': 'text/plain'
    });
    res.end("OK");
}).listen(8888, function () {
    console.info('server started ...\n')
});