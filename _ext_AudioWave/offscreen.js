var player = {
    
    streamId : false,
    title : false,
    tabId : false,
    
    state : "idle",
    
    // put your settings
    titleDisplayIP : false, // addition display for controll title
    waveDisplayIP : "127.0.0.1",
};

var onMessage = function(request, sender, callback) {
    
  var response = {
        
        senderId : 'offscreen',
        error : '',
        method : request.method,        
    };
    console.log(request);
  if (request.target === 'offscreen') {
          
        switch (request.method) {
            
          case 'start-recording':
            
            if (player.state == "process") {
                response.state = player.state;
                callback(response);
                return;
            }
            
            if (player.state == "record") {
                
                stopRecording();
            }   
        
            response.message = "record started";
            
            player.streamId = request.streamId;
            player.tabId = request.tabId;
            player.title = request.title;
            console.log("[startWaveCapture] : starting...");
            
            startWaveCapture(function(){ 
                
                response.state = player.state;
                
                console.log("[startWaveCapture] : result : " + response.state);
                callback(response);
            });
            
            return true;
            
            break;
            
          case 'get-stream-state':
            
            break;
            
          case 'tab-close':
            
            if (player.tabId == request.tabId) {
                stopRecording();
            }
            
            break;
             
          case 'stop-recording':
          
            stopRecording();
            break;
            
          default:
            throw new Error('Unrecognized message:', request.method);
        }
        
  }
  
  response.state = player.state;
  callback(response);
}


chrome.runtime.onMessage.addListener(onMessage);

// screen buffer object - to easy fill & work with pixels
var sbController = {};
    sbController.init = function(w, h) {

        sbController.screenWidth = w;
        sbController.screenHeight = h;
        sbController.bitPerPixel = 1;
        sbController.bufferLen = (sbController.screenWidth * sbController.screenHeight / 8) * sbController.bitPerPixel;
        sbController.buffer = new Uint8Array( sbController.bufferLen );
        console.log('Init screen buffer [' + sbController.screenWidth + 'x' + sbController.screenHeight + '] - Bytes : ' + sbController.bufferLen);
    }
    
    sbController.getBit = function(byte, position) {
    
        return (byte >> (7 - position)) & 0x1;
    }

    sbController.setBit = function(number, position, state){
    
        position = (7 - position);
        
        if (!state) {
            var mask = ~(1 << position);
            return number & mask;
        } else {
            return number | (1 << position) ;
        }        
    }

    sbController.setScreenBufferPixel = function(x, y, val) {
            
        x = Math.abs(x-sbController.screenWidth);
        y = Math.abs(y-sbController.screenHeight);
        
        var bitPos = (y * sbController.screenWidth) + x;  
            bitPos = sbController.bitPerPixel * bitPos;
        
        var bufferCursorByte = 0;
        var bufferCursorBit = 0;
        
        if (bitPos % 8) {
            
            bufferCursorByte = Math.floor(bitPos / 8);
            bufferCursorBit = bitPos - (bufferCursorByte * 8);
    
        } else {
    
            bufferCursorBit = 0;
            bufferCursorByte = bitPos / 8;
        }
        // console.log(bitPos);
        // console.log('edit ' + bufferCursorByte + ' - ' + bufferCursorBit);

        if (bufferCursorByte >= sbController.buffer.length) {

            console.log('Out of bounds ' + x + ' - ' + y);
        } else {
        sbController.buffer[bufferCursorByte] = sbController.setBit(sbController.buffer[bufferCursorByte], bufferCursorBit, val ? 1 : 0);
        }
    } 

//sbController.setScreenBufferPixel(1, 0, 1); sbController.setScreenBufferPixel(2, 0, 1); sbController.setScreenBufferPixel(3, 0, 1);
//console.log(sbController.buffer);

