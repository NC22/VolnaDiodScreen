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
        console.log(bitPos);
        console.log('edit ' + bufferCursorByte + ' - ' + bufferCursorBit);
        
        sbController.buffer[bufferCursorByte] = sbController.setBit(sbController.buffer[bufferCursorByte], bufferCursorBit, val ? 1 : 0);
    }

sbController.init(8 * 4, 8 * 1);
sbController.setScreenBufferPixel(1, 0, 1); sbController.setScreenBufferPixel(2, 0, 1); sbController.setScreenBufferPixel(3, 0, 1);
console.log(sbController.buffer);

var wsController = {};
    wsController.gateway = 'ws://10.4.1.111:81';
    wsController.websocket;

    wsController.onOpen = function(event) {
        console.log('Connection opened');
    }

    wsController.onClose = function(event) {
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

    wsController.initWebSocket = function() {
        
        console.log('Trying to open a WebSocket connection...');
        
        wsController.websocket = new WebSocket(wsController.gateway);
        wsController.websocket.binaryType = "arraybuffer";
        
        wsController.websocket.onopen    = wsController.onOpen;
        wsController.websocket.onclose   = wsController.onClose;
        wsController.websocket.onmessage = wsController.onMessage; 
    }

    wsController.initWebSocket();