var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);

function addZ(n) {
    return n<10?'0'+n:''+n;
}

function datelog(string) {
    var date = new Date();
    console.log("[" + date.getFullYear() + "-" + addZ(date.getMonth()+1) + "-" + addZ(date.getDate()) + " " + addZ(date.getHours()) + ":" + addZ(date.getMinutes()) + "] " + string);
}

var port = 9000;
http.listen(port);
datelog('Server listening on port '+port+'.')

var list_watch_client = {}

io.on('connection', function(socket){
  datelog("New client connected");

  list_watch_client[socket.id] = socket

  socket.emit('event', {"id": socket.id});

  socket.on('connected', function(data) {
      datelog("Client ("+data["IMEI"]+") says: "+data["Hi"]);
  });

  socket.on('clicked', function(data) {
      datelog("Client ("+data["IMEI"]+") says: "+data["Date"]);
  });

  socket.on('touchframe', function(data) {
      datelog("Client ("+data["IMEI"]+") says: Touch frame!");
      datelog("Client ("+data["IMEI"]+")         x: "+data["x"]);
      datelog("Client ("+data["IMEI"]+")         y: "+data["y"]);
      datelog("Client ("+data["IMEI"]+")      type: "+data["type"]);
  });

  socket.on('disconnect' , function() {
      datelog("Client disconnected");
      delete list_watch_client[socket.id]
  });
});


const net = require('net');

const PORT = 9001;
const ADDRESS = 'localhost';

let server = net.createServer(onClientConnected);
server.listen(PORT, ADDRESS);

function onClientConnected(socket) {

  var clientName = 'Qt Client'
  datelog(clientName + ' connected.');

  socket.on('data', (data) => { 
    //var msg = data.toString().replace(/[\n\r]*$/, '');
    // var msg = data.toString();
    // datelog(clientName + ' said: ' + msg);
    // datelog(clientName + ' said: something');

    // notifing the client
    // socket.write('We got your message (' + msg + '). Thanks!\n');
    socket.write('We got your message. Thanks!\n');

    for (var key in list_watch_client) {
        list_watch_client[key].emit('qt', {"msg": data});
    }
  });
  
  socket.on('end', () => {
    datelog(clientName + ' disconnected.');
  });
}

datelog('Server for Qt started at: ' + ADDRESS + ':' + PORT);