// web socket manager, keeps socket connection to led screen to keep send screen data on it
var wsController = {};

    wsController.titleDisplay = player.titleDisplayIP;
    wsController.waveDisplay = player.waveDisplayIP;
    wsController.origTitle = "";
    
    wsController.gateway = 'ws://' + wsController.waveDisplay + ':81';
    
    wsController.initSocketTimer = false;
    wsController.websocket = false;
    wsController.workEnd = true;

    wsController.onOpen = function(event) {
        console.log('Connection opened');
        wsController.connected = true;
    }

    wsController.onClose = function(event) {
        
        if (wsController.workEnd) return;
        
        wsController.websocket = false;
        console.log('Connection closed');
        setTimeout(wsController.initWebSocket, 2000);
    }

    wsController.onMessage = function(event) {
        var state;
        for (var i = 0; i < event.data.length; i++) {

            console.log(event.data[i]);
        }
        // document.getElementById('state').innerHTML = state;
    }
    
    wsController.updateTitle = function() {

        if (wsController.titleDisplay && player.state == "record") {
                 
            chrome.runtime.sendMessage({
                
                    method: 'get-tab-info',
                    target: 'bg',
                    tabId : player.tabId,
                    
            }, function(response) {
                    
               //  console.log(response);
               if (chrome.runtime.lastError) {
                   console.log(chrome.runtime.lastError);
                   return;
                   
               }
               
               var testTitle = response.tabData.title.toLowerCase();
               
                if (!wsController.origTitle || wsController.origTitle.localeCompare(testTitle) != 0 ) {
                     
                     wsController.origTitle = testTitle;
                     
                     var formData = new FormData();
                         formData.append("text", testTitle);
                         formData.append("duration", '3600');
                         formData.append("key", 'wavetitle');  
                         formData.append("scroll", '1');
                         
                    fetch('http://' + wsController.titleDisplay + '/api/notification', {
                            method: 'POST',
                            responseType : 'text',
                            body : formData,                                     
                   }).then(function(text) { console.log(text);   })
                    .catch(function(error) {
                        console.log("[updateTitle] + " + wsController.titleDisplay + " - unavailable");
                        console.log(error); 
                    });
                }
            });
            
        }
                        
    }

    wsController.initWebSocket = function(onReady) {
        
        if (wsController.websocket != false) {
            wsController.reset();
        } 
        
        wsController.workEnd = false;
        
        var formData = new FormData();
            formData.append("enable", '1');

        fetch('http://' + wsController.waveDisplay + '/api/direct/input', {
                    method: 'POST',
                    responseType : 'text',
                    body : formData,
                     
         }).then(function(response) {
                 
                response.text().then(function(responseFormated) { 
                    
                    wsController.initSocketTimer = setTimeout(function() {                        
                                        
                        console.log('Trying to open a WebSocket connection...');
                        
                        wsController.websocket = new WebSocket(wsController.gateway);
                        wsController.websocket.binaryType = "arraybuffer";
                        
                        wsController.websocket.onopen    = wsController.onOpen;
                        wsController.websocket.onclose   = wsController.onClose;
                        wsController.websocket.onmessage = wsController.onMessage; 
                        
                        if (wsController.titleDisplay) wsController.updTitleInterval = setInterval(wsController.updateTitle, 1000);
                        
                        onReady(true);
                    }, 1000);
                    
                    wsController.updateTitle();
                    console.log('Turn display to direct output mode');
                    console.log(responseFormated);   
                
                });
              
        }).then(function(text) { console.log(text);   })
        .catch(function(error) {
            onReady(false, error);
            console.log("Clock unavailable");
            console.log(error);
        });
        
    }
    
    wsController.setDisplayMode = function(addr, mode, time) {
        
         var formData = new FormData();
             formData.append("mode", mode);
        
        if (time) {
            formData.append("time", time);
        }
        
        fetch('http://' + addr + "/api/mode", {
                    method: 'POST',
                    responseType : 'text',
                    body : formData,
                     
         }).then(function(response) {
                 
                response.text().then(function(responseFormated) { console.log(responseFormated);   });
              
        }).then(function(text) { console.log(text);   })
        .catch(function(error) {
            console.log(addr + " - unavailable");
            console.log(error); 
        });
    }
    
    wsController.reset = function() {
        
        if (wsController.initSocketTimer) {
            clearTimeout(wsController.initSocketTimer);
            wsController.initSocketTimer = false;
        }
        
        if (wsController.updTitleInterval) { // todo - иногда не обнуляется или успевает сработать лишний раз
            clearInterval(wsController.updTitleInterval);
        }
        
        if (wsController.websocket) {      
            wsController.websocket.close();
        }
        
        wsController.origTitle = "";
        wsController.websocket = false;
        wsController.connected = false;
        wsController.workEnd = true;
        
        wsController.setDisplayMode(wsController.waveDisplay, "date");
        
        if (wsController.titleDisplay) {
            wsController.setDisplayMode(wsController.titleDisplay, "time");
        }
    }

