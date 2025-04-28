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
            
            // Continue to play the captured audio to the user.
            player.output = new AudioContext();
            player.source = player.output.createMediaStreamSource(stream);
            player.source.connect(player.output.destination);
            player.stream = stream;
            
            player.fft = player.output.createAnalyser(); 
            player.fft.fftSize = 128; 
            player.fft.connect(player.output.destination);
                
        });

  }
        /*
        chrome.tabCapture.capture(
          {
            audio: true,
            video: false,
          },
          (stream) => {
            console.log(stream);
          }
        );
        */
        

  //callback({});
});

console.log('----------------------mememem');

/* Receive data from Current Tab or Background

var player = new Object();

function captureTabAudio() {
    chrome.tabCapture.capture({audio: true, video: false}, (stream) => {

        console.log(stream);
        player = new Object();
            player.buf;
            
            // these lines enable the audio to continue playing while capturing
            player.ctx = new AudioContext();
            
            player.stream = stream;
            
            var newStream = player.ctx.createMediaStreamSource(stream);
            newStream.connect(player.ctx.destination);
        
        
          player.src = stream; 
          player.src.buffer = player.buf; 
          
          // создаём fft 
          player.fft = player.ctx.createAnalyser(); 
          player.fft.fftSize = player.samples; 
          
          // соединяем в цепочку
          player.src.connect(player.fft); 
          player.fft.connect(player.ctx.destination); 
          
          // сразу воспроизводим
          // player.src.noteOn(0); 
          // player.src.start();
          // player.setup = true; 
          
          player.interval = setInterval(player.updateScreen, 100);
          

          player.updateScreen = function() { 
              
              var waveform = new Float32Array(player.fft.frequencyBinCount);                          
              player.fft.getFloatTimeDomainData(waveform);
              
              player.waveform = waveform;
        }
    })
}




console.log(chrome.tabCapture.capture);
console.log(chrome.tabCapture);
*/