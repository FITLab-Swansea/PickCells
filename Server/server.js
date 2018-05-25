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

var list_watch_client = {};
var list_imei_client = {};
var qt_client = null;

var buffer_qt = null;
var side_msgs = [];
var configuration = {'devices':[], 'watches': []};
// {'devices': [ {'min_x':-1, 'max_x': 0, 'min_y':0, 'max_y':0, 'min_z':0, 'max_z':0,
//                'connections': {
//                                 id1: {'x':0, 'y':0, 'z':0, 'B':null, 'T':null, 'N':null, 'S':null, 'E':null, 'W':id2},
//                                 id2: {'x':-1, 'y':0, 'z':0, 'B':null, 'T':null, 'N':null, 'S':null, 'E':id1, 'W':null}
//                               }
//               }
//               {'min_x':-1, 'max_x': 0, 'min_y':0, 'max_y':1, 'min_z':0, 'max_z':0,
//                'connections': {
//                                 id1: {'x':0, 'y':0, 'z':0, 'B':null, 'T':null, 'N':null, 'S':null, 'E':null, 'W':id2},
//                                 id2: {'x':-1, 'y':0, 'z':0, 'B':id3, 'T':null, 'N':null, 'S':null, 'E':id1, 'W':null},
//                                 id3: {'x':-1, 'y':1, 'z':0, 'B':null, 'T':id2, 'N':null, 'S':null, 'E':id1, 'W':null}
//                               }
//               }
// 
// 
//             ],
//  'watches': [id1, id2, id3]
// }

function print_configuration() {
  datelog("Configuration");
  datelog("  Watches");
  datelog("      -> "+configuration['watches']);
  datelog("  Devices");
  for (var i = 0; i < configuration['devices'].length; i++) {
    datelog("      -> min_x "+configuration['devices'][i]['min_x']+"   max_x "+configuration['devices'][i]['max_x']);
    datelog("         min_y "+configuration['devices'][i]['min_y']+"   max_y "+configuration['devices'][i]['max_y']);
    datelog("         min_z "+configuration['devices'][i]['min_z']+"   max_z "+configuration['devices'][i]['max_z']);
    datelog("         Connections");
    for (var key in configuration['devices'][i]['connections']) {
      var str = "             -> ";
      str += "id "+key;
      str += ", x "+configuration['devices'][i]['connections'][key]['x'];
      str += ", y "+configuration['devices'][i]['connections'][key]['y'];
      str += ", z "+configuration['devices'][i]['connections'][key]['z'];
      var con = ['B', 'T', 'N', 'S', 'W', 'E'];
      for (var k = 0; k < con.length; k++) {
        str += ", "+con[k] + ": "+configuration['devices'][i]['connections'][key][con[k]];
      }
      datelog(str);
    }
  }
}