function startWaveCapture(onReady) {

  
  console.log("start record " + player.streamId);
  player.state = "process";
    
  navigator.mediaDevices.getUserMedia({
    audio: {
      mandatory: {
        chromeMediaSource: 'tab',
        chromeMediaSourceId: player.streamId
      }
    },
    video: false,
  }).then(function (stream) {

      // Continue to play the captured audio to the user.
      const output = new AudioContext();
      const source = output.createMediaStreamSource(stream);
      source.connect(output.destination);
        
        player.stream = stream;
        
        player.fft = output.createAnalyser(); 
        //player.fft.connect(output.destination);
        
        source.connect(player.fft);
        
        player.maxLevel = [];
        player.levelCheckerDt = 1000;
        player.levelCheckerDtStep = 1000;
        player.checkDevider = function() {
            
            if (player.maxLevel.length < 10) return 2;
            var max = 0;
            
            for(var i=0; i < player.maxLevel.length; i++) { 
                
                if (max < player.maxLevel[i]) max = player.maxLevel[i];
            }
            
            if (max < 7.6) return 0.5;
            else return 2;
        }
        
        player.controller = setInterval(function() {
                
              if (wsController.connected) {
                  
                  var waveform = new Float32Array(player.fft.frequencyBinCount);                          
                  player.fft.getFloatTimeDomainData(waveform);
                  player.waveform = waveform;
                  
                  var tt = '';
                  for (var i = 0; i < player.waveform.length; i++) {
                      tt += player.waveform[i];              
                  }
                  
                  // console.log(tt);
                  
                  for (var i = 0; i < sbController.bufferLen; i++) {
                     sbController.buffer[i] = 0;
                  }
                    
                  var pt = 1 / 100;
                  var pixelsY = 10;
                  var ptScreen = pixelsY / 100;
                  
                  var length = waveform.length;
                  var pixelsX = 32;
                  var step = Math.round(length / pixelsX);
                  var x = 1;
                  player.decMp = 2; // todo - better change manually if needed
                  
                  player.levelCheckerDt -= 100;
                                    
                  var maxLevelLocal = 0;
                  for(var i=0; i < length; i+=step) { 
                        
                        var val = (waveform[i] / player.decMp) + 0.5; // ~ from 0 to 1
                        if (val < 0) val = 0;
                        
                        var percent = (val) / pt;
                        if (percent > 100) percent = 100;
                        
                        var yMax = Math.round(ptScreen * percent);
                        
                        var lvl =  Math.abs(percent - 50); // Math.abs((((waveform[i] / 2) + 0.5) / pt) - 50); // same formula but static devided by to to detect if increase devider needed                    
                        if (lvl > maxLevelLocal) maxLevelLocal = lvl;
                        
                        if (yMax >= 1) {
                            
                            sbController.setScreenBufferPixel(x, yMax, 1);
                        }
                        
                        x++;
                  }
                  
                  /*
                  if (player.levelCheckerDt <= 0) {
                      
                      player.levelCheckerDt = player.levelCheckerDtStep;
                      // console.log(player.maxLevel);
                      // console.log(player.checkDevider());
                      player.maxLevel.push(maxLevelLocal);
                    
                      if (player.maxLevel.length > 10) {
                          player.maxLevel.shift();
                      }
                  }
                  */
                  
                  wsController.websocket.send( sbController.buffer );
              }
              
        }, 100);
        
        sbController.init(8 * 4, 8 * 1);
        wsController.initWebSocket(function(error, desc) {
            
            window.location.hash = 'recording';
            
            if (error) {
                // wsController.websocket.send(sbController.buffer);
                player.state = "record";
            } else {
                
                console.log("Error record ");
                console.log(desc);
                stopRecording();
            } 
            
            onReady();          
        });      
  });

}

async function stopRecording() {
    
    if (player.state != "record") return;
    
    clearInterval(player.controller);
    player.state = "idle";
    if (player.stream) {
        player.stream.getTracks().forEach((t) => t.stop());
    }
            
    wsController.reset();
    window.location.hash = '';
}