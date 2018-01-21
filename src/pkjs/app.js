//var krakenUrl = "https://api.kraken.com/0/public/Ticker?pair=XETHZEUR,XXMRZEUR";

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log('PebbleKit JS ready!');
    processBTCValue();
  }
);

function httpGetASync(theUrl, fn) {

  var xhr = new XMLHttpRequest();
  xhr.open("GET", theUrl, true);
  xhr.onload = function (e) {
    if (xhr.readyState === 4) {
      if (xhr.status === 200) {
        fn(xhr.responseText);
      } else {
        console.error(xhr.statusText);
      }
    }
  };
  xhr.onerror = function (e) {
    console.error(xhr.statusText);
  };
  xhr.send(null);
}

function formatNumber(f) {
	return Math.round(f).toString().replace(/\B(?=(\d{3})+(?!\d))/g, ",");
}

function getBTCValueFromKrakenMessage(json) {
  var msg = JSON.parse(json);

  try {
    
    var items = ["XETHZEUR", "XXMRZEUR"];
    
    for(item in items) {
      
    //var XXBTZEUR = msg.result.XXBTZEUR;
    //var aBTC = parseFloat(XXBTZEUR.a[0]);
    //var bBTC = parseFloat(XXBTZEUR.b[0]);
      console.log(items[item]);
    }
    
    var XETHZEUR = msg.result["XETHZEUR"];
    var aETH = parseFloat(XETHZEUR.a[0]);
    var bETH = parseFloat(XETHZEUR.b[0]);
    
    var XXMRZEUR = msg.result.XXMRZEUR;
    var aXMR = parseFloat(XXMRZEUR.a[0]);
    var bXMR = parseFloat(XXMRZEUR.b[0]);
    
//    var btc_value = formatNumber((aBTC + bBTC) / 2.0);
    var eth_value = formatNumber((aETH + bETH) / 2.0);
    var xmr_value = formatNumber((aXMR + bXMR) / 2.0);
    
    return { 
      'I': "E:" + eth_value + "  M:" + xmr_value
    };
  } catch(error) {
    console.log(error);
    return { 
      'I': "ERR"
    };
  }
}

function sendBTCValueToPebble(values) {

      // Send to Pebble
    Pebble.sendAppMessage(values); //,
    //    function(e) {
//          console.log('BTC value info sent to Pebble successfully!');
//        },
//        function(e) {
//          console.log('Error sending BTC value info to Pebble!');
//        }
//      );
}

function processBTCValue() {

    httpGetASync(krakenUrl, function(response)
    {
      var v = getBTCValueFromKrakenMessage(response);
      sendBTCValueToPebble(v);
      //console.log("BTC: " + v.B2);
      //console.log("XMR: " + v.M2);
    });
}

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    processBTCValue();
  }                     
);