function update_device(k) {
  // device k might not be continuous anymore
  var keys = [];
  for (var key in configuration['devices'][k]['connections']) {
    keys.push(key);
  }
  if (keys.length == 0) {
    configuration['devices'].splice(k, 1);
    return;
  }

  var devices = [];
  while (keys.length > 0) {
    devices.push({});
    var in_process = [keys.pop()];
    while (in_process.length > 0) {
      var key = in_process.pop();
      devices[devices.length-1][key] = configuration['devices'][k]['connections'][key];

      var con = ['B', 'T', 'N', 'S', 'W', 'E'];
      for (var i = 0; i < con.length; i++) {
        if (configuration['devices'][k]['connections'][key][con[i]] != null) {
          in_process.push();
          var index = keys.indexOf(configuration['devices'][k]['connections'][key][con[i]]);
          if (index > -1) {
            keys.splice(index, 1);
            in_process.push(configuration['devices'][k]['connections'][key][con[i]]);
          }
        }
      }
    }
  }

  // check for the geometry
  configuration['devices'].splice(k, 1);
  for (var i = devices.length-1; i >= 0; i--) {
    var device = {'connections': devices[i]};
    var once = false;
    for (var con in device['connections']) {
      if (!once) {
        device['min_x'] = device['connections'][con]['x'];
        device['max_x'] = device['connections'][con]['x'];
        device['min_y'] = device['connections'][con]['y'];
        device['max_y'] = device['connections'][con]['y'];
        device['min_z'] = device['connections'][con]['z'];
        device['max_z'] = device['connections'][con]['z'];
        once = true;
      } else {
        device['min_x'] = Math.min(device['connections'][con]['x'],device['min_x']);
        device['max_x'] = Math.max(device['connections'][con]['x'],device['max_x']);
        device['min_y'] = Math.min(device['connections'][con]['y'],device['min_y']);
        device['max_y'] = Math.max(device['connections'][con]['y'],device['max_y']);
        device['min_z'] = Math.min(device['connections'][con]['z'],device['min_z']);
        device['max_z'] = Math.max(device['connections'][con]['z'],device['max_z']);
      }
    }
    configuration['devices'].splice(k, 0, device);
  }
}

