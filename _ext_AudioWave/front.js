/*

var port = chrome.runtime.connect({name: "downloader"}); 
var onMessage = function(request) {
                
    console.log(request);
    
}
            
port.onMessage.addListener(onMessage);                
port.onDisconnect.addListener(function() {
    
     console.log('[PORT] [Disconected] BG Process. Attempt to reconnect...');
});



var player = {};

chrome.runtime.onMessage.addListener((request, sender, callback) => {

   console.log(request);
   
console.log('----------------------qweqwe');
console.log('----------------------qweqweq');
console.log('----------------------eqwe');

  if (request.method == 'options.start.record') {    


        console.log(1111111);
        console.log(1111111);
        console.log(1111111);
        console.log(1111111);

        navigator.mediaDevices.getUserMedia({
            audio: {
                mandatory: {
                  chromeMediaSource: "tab",
                  chromeMediaSourceId: request.streamId,
                },
            },
            video: false,
        }).then((stream) => {
            
            context = new AudioContext();
            var audio = context.createMediaStreamSource(stream);
            audio.connect(context.destination);
        
            player.output = context;
            player.source = audio;
            
            player.stream = stream;
            
            player.fft = context.createAnalyser(); 
            player.fft.fftSize = 128; 
            player.fft.connect(context.destination);
            
                  
            setInterval(function() {

                  var waveform = new Float32Array(player.fft.frequencyBinCount);                          
                  player.fft.getFloatTimeDomainData(waveform);
                  player.waveform = waveform;
                  
                  console.log(player.waveform);
                  
            }, 2000);
        });

  }
        

  callback({});
});

*/
console.log('----------------------mememem');