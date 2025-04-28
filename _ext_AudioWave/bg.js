
var onMessage = function(request, sender, callback) {
    
          var response = {
            
            senderId : 'bg',
            error : '',
            method : request.method,        
        };
        
       // console.log(request);
        
        
      //  console.log("BG ------ REQUEST ");
      //  console.log(request);

      //  console.log("BG ------");

      if (request.target === 'bg') {
   
        if (request.method == 'get-tab-info') {
            
            chrome.tabs.get(parseInt(request.tabId), function(tabData) {
                
                response.tabData = {id : tabData.id, title : tabData.title};
                
                callback(response);
            });
            
            return true;
            
        }
      
      }
          
    callback(response);
}


var updateIconState = function(state) {
       
   if (state == "idle" || state == "process") {
       chrome.action.setIcon({ path: 'icons/not-recording.png' });
   } else {
        chrome.action.setIcon({ path: 'icons/recording.png' });
   }
}

var startCapture = function(currentTab) {
    
    console.log(currentTab);
    
    chrome.tabCapture.getMediaStreamId({targetTabId: currentTab.id}).then(function(streamId) {
          
        chrome.runtime.sendMessage({
            
                method: 'start-recording',
                target: 'offscreen',
                streamId : streamId,
                title : currentTab.title,
                tabId : currentTab.id,
                
        }, function(response) {
                
           console.log(response);
           if (chrome.runtime.lastError) {
               console.log(chrome.runtime.lastError);
               return;
               
           }
           
           updateIconState(response.state);
        });

    });
}

var stopCapture = function(currentTab) {
     
  chrome.runtime.sendMessage({
            
            method: 'stop-recording',
            target: 'offscreen',
            
    }, function(response) {
            
       console.log(response);
       if (chrome.runtime.lastError) {
           console.log(chrome.runtime.lastError);
           return;
           
       }
       
       updateIconState(response.state);
    });
  
}

chrome.action.onClicked.addListener(async (currentTab) => {
    
      //console.log("Current Tab", currentTab);
      //console.log(currentTab);
        const existingContexts = await chrome.runtime.getContexts({});
        const offscreenDocument = existingContexts.find((c) => c.contextType === 'OFFSCREEN_DOCUMENT');
        if (!offscreenDocument) {
            
            await chrome.offscreen.createDocument({
                  url: 'offscreen.html',
                  reasons: ['USER_MEDIA'],
                  justification: 'Recording from chrome.tabCapture API',
                });
        } 
        
        
        chrome.runtime.sendMessage({
            
                method: 'get-stream-state',
                target: 'offscreen',
                
        }, function(response) {
            
            updateIconState(response.state);
            
            if (response.state == 'idle') {
                startCapture(currentTab);
            } else {
                stopCapture(currentTab);
            }
        });
        
        

});

chrome.tabs.onRemoved.addListener(function(tabId, removed) {
   
     chrome.runtime.sendMessage({
            
            method: 'tab-close',
            target: 'offscreen',
            tabId : tabId,
            
    }, function(response) {
            
       if (chrome.runtime.lastError) {
           console.log(chrome.runtime.lastError);
           return;
           
       }
       
       updateIconState(response.state);
    });
})

chrome.runtime.onMessage.addListener(onMessage);   