function update_configuration(connection,watch) {
  if (watch != null)  {
    to_remove = null;
    to_add = null;

    if (!connection) {
      var index = configuration['watches'].indexOf(watch);
      if (index > -1) {
        to_remove = watch;
        configuration['watches'].splice(index, 1);
      }
    } else {
      var index = configuration['watches'].indexOf(watch);
      if (index > -1) {
        print_configuration();
        return;
      }
      to_add = watch;
      configuration['watches'].push(watch)
    }

    if (to_remove != null) {
      // remove watch
      for (var k = 0; k < configuration['devices'].length; k++) {
        if (to_remove in configuration['devices'][k]['connections']) {
          if (configuration['devices'][k]['connections'][to_remove]['B'] != null) {
            configuration['devices'][k]['connections'][configuration['devices'][k]['connections'][to_remove]['B']]['T'] = null;
          }
          if (configuration['devices'][k]['connections'][to_remove]['T'] != null) {
            configuration['devices'][k]['connections'][configuration['devices'][k]['connections'][to_remove]['T']]['B'] = null;
          }
          if (configuration['devices'][k]['connections'][to_remove]['N'] != null) {
            configuration['devices'][k]['connections'][configuration['devices'][k]['connections'][to_remove]['N']]['S'] = null;
          }
          if (configuration['devices'][k]['connections'][to_remove]['S'] != null) {
            configuration['devices'][k]['connections'][configuration['devices'][k]['connections'][to_remove]['S']]['N'] = null;
          }
          if (configuration['devices'][k]['connections'][to_remove]['W'] != null) {
            configuration['devices'][k]['connections'][configuration['devices'][k]['connections'][to_remove]['W']]['E'] = null;
          }
          if (configuration['devices'][k]['connections'][to_remove]['E'] != null) {
            configuration['devices'][k]['connections'][configuration['devices'][k]['connections'][to_remove]['E']]['W'] = null;
          }
          delete configuration['devices'][k]['connections'][to_remove];
          update_device(k);
          break;
        }
      }
    } else if (to_add != null) {
      // add new device as a new device
      device = {'min_x':0, 'max_x': 0, 'min_y':0, 'max_y':0, 'min_z':0, 'max_z':0, 'connections': {}};
      device['connections'][to_add] = {'x':0, 'y':0, 'z':0, 'B':null, 'T':null, 'N':null, 'S':null, 'E':null, 'W':null};
      configuration['devices'].push(device);
    }
  } else {
    // add / remove configurations

    // data["IMEI"]
    // data["side"]   // "North" "East" "South" "West" "Bottom"
    // data["active"] // "false" "true"

    while (side_msgs.length) {
      var side_msg = side_msgs.pop();
      var opposite = side_msg["side"];
      if (opposite == "North") {
        opposite = "South";
      } else if (opposite == "South") {
        opposite = "North";
      } else if (opposite == "East") {
        opposite = "West";
      } else if (opposite == "West") {
        opposite = "East";
      } else if (opposite == "Top") {
        opposite = "Bottom";
      } else if (opposite == "Bottom") {
        opposite = "Top";
      }

      var opp_msg = null;
      for (var k = 0; k < side_msgs.length; k++) {
        if ((side_msgs[k]["side"] == opposite) && (side_msgs[k]["active"] == side_msg["active"])) {
          opp_msg = side_msgs[k];
          break;
        }
      }

      if (opp_msg != null) {
        if (side_msg["active"]) {
          // check if devices already in the same
          // if not merge
          var device_index_a = -1;
          for (var i = 0; i < configuration['devices'].length; i++) {
            if (side_msg["IMEI"] in configuration['devices'][i]['connections']) {
              device_index_a = i;
              break;
            }
          }
          var device_index_b = -1;
          for (var i = 0; i < configuration['devices'].length; i++) {
            if (opp_msg["IMEI"] in configuration['devices'][i]['connections']) {
              device_index_b = i;
              break;
            }
          }

          if (device_index_a == device_index_b) {
            configuration['devices'][device_index_a]['connections'][side_msg["IMEI"]][side_msg["side"][0]] = opp_msg["IMEI"];
            configuration['devices'][device_index_a]['connections'][opp_msg["IMEI"]][opp_msg["side"][0]] = side_msg["IMEI"];

            // update the connections
            update_device(device_index_a);
          } else {
            var to_merge = configuration['devices'][device_index_b];
            configuration['devices'][device_index_a]['connections'][side_msg["IMEI"]][side_msg["side"][0]] = opp_msg["IMEI"];
            var dx = 0;
            var dy = 0;
            var dz = 0;
            if (side_msg["side"][0] == 'N') {
              dx = configuration['devices'][device_index_a]['connections'][side_msg["IMEI"]]['x'];
              dy = configuration['devices'][device_index_a]['connections'][side_msg["IMEI"]]['y']-1;
              dz = configuration['devices'][device_index_a]['connections'][side_msg["IMEI"]]['z'];
            } else if (side_msg["side"][0] == 'S') {
              dx = configuration['devices'][device_index_a]['connections'][side_msg["IMEI"]]['x'];
              dy = configuration['devices'][device_index_a]['connections'][side_msg["IMEI"]]['y']+1;
              dz = configuration['devices'][device_index_a]['connections'][side_msg["IMEI"]]['z'];
            } else if (side_msg["side"][0] == 'W') {
              dx = configuration['devices'][device_index_a]['connections'][side_msg["IMEI"]]['x']-1;
              dy = configuration['devices'][device_index_a]['connections'][side_msg["IMEI"]]['y'];
              dz = configuration['devices'][device_index_a]['connections'][side_msg["IMEI"]]['z'];
            } else if (side_msg["side"][0] == 'E') {
              dx = configuration['devices'][device_index_a]['connections'][side_msg["IMEI"]]['x']+1;
              dy = configuration['devices'][device_index_a]['connections'][side_msg["IMEI"]]['y'];
              dz = configuration['devices'][device_index_a]['connections'][side_msg["IMEI"]]['z'];
            } else if (side_msg["side"][0] == 'T') {
              dx = configuration['devices'][device_index_a]['connections'][side_msg["IMEI"]]['x'];
              dy = configuration['devices'][device_index_a]['connections'][side_msg["IMEI"]]['y'];
              dz = configuration['devices'][device_index_a]['connections'][side_msg["IMEI"]]['z']+1;
            } else if (side_msg["side"][0] == 'B') {
              dx = configuration['devices'][device_index_a]['connections'][side_msg["IMEI"]]['x'];
              dy = configuration['devices'][device_index_a]['connections'][side_msg["IMEI"]]['y'];
              dz = configuration['devices'][device_index_a]['connections'][side_msg["IMEI"]]['z']-1;
            }
            dx -= to_merge['connections'][opp_msg["IMEI"]]['x'];
            dy -= to_merge['connections'][opp_msg["IMEI"]]['y'];
            dz -= to_merge['connections'][opp_msg["IMEI"]]['z'];
            for (var imei in to_merge['connections']) {
              to_merge['connections'][imei]['x'] += dx;
              to_merge['connections'][imei]['y'] += dy;
              to_merge['connections'][imei]['z'] += dz;
              configuration['devices'][device_index_a]['connections'][imei] = to_merge['connections'][imei];
            }
            configuration['devices'][device_index_a]['connections'][opp_msg["IMEI"]][opp_msg["side"][0]] = side_msg["IMEI"];

            // update the connections
            update_device(device_index_a);
            configuration['devices'].splice(device_index_b, 1);
          }
        } else {
          for (var i = 0; i < configuration['devices'].length; i++) {
            if (side_msg["IMEI"] in configuration['devices'][i]['connections'] &&
                opp_msg["IMEI"] in configuration['devices'][i]['connections']) {
              configuration['devices'][i]['connections'][side_msg["IMEI"]][side_msg["side"][0]] = null;
              configuration['devices'][i]['connections'][opp_msg["IMEI"]][opp_msg["side"][0]] = null;
              update_device(i);
              break;
            }
          }
        }
      }
      // ignore bottom for now
    }
  }

  print_configuration();
}

