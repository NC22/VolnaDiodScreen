var player = {
    
    streamId : false,
    title : false,
    tabId : false,
    
    state : "idle",
    
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
            
            if (player.state == "record") {
                
                stopRecording();
            }   
        
            response.message = "record started";
            
            player.streamId = request.streamId;
            player.tabId = request.tabId;
            player.title = request.title;
            
            startWaveCapture(function(){ 
                
                response.state = player.state;
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


function startWaveCapture(onReady) {

  
  console.log("start record " + player.streamId);
    
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
        
        player.controller = setInterval(function() {

              var waveform = new Float32Array(player.fft.frequencyBinCount);                          
              player.fft.getFloatTimeDomainData(waveform);
              player.waveform = waveform;
              
              var tt = '';
              for (var i = 0; i < player.waveform.length; i++) {
                  tt += player.waveform[i];              
              }
              
              console.log(tt);
              
        }, 2000);
        
        player.state = "record";
        
      window.location.hash = 'recording';
      onReady();
  });

}

async function stopRecording() {
    
    if (player.state != "record") return;
    
    clearInterval(player.controller);
    player.state = "idle";
    if (player.stream) {
        player.stream.getTracks().forEach((t) => t.stop());
    }
    
    window.location.hash = '';
}