function send_configuration_qt() {
  console.log(side_msgs);
  update_configuration(true,null);
  if (qt_client != null) {
    qt_client.write("conf:"+JSON.stringify(configuration,'utf8'));
  }
  buffer_qt = null;
}

function handle_side(message) {
  if (buffer_qt != null) {
    clearTimeout(buffer_qt);
  }
  side_msgs.push(message);
  buffer_qt = setTimeout(send_configuration_qt, 5000);
}

io.on('connection', function(socket){
  datelog("New client connected");

  list_watch_client[socket.id] = {'socket': socket}

  socket.emit('event', {"id": socket.id});

  socket.on('connected', function(data) {
      datelog("Client ("+data["IMEI"]+") says: "+data["Hi"]);
      list_watch_client[socket.id]['IMEI'] = data["IMEI"];
      list_imei_client[data["IMEI"]] = {'socket': socket };

      update_configuration(true,data["IMEI"]);
      send_configuration_qt();
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
    
  socket.on('sideChange', function(data) {
      datelog("Client ("+data["IMEI"]+") says: sideChange!");
      datelog("Client ("+data["IMEI"]+")        side: "+data["side"]);   // "North" "East" "South" "West" "Bottom"
      datelog("Client ("+data["IMEI"]+")      active: "+data["active"]); // "false" "true"

      handle_side(data);
  });

  socket.on('disconnect' , function() {
      datelog("Client disconnected");

      update_configuration(false,list_watch_client[socket.id]['IMEI']);
      send_configuration_qt();

      delete list_imei_client[list_watch_client[socket.id]['IMEI']];
      delete list_watch_client[socket.id];
  });
});


const net = require('net');

const PORT = 9001;
const ADDRESS = 'localhost';

let server = net.createServer(onClientConnected);
server.listen(PORT, ADDRESS);

function parse_buffer(data) {
    datelog("Data from Qt App received ("+Buffer(data).length+" bytes)");
    if (Buffer(data).length > 3) {
      var index = Buffer(data).indexOf(':',0,'utf8');
      if ((index >= 0) && (index < Buffer(data).length)) {
        var tag = Buffer(data).slice(0,index).toString('utf8');
        datelog("   -> tag "+tag);
        if (tag == "img") {
          data = Buffer(data).slice(index+1,Buffer(data).length);
          index = Buffer(data).indexOf(':',0,'utf8');
          if ((index >= 0) && (index < Buffer(data).length-1)) {
            var imei = Buffer(data).slice(0,index).toString('utf8');
            datelog("   -> img received, on instance to be send to "+imei);
            if (imei in list_imei_client) {
              var msg = Buffer(data).slice(index+1,Buffer(data).length);
              datelog("   -> img sent to "+imei+" ("+msg.length+" bytes)");
              list_imei_client[imei]['socket'].emit('qt', {"msg": JSON.stringify(msg,'utf8')});
            }
          }
        }
      }
    }
}

var prev_buf = null;
function onClientConnected(socket) {

  var clientName = 'Qt Client'
  datelog(clientName + ' connected.');
  qt_client = socket;

  send_configuration_qt();

  socket.on('data', (data) => {
    var new_data = true;
    while (new_data) {
      new_data = false;
      datelog("Original data received ("+Buffer(data).length+" bytes)");
      if (prev_buf != null) {
        data = Buffer.concat([prev_buf, data]);
      }
      datelog("Buffer adjustment ("+data.length+" bytes)");
      var index = Buffer(data).indexOf('\n',0,'utf8');
      if ((index >= 0) && (index < Buffer(data).length)) {
        parse_buffer(Buffer(data).slice(0,index));
        prev_buf = null;
        if (index == Buffer(data).length-1) {
          datelog("Buffer reset");
        } else {
          data = Buffer(data).slice(index+1,Buffer(data).length);
          datelog("Buffer trimmed ("+Buffer(data).length+" bytes)");
          new_data = true;
        }
      } else {
        prev_buf = data;
        datelog("Buffer carried ("+prev_buf.length+" bytes)");
      }
    }
  });
  
  socket.on('end', () => {
    datelog(clientName + ' disconnected.');
    qt_client = null;
  });
}

datelog('Server for Qt started at: ' + ADDRESS + ':' + PORT);




// Debug
// configuration = {
//   'devices':[{
//                 'min_x':0, 'max_x': 0, 'min_y':0, 'max_y':0, 'min_z':0, 'max_z':0,
//                 'connections': {
//                                   111: {'x':0, 'y':0, 'z':0, 'B':null, 'T':null, 'N':null, 'S':null, 'E':null, 'W':null}
//                                }
//              },
//              {
//                 'min_x':0, 'max_x': 1, 'min_y':-1, 'max_y':1, 'min_z':0, 'max_z':0,
//                 'connections': {
//                                   222: {'x':0, 'y':0, 'z':0, 'B':null, 'T':null, 'N':null, 'S':null, 'E':333, 'W':null},
//                                   333: {'x':1, 'y':0, 'z':0, 'B':null, 'T':null, 'N':444, 'S':777, 'E':null, 'W':222},
//                                   444: {'x':1, 'y':-1, 'z':0, 'B':null, 'T':null, 'N':null, 'S':333, 'E':null, 'W':null},
//                                   777: {'x':1, 'y':1, 'z':0, 'B':null, 'T':null, 'N':333, 'S':333, 'E':null, 'W':null}
//                                }
//              },
//              {
//                 'min_x':-1, 'max_x': -1, 'min_y':2, 'max_y':3, 'min_z':4, 'max_z':5,
//                 'connections': {
//                                   555: {'x':-1, 'y':2, 'z':4, 'B':null, 'T':666, 'N':null, 'S':888, 'E':null, 'W':null},
//                                   666: {'x':-1, 'y':2, 'z':5, 'B':555, 'T':null, 'N':null, 'S':null, 'E':null, 'W':null},
//                                   888: {'x':-1, 'y':3, 'z':4, 'B':555, 'T':null, 'N':555, 'S':null, 'E':null, 'W':null}
//                                }
//              }
//             ],
//   'watches': [111, 222, 333, 444, 555, 666, 777, 888]
// };
// print_configuration();